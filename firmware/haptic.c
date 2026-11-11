#include "demuxer.h"

// Implementation for haptic feedback on individual fingers for interaction feedback.

// Initializer to setup haptic motor control and feedback patterns as per configuration.
void haptic_init(void) {
    for (int i = 0; i < FINGER_COUNT; i++) {
        _pwm_stop(HAPTIC_PWM_PINS[i]);
        _autoclick_active[i] = false;
    }
}

// Implementation for executing a pulse on the given finger's haptic motor.
void haptic_pulse(uint8_t finger, uint16_t freq_hz, uint16_t duration_ms) {
    if (finger >= FINGER_COUNT) return;
    _pwm_set(HAPTIC_PWM_PINS[finger], freq_hz, 128);
    // duration control logic pending platform
    // (void)duration_ms;
}

// Routines for autoclick start and stop operations, ensuring frequency limits and sustained operation warnings.
void haptic_autoclick_start(uint8_t finger, uint16_t freq_hz) {
    if (finger >= FINGER_COUNT) return;
    if (freq_hz > AUTOCLICK_MAX_FREQ_HZ) freq_hz = AUTOCLICK_MAX_FREQ_HZ;
    _autoclick_freq[finger] = freq_hz;
    _autoclick_active[finger] = true;
    _autoclick_start_ms[finger] = _millis();
    _pwm_set(HAPTIC_PWM_PINS[finger], freq_hz, 128);
}

void haptic_autoclick_stop(uint8_t finger) {
    if (finger >= FINGER_COUNT) return;
    _autoclick_active[finger] = false;
    _pwm_stop(HAPTIC_PWM_PINS[finger]);
}

void haptic_tick(void) {
    uint32_t now = _millis();
    for (int i = 0; i < FINGER_COUNT; i++) {
        if (!_autoclick_active[i]) continue;
        uint32_t elapsed = now - _autoclick_start_ms[i];
        if (elapsed >= AUTOCLICK_WARNING_MS) {
            haptic_autoclick_stop(i);
            // Optional: trigger touchscreen warning
        }
    }
}
