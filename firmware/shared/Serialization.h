#ifndef TEJASVINI_SHARED_SERIALIZATION_H_
#define TEJASVINI_SHARED_SERIALIZATION_H_

#include <stddef.h>
#include <stdbool.h>
#include "Protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Formats a CommandPacket into a wire string buffer.
 */
int serialize_command(const CommandPacket* pkt, char* buf, size_t buf_size);

/**
 * @brief Formats a ResponsePacket into a wire string buffer.
 */
int serialize_response(const ResponsePacket* pkt, char* buf, size_t buf_size);

/**
 * @brief Formats a StatusPacket into a wire string buffer.
 */
int serialize_status(const StatusPacket* pkt, char* buf, size_t buf_size);

/**
 * @brief Formats a FaultPacket into a wire string buffer.
 */
int serialize_fault(const FaultPacket* pkt, char* buf, size_t buf_size);

/**
 * @brief Formats an EventPacket into a wire string buffer.
 */
int serialize_event(const EventPacket* pkt, char* buf, size_t buf_size);

/**
 * @brief Safe tokenizer for whitespace-separated arguments.
 */
int tokenize_line(char* line, char* tokens[], int max_tokens);

/**
 * @brief Extracts a key-value parameter from a token string (e.g. "key=val").
 */
bool parse_key_value(const char* token, char* key, size_t key_len, char* val, size_t val_len);

#ifdef __cplusplus
}
#endif

#endif // TEJASVINI_SHARED_SERIALIZATION_H_
