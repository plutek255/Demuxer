#include "demuxer.h"

// Implementation of touch controls for foil contact pads on palm and fingertips.
void foil_init(void) {
    for (int i = 0; i < FOIL_COUNT; i++) {
        // Set GPIO as input with pull-up resistance.
        // (void)FOIL_GPIO_PINS[i];
        _raw[i] = false;
        _debounced[i] = false;
        _press_start[i] = 0;
    }
}

// Reads the current state of foil inputs and updates the structure with their states.
void foil_read(foil_state_t *out) {
    uint32_t now = millis();

    for (int i = 0; i < FOIL_COUNT; i++) {
        bool reading = _gpi_read(FOIL_GPIO_PINS[i]);

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

// Returns true if a specific foil contact has been held for a minimum time.
bool foil_held(const foil_state_t *state, uint8_t idx, uint32_t min_ms) {
    if (idx >= FOIL_COUNT) return false;
    return state->pressed[idx] && (state->hold_ms[idx] >= min_ms);
}

// Custom logic to handle recalibration trigger from a foil contact.
bool foil_recal_triggered(const foil_state_t *state) {
    return foil_held(state, FOIL_RECAL, 3000);
}
