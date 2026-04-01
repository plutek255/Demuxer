#include "demuxer.h"
#include <math.h>

// ── imu.c ─────────────────────────────────────────────────────────────────────
// handles 6DOF IMU reads (accel + gyro) and gesture detection
// also handles wheel mode angle calculation

// ── config defaults ───────────────────────────────────────────────────────────
#define JERK_THRESHOLD_DEFAULT      2.5f    // m/s^2 delta to trigger jerk gesture
#define TWIST_THRESHOLD_DEFAULT     80.0f   // deg/s to trigger twist
#define TILT_THRESHOLD_DEFAULT      25.0f   // degrees tilt to trigger
#define GESTURE_COOLDOWN_DEFAULT_MS 400     // ms before same gesture can retrigger
#define SMOOTHING_ALPHA             0.2f    // low-pass filter alpha (0=no update, 1=no filter)

// ── internal state ────────────────────────────────────────────────────────────
static imu_raw_t _smoothed = {0};
static imu_raw_t _prev = {0};
static uint32_t  _last_gesture_ms = 0;
static gesture_t _last_gesture = GESTURE_NONE;

// stub: replace with actual IMU read (I2C/SPI to your 6DOF chip)
static void _imu_hw_read(imu_raw_t *out) {
    // e.g. MPU6050, ICM-42688, LSM6DS3 etc
    out->ax = 0.0f; out->ay = 0.0f; out->az = 9.81f;
    out->gx = 0.0f; out->gy = 0.0f; out->gz = 0.0f;
}

// stub: returns ms since boot — replace with platform timer
static uint32_t _millis(void) {
    return 0;
}

// ── low-pass filter ───────────────────────────────────────────────────────────
static void _smooth(imu_raw_t *s, const imu_raw_t *new_val) {
    #define LPF(a, b) ((SMOOTHING_ALPHA * (b)) + ((1.0f - SMOOTHING_ALPHA) * (a)))
    s->ax = LPF(s->ax, new_val->ax);
    s->ay = LPF(s->ay, new_val->ay);
    s->az = LPF(s->az, new_val->az);
    s->gx = LPF(s->gx, new_val->gx);
    s->gy = LPF(s->gy, new_val->gy);
    s->gz = LPF(s->gz, new_val->gz);
    #undef LPF
}

// ── init ──────────────────────────────────────────────────────────────────────
void imu_init(void) {
    // platform-specific IMU chip init (I2C/SPI setup, register config)
    // e.g. set sample rate, enable accel+gyro, configure DLPF
    imu_raw_t initial = {0};
    initial.az = 9.81f; // gravity baseline
    _smoothed = initial;
    _prev = initial;
}

// ── read ──────────────────────────────────────────────────────────────────────
void imu_read(imu_raw_t *out) {
    imu_raw_t raw;
    _imu_hw_read(&raw);
    _smooth(&_smoothed, &raw);
    *out = _smoothed;
}

// ── gesture detection ─────────────────────────────────────────────────────────
gesture_t imu_detect_gesture(const imu_raw_t *data, const profile_t *profile) {
    uint32_t now = _millis();

    // cooldown check — prevent rapid-fire gesture triggers
    float cooldown = profile ? profile->imu_cooldown_ms : GESTURE_COOLDOWN_DEFAULT_MS;
    if ((now - _last_gesture_ms) < (uint32_t)cooldown) {
        return GESTURE_NONE;
    }

    float jerk_thresh  = profile ? profile->imu_thresholds[GESTURE_JERK_FORWARD]  : JERK_THRESHOLD_DEFAULT;
    float twist_thresh = profile ? profile->imu_thresholds[GESTURE_TWIST_LEFT]     : TWIST_THRESHOLD_DEFAULT;
    float tilt_thresh  = profile ? profile->imu_thresholds[GESTURE_TILT_LEFT]      : TILT_THRESHOLD_DEFAULT;

    // ── jerk detection (delta accel from prev frame) ──────────────────────────
    float dax = data->ax - _prev.ax;
    float day = data->ay - _prev.ay;

    if (dax > jerk_thresh) {
        _last_gesture = GESTURE_JERK_FORWARD;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_JERK_FORWARD;
    }
    if (dax < -jerk_thresh) {
        _last_gesture = GESTURE_JERK_BACKWARD;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_JERK_BACKWARD;
    }

    // ── twist detection (gyro Z axis) ─────────────────────────────────────────
    if (data->gz > twist_thresh) {
        _last_gesture = GESTURE_TWIST_RIGHT;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_TWIST_RIGHT;
    }
    if (data->gz < -twist_thresh) {
        _last_gesture = GESTURE_TWIST_LEFT;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_TWIST_LEFT;
    }

    // ── tilt detection (accel Y axis) ─────────────────────────────────────────
    // rough tilt angle estimate from accel
    float tilt_angle = atan2f(data->ay, data->az) * (180.0f / 3.14159265f);

    if (tilt_angle > tilt_thresh) {
        _last_gesture = GESTURE_TILT_LEFT;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_TILT_LEFT;
    }
    if (tilt_angle < -tilt_thresh) {
        _last_gesture = GESTURE_TILT_RIGHT;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_TILT_RIGHT;
    }

    // ── up/down tilt (accel X axis) ───────────────────────────────────────────
    float pitch = atan2f(data->ax, data->az) * (180.0f / 3.14159265f);
    if (pitch > tilt_thresh) {
        _last_gesture = GESTURE_TILT_UP;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_TILT_UP;
    }
    if (pitch < -tilt_thresh) {
        _last_gesture = GESTURE_TILT_DOWN;
        _last_gesture_ms = now;
        _prev = *data;
        return GESTURE_TILT_DOWN;
    }

    _prev = *data;
    return GESTURE_NONE;
}

// ── wheel mode ────────────────────────────────────────────────────────────────
// combines X tilt + vertical movement to estimate full wheel rotation angle
// output: wheel->current_angle, range -1.0 (full left) to 1.0 (full right)
void imu_update_wheel(wheel_state_t *wheel, const imu_raw_t *data) {
    if (!wheel->active) return;

    // X tilt (left/right arm tilt) = primary steering axis
    float tilt_x = atan2f(data->ay, data->az) * (180.0f / 3.14159265f);
    float tilt_y = atan2f(data->ax, data->az) * (180.0f / 3.14159265f); // vertical movement

    // normalize tilt_x relative to calibrated center + locks
    float rel = tilt_x - wheel->center_x;
    float range = (rel >= 0.0f)
        ? (wheel->lock_right - wheel->center_x)
        : (wheel->center_x - wheel->lock_left);

    if (range <= 0.0f) range = 1.0f; // avoid div by zero

    float angle = rel / range; // -1.0 to 1.0

    // blend in vertical component to smooth out freeform object drift
    // when arm tilts left + hand moves down = wheel turned left
    // when arm tilts right + hand moves up = wheel turned right
    float vert_factor = (tilt_y / 90.0f) * 0.3f; // minor correction weight
    angle += vert_factor * (angle >= 0.0f ? 1.0f : -1.0f);

    // apply deadzone
    if (fabsf(angle) < wheel->deadzone) angle = 0.0f;

    // clamp
    if (angle > 1.0f)  angle = 1.0f;
    if (angle < -1.0f) angle = -1.0f;

    wheel->current_angle = angle;
}
