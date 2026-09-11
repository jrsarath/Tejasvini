#ifndef TEJASVINI_BUZZER_MANAGER_H_
#define TEJASVINI_BUZZER_MANAGER_H_

#include <stdint.h>

class BuzzerManager {
public:
    explicit BuzzerManager(uint8_t pin_buzzer);

    void init();
    void update(uint32_t now_ms);

    void playBeep(uint16_t freq_hz, uint16_t duration_ms);
    void playClick();
    void playStartTone();
    void playCompleteTune();
    void playAlarm(bool active);

private:
    uint8_t pin_buzzer_;
    bool is_playing_;
    uint32_t tone_end_time_ms_;
    bool alarm_active_;
    uint32_t alarm_toggle_time_ms_;
    bool alarm_state_;
};

#endif // TEJASVINI_BUZZER_MANAGER_H_
