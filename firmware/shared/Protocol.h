#ifndef TEJASVINI_SHARED_PROTOCOL_H_
#define TEJASVINI_SHARED_PROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "Config.h"
#include "Types.h"
#include "ErrorCodes.h"
#include "ProtocolVersion.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CMD_UNKNOWN = 0,
    CMD_PING,
    CMD_GET_STATUS,
    CMD_START_PROFILE,
    CMD_STOP,
    CMD_PAUSE,
    CMD_RESUME,
    CMD_CLEAR_FAULT,
    CMD_SET_TARGET,
    CMD_SET_FAN_MODE
} CommandType;

typedef enum {
    MSG_TYPE_UNKNOWN = 0,
    MSG_TYPE_CMD,
    MSG_TYPE_ACK,
    MSG_TYPE_NACK,
    MSG_TYPE_PONG,
    MSG_TYPE_STATUS,
    MSG_TYPE_FAULT,
    MSG_TYPE_EVENT
} MessageType;

typedef struct {
    uint32_t seq_id;
    CommandType type;
    int int_arg1;
    int int_arg2;
    float float_arg1;
    char raw_args[64];
} CommandPacket;

typedef struct {
    uint8_t protocol_version;
    char firmware_version[16];
    MachineState state;
    ReflowProfile profile;
    ProfileStage stage;
    int32_t progress;        // 0 - 1000
    uint32_t elapsed_s;
    uint32_t target_s;
    int target_temp;         // °C
    float ntc1_temp;         // °C
    float ntc2_temp;         // °C
    float ntc3_temp;         // °C (-99.0f if unpopulated)
    bool heater_on;
    float duty_cycle;        // %
    uint8_t fan1_pwm;        // %
    uint16_t fan1_rpm;
    uint8_t fan2_pwm;        // %
    uint16_t fan2_rpm;
    FaultCode fault_code;
    FaultSeverity fault_severity;
    uint32_t uptime_s;
} StatusPacket;

typedef struct {
    uint32_t seq_id;
    bool is_ack;
    char message[64];
} ResponsePacket;

typedef struct {
    FaultCode code;
    FaultSeverity severity;
    char description[64];
} FaultPacket;

typedef struct {
    char event_name[32];
    char event_data[64];
} EventPacket;

const char* command_type_to_string(CommandType type);
CommandType string_to_command_type(const char* str);

/**
 * @brief Parses an inbound line buffer into a CommandPacket.
 * @param line Inbound null-terminated string (excluding newline).
 * @param out_pkt Pointer to output CommandPacket structure.
 * @return true if command was successfully recognized and parsed.
 */
bool protocol_parse_command(const char* line, CommandPacket* out_pkt);

/**
 * @brief Parses an inbound line buffer into a StatusPacket.
 */
bool protocol_parse_status(const char* line, StatusPacket* out_status);

/**
 * @brief Parses an inbound line buffer into a ResponsePacket (ACK/NACK/PONG).
 */
bool protocol_parse_response(const char* line, ResponsePacket* out_resp);

/**
 * @brief Parses an inbound line buffer into a FaultPacket.
 */
bool protocol_parse_fault(const char* line, FaultPacket* out_fault);

#ifdef __cplusplus
}
#endif

#endif // TEJASVINI_SHARED_PROTOCOL_H_
