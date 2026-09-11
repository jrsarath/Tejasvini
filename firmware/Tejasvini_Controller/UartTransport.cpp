#include "UartTransport.h"
#include <Arduino.h>
#include <string.h>

UartTransport::UartTransport(uint8_t pin_tx, uint8_t pin_rx, uint32_t baud_rate)
    : pin_tx_(pin_tx), pin_rx_(pin_rx), baud_rate_(baud_rate),
      rx_idx_(0), last_rx_time_ms_(0) {
    memset(rx_buffer_, 0, sizeof(rx_buffer_));
}

void UartTransport::init() {
    Serial.begin(baud_rate_);
    Serial1.setTX(pin_tx_);
    Serial1.setRX(pin_rx_);
    Serial1.begin(baud_rate_);
    last_rx_time_ms_ = millis();
}

bool UartTransport::readLine(char* out_line, size_t max_len) {
    if (!out_line || max_len == 0) return false;

    // Check both hardware UART (Serial1) and USB Serial (for debugging)
    Stream* stream = &Serial1;
    if (Serial.available()) {
        stream = &Serial;
    } else if (!Serial1.available()) {
        return false;
    }

    while (stream->available()) {
        int c = stream->read();
        if (c < 0) break;

        last_rx_time_ms_ = millis();

        if (c == '\r') {
            continue; // Ignore carriage return
        }

        if (c == '\n') {
            if (rx_idx_ > 0) {
                rx_buffer_[rx_idx_] = '\0';
                strncpy(out_line, rx_buffer_, max_len - 1);
                out_line[max_len - 1] = '\0';
                rx_idx_ = 0;
                return true;
            }
            continue;
        }

        if (rx_idx_ < sizeof(rx_buffer_) - 1) {
            rx_buffer_[rx_idx_++] = (char)c;
        } else {
            // Buffer overflow protection: reset buffer
            rx_idx_ = 0;
        }
    }
    return false;
}

void UartTransport::write(const char* str) {
    if (!str) return;
    Serial1.print(str);
    Serial.print(str);
}

void UartTransport::writeLine(const char* line) {
    if (!line) return;
    Serial1.println(line);
    Serial.println(line);
}
