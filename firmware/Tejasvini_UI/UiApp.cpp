#include "UiApp.h"
#include <Arduino.h>

UiApp* UiApp::s_instance = nullptr;

UiApp::UiApp()
    : model_(),
      client_(model_, PIN_UART_TX, PIN_UART_RX, UART_BAUD_RATE),
      touch_(),
      display_(touch_) {
    s_instance = this;
}

UiApp* UiApp::getInstance() {
    return s_instance;
}

void UiApp::setup() {
    // 1. Initialize touch and display
    touch_.init();
    display_.init();

    // 2. Initialize communication client
    client_.init();
}

void UiApp::loop() {
    uint32_t now = millis();

    // 1. Process inbound telemetry and outbound heartbeats
    client_.update(now);

    // 2. Update LVGL UI animations and rendering
    display_.update();

    yield();
}
