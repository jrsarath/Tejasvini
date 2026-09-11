#ifndef TEJASVINI_UART_TRANSPORT_H_
#define TEJASVINI_UART_TRANSPORT_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#if __has_include("../shared/Config.h")
#include "../shared/Config.h"
#else
#include "shared/Config.h"
#endif

class UartTransport {
public:
    UartTransport(uint8_t pin_tx, uint8_t pin_rx, uint32_t baud_rate = UART_BAUD_RATE);

    void init();
    bool readLine(char* out_line, size_t max_len);
    void write(const char* str);
    void writeLine(const char* line);

    uint32_t getLastRxTimeMs() const { return last_rx_time_ms_; }

private:
    uint8_t pin_tx_;
    uint8_t pin_rx_;
    uint32_t baud_rate_;

    char rx_buffer_[PROTOCOL_MAX_FRAME_LEN];
    size_t rx_idx_;
    uint32_t last_rx_time_ms_;
};

#endif // TEJASVINI_UART_TRANSPORT_H_
