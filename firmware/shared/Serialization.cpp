#include "Serialization.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int serialize_command(const CommandPacket* pkt, char* buf, size_t buf_size) {
    if (!pkt || !buf || buf_size == 0) return 0;

    const char* cmd_name = command_type_to_string(pkt->type);
    if (!cmd_name) cmd_name = "UNKNOWN";

    switch (pkt->type) {
        case CMD_PING:
        case CMD_GET_STATUS:
        case CMD_STOP:
        case CMD_PAUSE:
        case CMD_RESUME:
        case CMD_CLEAR_FAULT:
            return snprintf(buf, buf_size, "CMD %lu %s\n", (unsigned long)pkt->seq_id, cmd_name);

        case CMD_START_PROFILE:
            return snprintf(buf, buf_size, "CMD %lu %s %d\n", (unsigned long)pkt->seq_id, cmd_name, pkt->int_arg1);

        case CMD_SET_TARGET:
            return snprintf(buf, buf_size, "CMD %lu %s %d\n", (unsigned long)pkt->seq_id, cmd_name, pkt->int_arg1);

        case CMD_SET_FAN_MODE:
            return snprintf(buf, buf_size, "CMD %lu %s %d %d %d\n",
                            (unsigned long)pkt->seq_id, cmd_name, pkt->int_arg1, pkt->int_arg2, (int)pkt->float_arg1);

        default:
            if (pkt->raw_args[0] != '\0') {
                return snprintf(buf, buf_size, "CMD %lu %s %s\n", (unsigned long)pkt->seq_id, cmd_name, pkt->raw_args);
            }
            return snprintf(buf, buf_size, "CMD %lu %s\n", (unsigned long)pkt->seq_id, cmd_name);
    }
}

int serialize_response(const ResponsePacket* pkt, char* buf, size_t buf_size) {
    if (!pkt || !buf || buf_size == 0) return 0;
    const char* status = pkt->is_ack ? "ACK" : "NACK";
    if (pkt->message[0] != '\0') {
        return snprintf(buf, buf_size, "RESP %lu %s %s\n", (unsigned long)pkt->seq_id, status, pkt->message);
    }
    return snprintf(buf, buf_size, "RESP %lu %s\n", (unsigned long)pkt->seq_id, status);
}

int serialize_status(const StatusPacket* pkt, char* buf, size_t buf_size) {
    if (!pkt || !buf || buf_size == 0) return 0;

    return snprintf(buf, buf_size,
        "STATUS ver=%u mcu=%s state=%s prof=%s stage=%s prog=%ld elap=%lu rem=%lu "
        "t_tgt=%d t1=%.1f t2=%.1f t3=%.1f heat=%d duty=%.1f "
        "f1_pwm=%u f1_rpm=%u f2_pwm=%u f2_rpm=%u fault=%d sev=%d uptime=%lu\n",
        (unsigned int)pkt->protocol_version,
        pkt->firmware_version[0] ? pkt->firmware_version : "v1.1.0",
        machine_state_to_string(pkt->state),
        reflow_profile_to_string(pkt->profile),
        profile_stage_to_string(pkt->stage),
        (long)pkt->progress,
        (unsigned long)pkt->elapsed_s,
        (unsigned long)pkt->target_s,
        pkt->target_temp,
        pkt->ntc1_temp,
        pkt->ntc2_temp,
        pkt->ntc3_temp,
        pkt->heater_on ? 1 : 0,
        pkt->duty_cycle,
        (unsigned int)pkt->fan1_pwm,
        (unsigned int)pkt->fan1_rpm,
        (unsigned int)pkt->fan2_pwm,
        (unsigned int)pkt->fan2_rpm,
        (int)pkt->fault_code,
        (int)pkt->fault_severity,
        (unsigned long)pkt->uptime_s
    );
}

int serialize_fault(const FaultPacket* pkt, char* buf, size_t buf_size) {
    if (!pkt || !buf || buf_size == 0) return 0;
    return snprintf(buf, buf_size, "FAULT %s %s %s\n",
                    fault_code_to_string(pkt->code),
                    fault_severity_to_string(pkt->severity),
                    pkt->description[0] ? pkt->description : "Unspecified");
}

int serialize_event(const EventPacket* pkt, char* buf, size_t buf_size) {
    if (!pkt || !buf || buf_size == 0) return 0;
    if (pkt->event_data[0] != '\0') {
        return snprintf(buf, buf_size, "EVENT %s %s\n", pkt->event_name, pkt->event_data);
    }
    return snprintf(buf, buf_size, "EVENT %s\n", pkt->event_name);
}

int tokenize_line(char* line, char* tokens[], int max_tokens) {
    if (!line || !tokens || max_tokens <= 0) return 0;
    int count = 0;
    char* p = line;

    // Strip carriage return and newline if present
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n')) {
        line[len - 1] = '\0';
        len--;
    }

    while (*p != '\0' && count < max_tokens) {
        // Skip whitespace
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;

        tokens[count++] = p;

        // Advance to next whitespace or end
        while (*p != '\0' && *p != ' ' && *p != '\t') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
    }
    return count;
}

bool parse_key_value(const char* token, char* key, size_t key_len, char* val, size_t val_len) {
    if (!token || !key || !val || key_len == 0 || val_len == 0) return false;
    const char* eq = strchr(token, '=');
    if (!eq) return false;

    size_t k_len = eq - token;
    if (k_len >= key_len) k_len = key_len - 1;
    strncpy(key, token, k_len);
    key[k_len] = '\0';

    const char* v_start = eq + 1;
    size_t v_len = strlen(v_start);
    if (v_len >= val_len) v_len = val_len - 1;
    strncpy(val, v_start, v_len);
    val[v_len] = '\0';

    return true;
}
