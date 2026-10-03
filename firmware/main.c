#include "demuxer.h"

// ➖ main.c ➖

// include the wake_alarm header
#include "wake_alarm.c" 

// entry point for demuxer init firmware
// handles main loop, calibration flow, and finger-walk mode logic

// ➖ finger-walk config ➖
#define FINGERWALK_WALK_THRESH 1.5f // taps/sec below this = walk
#define FINGERWALK_RUN_THRESH 3.5f // taps/sec above this = run
#define FINGERWALK_TAP_FLEX 0.6f // flex threshold to count as a tap
#define FINGERWALK_DEBOUNCE_MS 80 // min ms between tap events (prevent double-fire)

#define BATTERY_DEFAULT_PCT 100 // default charge on bootup

// ➖ global state ➖
static demuxer_state_t _state = {0};

// stub ■ replace with platform ms timer
static uint32_t _millis(void) { return 0; }

// ➖ finger-walk tick ➖
static void _fingerwalk_tick(demuxer_state_t *s) {
    // existing finger-walk logic...
}

// ➖ full calibration flow ➖
void demuxer_calibrate_full(demuxer_state_t *s) {
    // existing calibration logic...
}

// ➖ init ➖
void demuxer_init(void) {
    flex_init();
    imu_init();
    foil_init();
    haptic_init();
    touchscreen_init();
    wake_alarm_init(); // Initialize wake and alarm system

    // load default profile
    _state.active_profile.mode = MODE_GAMEPAD;
    for (int i = 0; i < FINGER_COUNT; i++) {
        _state.active_profile.flex_sensitivity[i] = 1.0f;
    }
    _state.active_profile.imu_cooldown_ms = 400.0f;
    _state.fingerwalk.walk_threshold = FINGERWALK_WALK_THRESH;
    _state.fingerwalk.run_threshold  = FINGERWALK_RUN_THRESH;

    _state.calibrated = false;
    _state.battery_pct = BATTERY_DEFAULT_PCT; // Default battery level before actual read
}

// ➖ main loop ➖
void demuxer_loop(void) {
    wake_alarm_check_conditions(); // Check for wake/alarm conditions

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
        return; // wheel mode is exclusive, don't process other gestures
    }

    // finger-walk mode update
    if (_state.fingerwalk.active) {
        _fingerwalk_tick(&_state);
        return; // standalone mode, no other gesture processing
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

    // touchscreen update (not every frame, throttled inside if needed)
    touchscreen_render_status(&_state);
}