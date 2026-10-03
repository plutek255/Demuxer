#include "demuxer.h"

// ➖ foil.c ➖

// reads conductive foil contact pads via GPIO
// each pad is wired directly to an MCU GPIO pin
// contact = circuit completed between fingertip pad and palm pad

// ➖ config ➖
static const uint8_t FOIL_GPIO_PINS[FOIL_COUNT] = { 5, 6, 7, 8, 9 };

#define FOIL_DEBOUNCE_MS       25    // ms to wait before confirming press
#define FOIL_SWEAT_THRESHOLD   50    // Resistance threshold (platform specific)
                                    // above this = false trigger from sweat/graze

// ➖ internal state ➖
static bool    _raw[FOIL_COUNT]   = {false};
static bool    _debounced[FOIL_COUNT] = {false};
static uint32_t _press_start[FOIL_COUNT] = {0};

static bool _gpio_read(uint8_t pin) {
    // Logic to read GPIO pin state
    (void)pin;
    return false; // stub
}

static uint32_t _millis(void) {
    return 0;
}

// ➖ init ➖
void foil_init(void) {
    for (int i = 0; i < FOIL_COUNT; i++) {
        (void)FOIL_GPIO_PINS[i];
        _raw[i]        = false;
        _debounced[i]  = false;
        _press_start[i] = 0;
    }
}

// ➖ read ➖
// debounces each contact and outputs stable pressed state + hold duration
void foil_read(foil_state_t *out) {
    uint32_t now = _millis();

    for (int i = 0; i < FOIL_COUNT; i++) {
        bool reading = _gpio_read(FOIL_GPIO_PINS[i]);

        if (reading != _raw[i]) {
            _raw[i] = reading;
            _press_start[i] = now;
        }

        if ((now - _press_start[i]) >= FOIL_DEBOUNCE_MS) {
            _debounced[i] = _raw[i];
        }

        out->pressed[i] = _debounced[i];
        if (out->pressed[i]) {
            out->hold_ms[i] = now - _press_start[i];
        } else {
            out->hold_ms[i] = 0;
        }
    }
}

bool foil_held(const foil_state_t *state, uint8_t idx, uint32_t min_ms) {
    if (idx >= FOIL_COUNT) return false;
    return state->pressed[idx] && (state->hold_ms[idx] >= min_ms);
}

bool foil_recal_triggered(const foil_state_t *state) {
    return foil_held(state, FOIL_RECAL, 3000);
}

bool foil_modifier_active(const foil_state_t *state) {
    return state->pressed[FOIL_D];
}

// ➖ new function ➖
// Implements registration of touch-complete inputs without mechanical buttons
bool register_touch_complete(const foil_state_t *state) {
    /*
     * Registers touch complete for foil contact pads
     * without requiring mechanical button input
     * Returns true if the sequence of specific foil inputs
     * indicate a completed registration pattern.
     */
    return state->pressed[FOIL_A] && state->pressed[FOIL_B]; // Example logic
}