#include "demuxer.h"

// include the pointer device header
#include "pointer_device.c"

// ➖ main.c ➖
// includes and main function definitions

// ➖ global state ➖

// ➖ init ➖
void demuxer_init(void) {
    flex_init();
    imu_init();
    foil_init();
    haptic_init();
    touchscreen_init();
    wake_alarm_init();
    _pointer_device_init(); // Initialize pointer device

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
    wake_alarm_check_conditions();
    pointer_device_process(); // Process pointer updates

    // Existing loop logic
}