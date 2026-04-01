#include "demuxer.h"

// ── foil.c ────────────────────────────────────────────────────────────────────
// reads conductive foil contact pads via GPIO
// each pad is wired directly to an MCU GPIO pin
// contact = circuit completed between fingertip pad and palm pad

// ── config ────────────────────────────────────────────────────────────────────
// TODO: replace with actual GPIO pin numbers for your MCU
static const uint8_t FOIL_GPIO_PINS[FOIL_COUNT] = { 5, 6, 7, 8, 9 };

#define FOIL_DEBOUNCE_MS    25      // ms to wait before confirming press
#define FOIL_SWEAT_THRESHOLD 50     // resistance threshold (platform specific)
                                    // above this = false trigger from sweat/graze

// ── internal state ────────────────────────────────────────────────────────────
static bool     _raw[FOIL_COUNT]      = {false};
static bool     _debounced[FOIL_COUNT] = {false};
static uint32_t _press_start[FOIL_COUNT] = {0};

// stub: returns true if GPIO pin is pulled LOW (contact made)
// replace with your platform's GPIO read — foil pads typically pull to GND on contact
static bool _gpio_read(uint8_t pin) {
    (void)pin;
    return false; // stub
}

// stub: platform ms timer
static uint32_t _millis(void) {
    return 0;
}

// ── init ──────────────────────────────────────────────────────────────────────
void foil_init(void) {
    for (int i = 0; i < FOIL_COUNT; i++) {
        // set GPIO as input with pull-up
        // gpio_set_input_pullup(FOIL_GPIO_PINS[i]);
        (void)FOIL_GPIO_PINS[i];
        _raw[i]         = false;
        _debounced[i]   = false;
        _press_start[i] = 0;
    }
}

// ── read ──────────────────────────────────────────────────────────────────────
// debounces each contact and outputs stable pressed state + hold duration
void foil_read(foil_state_t *out) {
    uint32_t now = _millis();

    for (int i = 0; i < FOIL_COUNT; i++) {
        bool reading = _gpio_read(FOIL_GPIO_PINS[i]);

        if (reading != _raw[i]) {
            // state changed — restart debounce timer
            _raw[i] = reading;
            _press_start[i] = now;
        }

        // only accept state after debounce window
        if ((now - _press_start[i]) >= FOIL_DEBOUNCE_MS) {
            _debounced[i] = _raw[i];
        }

        out->pressed[i] = _debounced[i];

        // track hold duration
        if (out->pressed[i]) {
            out->hold_ms[i] = now - _press_start[i];
        } else {
            out->hold_ms[i] = 0;
        }
    }
}

// ── helpers ───────────────────────────────────────────────────────────────────

// returns true if the contact has been held for at least min_ms
bool foil_held(const foil_state_t *state, uint8_t idx, uint32_t min_ms) {
    if (idx >= FOIL_COUNT) return false;
    return state->pressed[idx] && (state->hold_ms[idx] >= min_ms);
}

// check if recal contact is held long enough to trigger recalibration (3s)
bool foil_recal_triggered(const foil_state_t *state) {
    return foil_held(state, FOIL_RECAL, 3000);
}

// check if modifier is being held (changes what gestures do)
bool foil_modifier_active(const foil_state_t *state) {
    return state->pressed[FOIL_D];
}
