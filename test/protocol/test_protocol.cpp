#include <cassert>
#include <cstdio>
#include <cstring>
#include "Protocol.h"
#include "Serialization.h"

void test_parse_commands() {
    CommandPacket pkt;

    // 1. Valid PING command
    assert(protocol_parse_command("CMD 101 PING", &pkt));
    assert(pkt.seq_id == 101);
    assert(pkt.type == CMD_PING);

    // 2. Plain PING without CMD tag or seq
    assert(protocol_parse_command("PING", &pkt));
    assert(pkt.type == CMD_PING);

    // 3. GET_STATUS
    assert(protocol_parse_command("CMD 102 GET_STATUS", &pkt));
    assert(pkt.seq_id == 102);
    assert(pkt.type == CMD_GET_STATUS);

    // 4. Valid START_PROFILE
    assert(protocol_parse_command("CMD 103 START_PROFILE 1", &pkt));
    assert(pkt.seq_id == 103);
    assert(pkt.type == CMD_START_PROFILE);
    assert(pkt.int_arg1 == 1);

    // 5. Invalid START_PROFILE (out of range profile index)
    assert(!protocol_parse_command("CMD 104 START_PROFILE 99", &pkt));

    // 6. Valid SET_TARGET
    assert(protocol_parse_command("CMD 105 SET_TARGET 215", &pkt));
    assert(pkt.seq_id == 105);
    assert(pkt.type == CMD_SET_TARGET);
    assert(pkt.int_arg1 == 215);

    // 7. Invalid SET_TARGET (out of range temperature)
    assert(!protocol_parse_command("CMD 106 SET_TARGET 350", &pkt));
    assert(!protocol_parse_command("CMD 107 SET_TARGET -10", &pkt));

    // 8. STOP, PAUSE, RESUME, CLEAR_FAULT
    assert(protocol_parse_command("CMD 108 STOP", &pkt));
    assert(pkt.type == CMD_STOP);

    assert(protocol_parse_command("CMD 109 PAUSE", &pkt));
    assert(pkt.type == CMD_PAUSE);

    assert(protocol_parse_command("CMD 110 RESUME", &pkt));
    assert(pkt.type == CMD_RESUME);

    assert(protocol_parse_command("CMD 111 CLEAR_FAULT", &pkt));
    assert(pkt.type == CMD_CLEAR_FAULT);

    // 9. Unknown command rejection
    assert(!protocol_parse_command("CMD 112 EXPLODE_HEATER", &pkt));

    // 10. Empty string handling
    assert(!protocol_parse_command("", &pkt));
    assert(!protocol_parse_command(nullptr, &pkt));

    printf("[PASS] test_parse_commands\n");
}

void test_serialize_and_parse_response() {
    ResponsePacket resp;
    resp.seq_id = 42;
    resp.is_ack = true;
    strncpy(resp.message, "OK", sizeof(resp.message));

    char buf[128];
    int len = serialize_response(&resp, buf, sizeof(buf));
    assert(len > 0);
    assert(strcmp(buf, "RESP 42 ACK OK\n") == 0);

    ResponsePacket parsed;
    assert(protocol_parse_response(buf, &parsed));
    assert(parsed.seq_id == 42);
    assert(parsed.is_ack == true);
    assert(strcmp(parsed.message, "OK") == 0);

    // NACK response
    resp.is_ack = false;
    strncpy(resp.message, "ERR_INVALID_PARAM", sizeof(resp.message));
    serialize_response(&resp, buf, sizeof(buf));
    assert(strcmp(buf, "RESP 42 NACK ERR_INVALID_PARAM\n") == 0);

    assert(protocol_parse_response(buf, &parsed));
    assert(parsed.seq_id == 42);
    assert(parsed.is_ack == false);
    assert(strcmp(parsed.message, "ERR_INVALID_PARAM") == 0);

    // PONG response
    assert(protocol_parse_response("PONG\n", &parsed));
    assert(parsed.is_ack == true);
    assert(strcmp(parsed.message, "PONG") == 0);

    printf("[PASS] test_serialize_and_parse_response\n");
}

void test_fault_packets() {
    FaultPacket fault;
    fault.code = FAULT_OVERTEMP;
    fault.severity = SEVERITY_CRITICAL;
    strncpy(fault.description, "T_meas > 280C", sizeof(fault.description));

    char buf[128];
    serialize_fault(&fault, buf, sizeof(buf));
    assert(strcmp(buf, "FAULT OVERTEMP CRITICAL T_meas > 280C\n") == 0);

    FaultPacket parsed;
    assert(protocol_parse_fault(buf, &parsed));
    assert(parsed.code == FAULT_OVERTEMP);
    assert(parsed.severity == SEVERITY_CRITICAL);

    printf("[PASS] test_fault_packets\n");
}

int main() {
    printf("Running Protocol Unit Tests...\n");
    test_parse_commands();
    test_serialize_and_parse_response();
    test_fault_packets();
    printf("All Protocol Unit Tests Passed Successfully!\n");
    return 0;
}
