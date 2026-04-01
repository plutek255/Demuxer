#include "demuxer.h"

// ── haptic.c ──────────────────────────────────────────────────────────────────
// drives per-finger haptic motors (LRA or ERM)
// also handles the experimental autoclick feature (rapid pulse = simulated rapid input)

// ── config ────────────────────────────────────────────────────────────────────
// TODO: replace with actual PWM pin assignments for your MCU
static const uint8_t HAPTIC_PWM_PINS[FINGER_COUNT] = { 10, 11, 12, 13, 14 };

// autoclick safety cap — firmware enforces this hard ceiling regardless of profile setting
#define AUTOCLICK_MAX_FREQ_HZ   20      // 20 clicks/sec max — stay within fair-use bounds
#define AUTOCLICK_WARNING_MS    30000   // warn after 30s of continuous autoclick use

// ── internal state ────────────────────────────────────────────────────────────
static bool     _autoclick_active[FINGER_COUNT]  = {false};
static uint16_t _autoclick_freq[FINGER_COUNT]    = {0};
static uint32_t _autoclick_start_ms[FINGER_COUNT] = {0};

// stubs — replace with platform PWM driver calls
static void _pwm_set(uint8_t pin, uint16_t freq_hz, uint8_t duty) {
    (void)pin; (void)freq_hz; (void)duty;
}

static void _pwm_stop(uint8_t pin) {
    (void)pin;
}

static uint32_t _millis(void) {
    return 0;
}

// ── init ──────────────────────────────────────────────────────────────────────
void haptic_init(void) {
    for (int i = 0; i < FINGER_COUNT; i++) {
        _pwm_stop(HAPTIC_PWM_PINS[i]);
        _autoclick_active[i] = false;
    }
}

// ── single pulse ──────────────────────────────────────────────────────────────
// fires a one-shot haptic pulse on a finger motor
// freq_hz = vibration frequency, duration_ms = how long
void haptic_pulse(uint8_t finger, uint16_t freq_hz, uint16_t duration_ms) {
    if (finger >= FINGER_COUNT) return;
    _pwm_set(HAPTIC_PWM_PINS[finger], freq_hz, 128); // 50% duty
    // TODO: use a non-blocking timer to stop after duration_ms
    // for now this is a placeholder — platform timer needed
    (void)duration_ms;
    // after duration: _pwm_stop(HAPTIC_PWM_PINS[finger]);
}

// ── autoclick ─────────────────────────────────────────────────────────────────
// starts continuous rapid vibration on a finger = simulates rapid button presses
// frequency is capped at AUTOCLICK_MAX_FREQ_HZ regardless of what's passed in
void haptic_autoclick_start(uint8_t finger, uint16_t freq_hz) {
    if (finger >= FINGER_COUNT) return;

    // hard cap — safety requirement, do not remove
    if (freq_hz > AUTOCLICK_MAX_FREQ_HZ) freq_hz = AUTOCLICK_MAX_FREQ_HZ;

    _autoclick_freq[finger]    = freq_hz;
    _autoclick_active[finger]  = true;
    _autoclick_start_ms[finger] = _millis();

    _pwm_set(HAPTIC_PWM_PINS[finger], freq_hz, 128);
}

void haptic_autoclick_stop(uint8_t finger) {
    if (finger >= FINGER_COUNT) return;
    _autoclick_active[finger] = false;
    _pwm_stop(HAPTIC_PWM_PINS[finger]);
}

// ── tick (call in main loop) ──────────────────────────────────────────────────
// checks autoclick usage timers and stops if over warning threshold
void haptic_tick(void) {
    uint32_t now = _millis();
    for (int i = 0; i < FINGER_COUNT; i++) {
        if (!_autoclick_active[i]) continue;
        uint32_t elapsed = now - _autoclick_start_ms[i];
        if (elapsed >= AUTOCLICK_WARNING_MS) {
            // force stop + flag warning — long continuous use may cause discomfort
            haptic_autoclick_stop(i);
            // TODO: trigger a touchscreen warning notification
        }
    }
}
