#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "Protocol.h"
#include "Serialization.h"

void test_status_round_trip() {
    StatusPacket src;
    memset(&src, 0, sizeof(StatusPacket));
    src.protocol_version = TEJASVINI_PROTOCOL_VERSION;
    strncpy(src.firmware_version, TEJASVINI_FIRMWARE_VERSION, sizeof(src.firmware_version));
    src.state = STATE_HEATING;
    src.profile = PROFILE_LEAD_FREE;
    src.stage = STAGE_PREHEAT;
    src.progress = 150;
    src.elapsed_s = 25;
    src.target_s = 90;
    src.target_temp = 150;
    src.ntc1_temp = 85.3f;
    src.ntc2_temp = 84.9f;
    src.ntc3_temp = -99.0f;
    src.heater_on = true;
    src.duty_cycle = 38.5f;
    src.fan1_pwm = 20;
    src.fan1_rpm = 1200;
    src.fan2_pwm = 0;
    src.fan2_rpm = 0;
    src.fault_code = FAULT_NONE;
    src.fault_severity = SEVERITY_NONE;
    src.uptime_s = 142;

    char buf[512];
    int len = serialize_status(&src, buf, sizeof(buf));
    assert(len > 0);
    assert(strncmp(buf, "STATUS ", 7) == 0);

    StatusPacket dst;
    assert(protocol_parse_status(buf, &dst));

    assert(dst.protocol_version == src.protocol_version);
    assert(strcmp(dst.firmware_version, src.firmware_version) == 0);
    assert(dst.state == STATE_HEATING);
    assert(dst.profile == PROFILE_LEAD_FREE);
    assert(dst.stage == STAGE_PREHEAT);
    assert(dst.progress == 150);
    assert(dst.elapsed_s == 25);
    assert(dst.target_s == 90);
    assert(dst.target_temp == 150);
    assert(std::fabs(dst.ntc1_temp - 85.3f) < 0.15f);
    assert(std::fabs(dst.ntc2_temp - 84.9f) < 0.15f);
    assert(std::fabs(dst.ntc3_temp - (-99.0f)) < 0.15f);
    assert(dst.heater_on == true);
    assert(std::fabs(dst.duty_cycle - 38.5f) < 0.15f);
    assert(dst.fan1_pwm == 20);
    assert(dst.fan1_rpm == 1200);
    assert(dst.fan2_pwm == 0);
    assert(dst.fan2_rpm == 0);
    assert(dst.fault_code == FAULT_NONE);
    assert(dst.fault_severity == SEVERITY_NONE);
    assert(dst.uptime_s == 142);

    printf("[PASS] test_status_round_trip\n");
}

void test_key_value_parser() {
    char key[32];
    char val[32];

    assert(parse_key_value("t1=123.4", key, sizeof(key), val, sizeof(val)));
    assert(strcmp(key, "t1") == 0);
    assert(strcmp(val, "123.4") == 0);

    assert(!parse_key_value("no_equal_sign", key, sizeof(key), val, sizeof(val)));
    assert(!parse_key_value("", key, sizeof(key), val, sizeof(val)));
    assert(!parse_key_value(nullptr, key, sizeof(key), val, sizeof(val)));

    printf("[PASS] test_key_value_parser\n");
}

int main() {
    printf("Running Serialization Unit Tests...\n");
    test_status_round_trip();
    test_key_value_parser();
    printf("All Serialization Unit Tests Passed Successfully!\n");
    return 0;
}
