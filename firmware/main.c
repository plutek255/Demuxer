#include "demuxer.h"
#include "usb_handler.h"

// 🌈 main.c 🌈

// entry point for demuxer init firmware
// handles main loop, calibration flow, and finger-walk mode logic

// ... existing code ...

// 🌈 init 🌈
void demuxer_init(void) {
    flex_init();
    imu_init();
    foil_init();
    haptic_init();
    touchscreen_init();
    usb_init(); // Initialize USB communication

    // load default profile
    _state.active_profile.mode = MODE_GAMEPAD;
    for (int i = 0; i < FINGER_COUNT; i++) {
        _state.active_profile.flex_sensitivity[i] = 1.0f;
    }
    _state.active_profile.imu_cooldown_ms = 400.0f;
    _state.fingerwalk.walk_threshold = FINGERWALK_WALK_THRESHOLD;
    _state.fingerwalk.run_threshold  = FINGERWALK_RUN_THRESHOLD;

    _state.calibrated = false;
    _state.battery_pct = BATTERY_DEFAULT_PCT; // Default battery level before actual read
}

// 🌈 main loop 🌈
void demuxer_loop(void) {
    // read all sensors
    flex_read(&_state.flex);
    imu_read(&_state.imu);
    foil_read(&_state.foil);
    
    // Handle USB data exchange
    uint8_t usb_buffer[64]; // example buffer size
    int received_length = usb_receive(usb_buffer, sizeof(usb_buffer));
    if (received_length > 0) {
        // Process received data
    }

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

    // grip/pinch detection from flex
    bool modifier = foil_modifier_active(&_state.foil);
    (void)modifier; // used to remap outputs when held

    // haptic tick (autoclick safety timer)
    haptic_tick();

    // touchscreen update (not every frame 🌟 throttled inside if needed)
    touchscreen_render_status(&_state);
}

// 🌈 platform entry point 🌈
// call these from your MCU's main() / Arduino setup()+loop() / RTOS task
// example (Arduino-style);

//   void setup() { demuxer_init(); }
//   void loop()  { demuxer_loop(); }

