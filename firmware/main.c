#include "demuxer.h"

// ── main.c ────────────────────────────────────────────────────────────────────
// entry point for demuxer init firmware
// handles main loop, calibration flow, and finger-walk mode logic

// ── finger-walk config ────────────────────────────────────────────────────────
#define FINGERWALK_WALK_THRESH   1.5f   // taps/sec below this = walk
#define FINGERWALK_RUN_THRESH    3.5f   // taps/sec above this = run
#define FINGERWALK_TAP_FLEX      0.6f   // flex threshold to count as a tap
#define FINGERWALK_DEBOUNCE_MS   80     // min ms between tap events (prevent double-fire)

// ── global state ──────────────────────────────────────────────────────────────
static demuxer_state_t _state = {0};

// stub — replace with platform ms timer
static uint32_t _millis(void) { return 0; }

// ── finger-walk tick ──────────────────────────────────────────────────────────
// call every main loop iteration when finger-walk mode is active
// detects alternating index/middle taps and maps to movement speed
static void _fingerwalk_tick(demuxer_state_t *s) {
    fingerwalk_state_t *fw = &s->fingerwalk;
    if (!fw->active) return;

    flex_data_t *flex = &s->flex;
    uint32_t now = _millis();

    // check for new tap on index or middle (must alternate, must be debounced)
    bool index_tap  = flex->normalized[FINGER_INDEX]  >= FINGERWALK_TAP_FLEX;
    bool middle_tap = flex->normalized[FINGER_MIDDLE] >= FINGERWALK_TAP_FLEX;

    uint8_t tapping_finger = 0;
    if (index_tap  && fw->last_tap_finger != FINGER_INDEX)  tapping_finger = FINGER_INDEX;
    if (middle_tap && fw->last_tap_finger != FINGER_MIDDLE) tapping_finger = FINGER_MIDDLE;

    if (tapping_finger && (now - fw->last_tap_ms) >= FINGERWALK_DEBOUNCE_MS) {
        // valid alternating tap detected
        uint32_t interval_ms = now - fw->last_tap_ms;
        if (interval_ms > 0) {
            fw->tap_speed = 1000.0f / (float)interval_ms; // taps/sec
        }
        fw->last_tap_finger = tapping_finger;
        fw->last_tap_ms = now;
    }

    // decay tap speed if no recent taps (idle = stopped)
    if ((now - fw->last_tap_ms) > 500) {
        fw->tap_speed = 0.0f;
    }

    // movement output based on tap speed
    // (in real impl: send gamepad axis or key event here)
    if (fw->tap_speed <= 0.0f) {
        // stopped
    } else if (fw->tap_speed < fw->walk_threshold) {
        // walk
    } else {
        // run
    }

    // wrist twist → turning (handled by IMU gesture: GESTURE_TWIST_LEFT/RIGHT)
    // no special handling needed here — imu_detect_gesture covers it
}

// ── full calibration flow ─────────────────────────────────────────────────────
void demuxer_calibrate_full(demuxer_state_t *s) {
    // step 1: hand relaxed baseline
    flex_data_t baseline = {0};
    flex_calibrate(&baseline, NULL);

    // step 2: full curl max (user curls all fingers)
    // in real impl: wait for user prompt on touchscreen, then sample
    flex_data_t max_curl = {0};
    flex_calibrate(NULL, &max_curl);

    // step 3: IMU baseline (wrist neutral, arm forward)
    // wrist motion test just samples some gestures — thresholds stay at defaults
    // user does forward/back jerk + twist for confirmation feedback

    // step 4: wheel mode calibration
    // user holds object level at center, then max left, max right
    s->wheel.center_x   = 0.0f;  // TODO: sample from IMU during cal
    s->wheel.lock_left  = -45.0f;
    s->wheel.lock_right =  45.0f;
    s->wheel.deadzone   = 0.05f;

    // step 5: finger-walk thresholds
    s->fingerwalk.walk_threshold = FINGERWALK_WALK_THRESH;
    s->fingerwalk.run_threshold  = FINGERWALK_RUN_THRESH;

    s->calibrated = true;
    touchscreen_render_status(s);
}

// ── init ──────────────────────────────────────────────────────────────────────
void demuxer_init(void) {
    flex_init();
    imu_init();
    foil_init();
    haptic_init();
    touchscreen_init();

    // load default profile
    _state.active_profile.mode = MODE_GAMEPAD;
    for (int i = 0; i < FINGER_COUNT; i++) {
        _state.active_profile.flex_sensitivity[i] = 1.0f;
    }
    _state.active_profile.imu_cooldown_ms = 400.0f;
    _state.fingerwalk.walk_threshold = FINGERWALK_WALK_THRESH;
    _state.fingerwalk.run_threshold  = FINGERWALK_RUN_THRESH;

    _state.calibrated = false;
    _state.battery_pct = 100; // TODO: read from battery gauge IC
}

// ── main loop ─────────────────────────────────────────────────────────────────
void demuxer_loop(void) {
    // read all sensors
    flex_read(&_state.flex);
    imu_read(&_state.imu);
    foil_read(&_state.foil);

    // check recal shortcut (hold FOIL_RECAL 3s)
    if (foil_recal_triggered(&_state.foil)) {
        demuxer_calibrate_full(&_state);
        return;
    }

    // wheel mode update
    if (_state.wheel.active) {
        imu_update_wheel(&_state.wheel, &_state.imu);
        // TODO: map wheel->current_angle to gamepad axis output
        return; // wheel mode is exclusive — don't process other gestures
    }

    // finger-walk mode update
    if (_state.fingerwalk.active) {
        _fingerwalk_tick(&_state);
        return; // standalone mode — no other gesture processing
    }

    // standard gesture detection
    gesture_t g = imu_detect_gesture(&_state.imu, &_state.active_profile);
    if (g != GESTURE_NONE) {
        _state.last_gesture = g;
        // TODO: map gesture to gamepad/keyboard output based on active profile + modifier
    }

    // grip / pinch detection from flex
    bool modifier = foil_modifier_active(&_state.foil);
    (void)modifier; // used to remap outputs when held

    // haptic tick (autoclick safety timer)
    haptic_tick();

    // touchscreen update (not every frame — throttled inside if needed)
    touchscreen_render_status(&_state);
}

// ── platform entry point ──────────────────────────────────────────────────────
// call these from your MCU's main() / Arduino setup()+loop() / RTOS task
// example (Arduino-style):
//
//   void setup() { demuxer_init(); }
//   void loop()  { demuxer_loop(); }
