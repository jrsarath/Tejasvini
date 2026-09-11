#include "Protocol.h"
#include "Serialization.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const char* command_type_to_string(CommandType type) {
    switch (type) {
        case CMD_PING:          return "PING";
        case CMD_GET_STATUS:     return "GET_STATUS";
        case CMD_START_PROFILE: return "START_PROFILE";
        case CMD_STOP:          return "STOP";
        case CMD_PAUSE:         return "PAUSE";
        case CMD_RESUME:        return "RESUME";
        case CMD_CLEAR_FAULT:   return "CLEAR_FAULT";
        case CMD_SET_TARGET:    return "SET_TARGET";
        case CMD_SET_FAN_MODE:  return "SET_FAN_MODE";
        default:                return "UNKNOWN";
    }
}

CommandType string_to_command_type(const char* str) {
    if (!str) return CMD_UNKNOWN;
    if (strcmp(str, "PING") == 0)          return CMD_PING;
    if (strcmp(str, "GET_STATUS") == 0)     return CMD_GET_STATUS;
    if (strcmp(str, "START_PROFILE") == 0) return CMD_START_PROFILE;
    if (strcmp(str, "STOP") == 0)          return CMD_STOP;
    if (strcmp(str, "PAUSE") == 0)         return CMD_PAUSE;
    if (strcmp(str, "RESUME") == 0)        return CMD_RESUME;
    if (strcmp(str, "CLEAR_FAULT") == 0)   return CMD_CLEAR_FAULT;
    if (strcmp(str, "SET_TARGET") == 0)    return CMD_SET_TARGET;
    if (strcmp(str, "SET_FAN_MODE") == 0)  return CMD_SET_FAN_MODE;
    return CMD_UNKNOWN;
}

static bool is_numeric_str(const char* s) {
    if (!s || *s == '\0') return false;
    if (*s == '-' || *s == '+') s++;
    while (*s) {
        if (*s < '0' || *s > '9') return false;
        s++;
    }
    return true;
}

bool protocol_parse_command(const char* line, CommandPacket* out_pkt) {
    if (!line || !out_pkt) return false;

    memset(out_pkt, 0, sizeof(CommandPacket));
    char buf[PROTOCOL_MAX_FRAME_LEN];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* tokens[PROTOCOL_CMD_MAX_ARGS];
    int token_count = tokenize_line(buf, tokens, PROTOCOL_CMD_MAX_ARGS);
    if (token_count <= 0) return false;

    int idx = 0;
    uint32_t seq = 0;

    // Optional leading "CMD" tag
    if (strcmp(tokens[idx], "CMD") == 0) {
        idx++;
        if (idx >= token_count) return false;
    }

    // Optional numeric sequence ID
    if (is_numeric_str(tokens[idx])) {
        seq = (uint32_t)strtoul(tokens[idx], NULL, 10);
        idx++;
        if (idx >= token_count) return false;
    }

    out_pkt->seq_id = seq;
    out_pkt->type = string_to_command_type(tokens[idx]);
    idx++;

    if (out_pkt->type == CMD_UNKNOWN) {
        return false;
    }

    switch (out_pkt->type) {
        case CMD_PING:
        case CMD_GET_STATUS:
        case CMD_STOP:
        case CMD_PAUSE:
        case CMD_RESUME:
        case CMD_CLEAR_FAULT:
            return true;

        case CMD_START_PROFILE:
            if (idx < token_count) {
                out_pkt->int_arg1 = atoi(tokens[idx]);
                if (out_pkt->int_arg1 < 0 || out_pkt->int_arg1 > 3) {
                    return false; // Invalid profile index
                }
                return true;
            }
            return false;

        case CMD_SET_TARGET:
            if (idx < token_count) {
                out_pkt->int_arg1 = atoi(tokens[idx]);
                if (out_pkt->int_arg1 < MIN_SETPOINT_TEMP || out_pkt->int_arg1 > MAX_SETPOINT_TEMP) {
                    return false; // Out of valid setpoint range
                }
                return true;
            }
            return false;

        case CMD_SET_FAN_MODE:
            if (idx < token_count) {
                out_pkt->int_arg1 = atoi(tokens[idx++]); // Fan ID (1 or 2)
            }
            if (idx < token_count) {
                out_pkt->int_arg2 = atoi(tokens[idx++]); // Fan mode (0=AUTO, 1=MANUAL, 2=OFF)
            }
            if (idx < token_count) {
                out_pkt->float_arg1 = (float)atof(tokens[idx++]); // Optional PWM %
            }
            return true;

        default:
            return true;
    }
}

bool protocol_parse_status(const char* line, StatusPacket* out_status) {
    if (!line || !out_status) return false;

    char buf[PROTOCOL_MAX_FRAME_LEN * 2];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* tokens[32];
    int token_count = tokenize_line(buf, tokens, 32);
    if (token_count < 1) return false;

    if (strcmp(tokens[0], "STATUS") != 0) return false;

    memset(out_status, 0, sizeof(StatusPacket));
    out_status->ntc3_temp = -99.0f; // Default unpopulated

    char key[32];
    char val[32];

    for (int i = 1; i < token_count; i++) {
        if (!parse_key_value(tokens[i], key, sizeof(key), val, sizeof(val))) {
            continue;
        }

        if (strcmp(key, "ver") == 0) {
            out_status->protocol_version = (uint8_t)atoi(val);
        } else if (strcmp(key, "mcu") == 0) {
            strncpy(out_status->firmware_version, val, sizeof(out_status->firmware_version) - 1);
        } else if (strcmp(key, "state") == 0) {
            out_status->state = string_to_machine_state(val);
        } else if (strcmp(key, "prof") == 0) {
            out_status->profile = string_to_reflow_profile(val);
        } else if (strcmp(key, "stage") == 0) {
            out_status->stage = string_to_profile_stage(val);
        } else if (strcmp(key, "prog") == 0) {
            out_status->progress = (int32_t)atol(val);
        } else if (strcmp(key, "elap") == 0) {
            out_status->elapsed_s = (uint32_t)strtoul(val, NULL, 10);
        } else if (strcmp(key, "rem") == 0) {
            out_status->target_s = (uint32_t)strtoul(val, NULL, 10);
        } else if (strcmp(key, "t_tgt") == 0) {
            out_status->target_temp = atoi(val);
        } else if (strcmp(key, "t1") == 0) {
            out_status->ntc1_temp = (float)atof(val);
        } else if (strcmp(key, "t2") == 0) {
            out_status->ntc2_temp = (float)atof(val);
        } else if (strcmp(key, "t3") == 0) {
            out_status->ntc3_temp = (float)atof(val);
        } else if (strcmp(key, "heat") == 0) {
            out_status->heater_on = (atoi(val) != 0);
        } else if (strcmp(key, "duty") == 0) {
            out_status->duty_cycle = (float)atof(val);
        } else if (strcmp(key, "f1_pwm") == 0) {
            out_status->fan1_pwm = (uint8_t)atoi(val);
        } else if (strcmp(key, "f1_rpm") == 0) {
            out_status->fan1_rpm = (uint16_t)atoi(val);
        } else if (strcmp(key, "f2_pwm") == 0) {
            out_status->fan2_pwm = (uint8_t)atoi(val);
        } else if (strcmp(key, "f2_rpm") == 0) {
            out_status->fan2_rpm = (uint16_t)atoi(val);
        } else if (strcmp(key, "fault") == 0) {
            out_status->fault_code = (FaultCode)atoi(val);
        } else if (strcmp(key, "sev") == 0) {
            out_status->fault_severity = (FaultSeverity)atoi(val);
        } else if (strcmp(key, "uptime") == 0) {
            out_status->uptime_s = (uint32_t)strtoul(val, NULL, 10);
        }
    }
    return true;
}

bool protocol_parse_response(const char* line, ResponsePacket* out_resp) {
    if (!line || !out_resp) return false;

    char buf[PROTOCOL_MAX_FRAME_LEN];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* tokens[8];
    int count = tokenize_line(buf, tokens, 8);
    if (count < 1) return false;

    memset(out_resp, 0, sizeof(ResponsePacket));

    if (strcmp(tokens[0], "PONG") == 0) {
        out_resp->is_ack = true;
        strncpy(out_resp->message, "PONG", sizeof(out_resp->message) - 1);
        return true;
    }

    if (strcmp(tokens[0], "RESP") == 0) {
        if (count < 3) return false;
        out_resp->seq_id = (uint32_t)strtoul(tokens[1], NULL, 10);
        out_resp->is_ack = (strcmp(tokens[2], "ACK") == 0);
        if (count >= 4) {
            strncpy(out_resp->message, tokens[3], sizeof(out_resp->message) - 1);
        }
        return true;
    }

    return false;
}

bool protocol_parse_fault(const char* line, FaultPacket* out_fault) {
    if (!line || !out_fault) return false;

    char buf[PROTOCOL_MAX_FRAME_LEN];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char* tokens[8];
    int count = tokenize_line(buf, tokens, 8);
    if (count < 3) return false;

    if (strcmp(tokens[0], "FAULT") != 0) return false;

    memset(out_fault, 0, sizeof(FaultPacket));
    out_fault->code = string_to_fault_code(tokens[1]);
    out_fault->severity = string_to_fault_severity(tokens[2]);
    if (count >= 4) {
        strncpy(out_fault->description, tokens[3], sizeof(out_fault->description) - 1);
    }
    return true;
}
