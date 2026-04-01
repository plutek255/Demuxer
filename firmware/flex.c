#include "demuxer.h"

// ── flex.c ────────────────────────────────────────────────────────────────────
// reads analog flex sensor values from ADC pins and normalizes them
// per-user calibration is stored in the active profile baseline/max values

// TODO: replace with actual ADC pin assignments for your MCU
static const uint8_t FLEX_ADC_PINS[FINGER_COUNT] = { 0, 1, 2, 3, 4 };

// calibration baselines (set during calibration flow)
static flex_data_t _baseline = {0};
static flex_data_t _max_curl = {0};
static bool _calibrated = false;

// ── adc read stub ─────────────────────────────────────────────────────────────
// replace this with your platform's actual ADC read call
static uint16_t adc_read(uint8_t pin) {
    // stub — returns 0
    (void)pin;
    return 0;
}

// ── init ──────────────────────────────────────────────────────────────────────
void flex_init(void) {
    // set ADC pins as inputs
    // platform-specific GPIO init goes here
    for (int i = 0; i < FINGER_COUNT; i++) {
        (void)FLEX_ADC_PINS[i];
        // gpio_set_input(FLEX_ADC_PINS[i]);
    }

    // default baseline (uncalibrated safe values)
    for (int i = 0; i < FINGER_COUNT; i++) {
        _baseline.raw[i] = 100;
        _max_curl.raw[i] = 900;
        _baseline.normalized[i] = 0.0f;
        _max_curl.normalized[i] = 1.0f;
    }
}

// ── read ──────────────────────────────────────────────────────────────────────
void flex_read(flex_data_t *out) {
    for (int i = 0; i < FINGER_COUNT; i++) {
        out->raw[i] = adc_read(FLEX_ADC_PINS[i]);

        // normalize between baseline and max curl
        uint16_t span = _max_curl.raw[i] - _baseline.raw[i];
        if (span == 0) {
            out->normalized[i] = 0.0f;
            continue;
        }

        float norm = (float)(out->raw[i] - _baseline.raw[i]) / (float)span;

        // clamp 0.0–1.0
        if (norm < 0.0f) norm = 0.0f;
        if (norm > 1.0f) norm = 1.0f;

        out->normalized[i] = norm;
    }
}

// ── calibrate ─────────────────────────────────────────────────────────────────
// call twice during calibration flow:
//   1) hand relaxed → pass as baseline
//   2) full curl → pass as max_curl
void flex_calibrate(flex_data_t *baseline, flex_data_t *max_curl) {
    // sample multiple reads and average for stability
    #define CAL_SAMPLES 16
    uint32_t sum[FINGER_COUNT] = {0};

    for (int s = 0; s < CAL_SAMPLES; s++) {
        for (int i = 0; i < FINGER_COUNT; i++) {
            sum[i] += adc_read(FLEX_ADC_PINS[i]);
        }
        // small delay between samples — platform specific
        // delay_ms(10);
    }

    flex_data_t *target = (baseline != NULL) ? baseline : max_curl;
    for (int i = 0; i < FINGER_COUNT; i++) {
        target->raw[i] = (uint16_t)(sum[i] / CAL_SAMPLES);
        target->normalized[i] = (baseline != NULL) ? 0.0f : 1.0f;
    }

    if (baseline != NULL) _baseline = *baseline;
    if (max_curl != NULL)  _max_curl = *max_curl;

    _calibrated = (baseline != NULL && max_curl != NULL);
}

// ── helpers ───────────────────────────────────────────────────────────────────

// returns true if finger bend exceeds threshold (0.0–1.0)
bool flex_finger_bent(const flex_data_t *data, uint8_t finger, float threshold) {
    if (finger >= FINGER_COUNT) return false;
    return data->normalized[finger] >= threshold;
}

// returns true if all fingers are gripping (all above threshold)
bool flex_is_grip(const flex_data_t *data, float threshold) {
    for (int i = FINGER_INDEX; i <= FINGER_PINKY; i++) {
        if (data->normalized[i] < threshold) return false;
    }
    return true;
}

// returns true if thumb + index are pinching
bool flex_is_pinch(const flex_data_t *data, float threshold) {
    return data->normalized[FINGER_THUMB] >= threshold &&
           data->normalized[FINGER_INDEX] >= threshold;
}
