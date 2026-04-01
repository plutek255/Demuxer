#ifndef DEMUXER_H
#define DEMUXER_H

#include <stdint.h>
#include <stdbool.h>

// ── version ───────────────────────────────────────────────────────────────────
#define DEMUXER_MAKE        "Demuxer"
#define DEMUXER_MODEL       "Init"
#define DEMUXER_FW_VERSION  "0.1.0-beta"

// ── finger indices ────────────────────────────────────────────────────────────
#define FINGER_THUMB    0
#define FINGER_INDEX    1
#define FINGER_MIDDLE   2
#define FINGER_RING     3
#define FINGER_PINKY    4
#define FINGER_COUNT    5

// ── foil contact indices ──────────────────────────────────────────────────────
// exact layout TBD — placeholder count
#define FOIL_A          0   // confirm / menu
#define FOIL_B          1   // back
#define FOIL_C          2   // push-to-talk
#define FOIL_D          3   // modifier
#define FOIL_RECAL      4   // recalibrate (hold 3s)
#define FOIL_COUNT      5

// ── input modes ───────────────────────────────────────────────────────────────
typedef enum {
    MODE_GAMEPAD = 0,
    MODE_KEYBOARD_MOUSE,
    MODE_CUSTOM
} input_mode_t;

// ── flex sensor data ──────────────────────────────────────────────────────────
typedef struct {
    uint16_t raw[FINGER_COUNT];     // raw ADC values
    float    normalized[FINGER_COUNT]; // 0.0 (straight) to 1.0 (full curl)
} flex_data_t;

// ── IMU data ──────────────────────────────────────────────────────────────────
typedef struct {
    float ax, ay, az;   // accelerometer (m/s^2)
    float gx, gy, gz;   // gyroscope (deg/s)
} imu_raw_t;

// ── gesture types ─────────────────────────────────────────────────────────────
typedef enum {
    GESTURE_NONE = 0,
    GESTURE_JERK_FORWARD,
    GESTURE_JERK_BACKWARD,
    GESTURE_TWIST_LEFT,
    GESTURE_TWIST_RIGHT,
    GESTURE_TILT_LEFT,
    GESTURE_TILT_RIGHT,
    GESTURE_TILT_UP,
    GESTURE_TILT_DOWN,
    GESTURE_GRIP,
    GESTURE_PINCH,
    GESTURE_COUNT
} gesture_t;

// ── foil contact state ────────────────────────────────────────────────────────
typedef struct {
    bool     pressed[FOIL_COUNT];
    uint32_t hold_ms[FOIL_COUNT];  // how long each has been held
} foil_state_t;

// ── wheel mode state ──────────────────────────────────────────────────────────
typedef struct {
    bool  active;
    float center_x;     // calibrated center X tilt
    float center_y;     // calibrated center Y
    float lock_left;    // max left angle
    float lock_right;   // max right angle
    float deadzone;     // 0.0–1.0
    float current_angle; // -1.0 (full left) to 1.0 (full right)
} wheel_state_t;

// ── finger-walk mode state ────────────────────────────────────────────────────
typedef struct {
    bool     active;
    uint8_t  last_tap_finger; // FINGER_INDEX or FINGER_MIDDLE
    uint32_t last_tap_ms;
    float    tap_speed;       // taps/sec
    float    walk_threshold;  // taps/sec below = walk
    float    run_threshold;   // taps/sec above = run
} fingerwalk_state_t;

// ── profile ───────────────────────────────────────────────────────────────────
typedef struct {
    char         name[32];
    input_mode_t mode;
    float        flex_sensitivity[FINGER_COUNT]; // per-finger multiplier
    float        imu_thresholds[GESTURE_COUNT];  // gesture trigger thresholds
    float        imu_cooldown_ms;                // ms between gesture triggers
    uint8_t      foil_mappings[FOIL_COUNT];      // maps foil → key/button id
    wheel_state_t wheel;
    fingerwalk_state_t fingerwalk;
} profile_t;

// ── global device state ───────────────────────────────────────────────────────
typedef struct {
    flex_data_t       flex;
    imu_raw_t         imu;
    foil_state_t      foil;
    gesture_t         last_gesture;
    wheel_state_t     wheel;
    fingerwalk_state_t fingerwalk;
    profile_t         active_profile;
    bool              calibrated;
    uint8_t           battery_pct;  // 0–100
    bool              touchscreen_locked;
} demuxer_state_t;

// ── function prototypes ───────────────────────────────────────────────────────

// flex.c
void flex_init(void);
void flex_read(flex_data_t *out);
void flex_calibrate(flex_data_t *baseline, flex_data_t *max_curl);

// imu.c
void imu_init(void);
void imu_read(imu_raw_t *out);
gesture_t imu_detect_gesture(const imu_raw_t *data, const profile_t *profile);
void imu_update_wheel(wheel_state_t *wheel, const imu_raw_t *data);

// foil.c
void foil_init(void);
void foil_read(foil_state_t *out);
bool foil_held(const foil_state_t *state, uint8_t idx, uint32_t min_ms);

// haptic.c
void haptic_init(void);
void haptic_pulse(uint8_t finger, uint16_t freq_hz, uint16_t duration_ms);
void haptic_autoclick_start(uint8_t finger, uint16_t freq_hz);
void haptic_autoclick_stop(uint8_t finger);

// touchscreen.c
void touchscreen_init(void);
void touchscreen_set_lock(bool locked);
void touchscreen_render_status(const demuxer_state_t *state);

// main.c
void demuxer_init(void);
void demuxer_loop(void);
void demuxer_calibrate_full(demuxer_state_t *state);

#endif // DEMUXER_H
