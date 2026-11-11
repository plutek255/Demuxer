#include "demuxer.h"
#include <math.h>

/* IMU Control for Gesture Recognition
 * Integrates a 6DOF IMU for detecting wrist gestures and facilitating input path usage.
 */

void imu_init(void) {
    imu_raw_t initial = {0};
    initial.az = 9.81f; // gravity baseline
    _smoothed = initial;
    _prev = initial;
}

void imu_read(imu_raw_t *out) {
    imu_raw_t raw;
    _imu_hw_read(&raw);
    _smooth(&_smoothed, &raw);
    *out = _smoothed;
}

gesture_t imu_detect_gesture(const imu_raw_t *data, const profile_t *profile) {
    uint32_t now = _millis();
    float cooldown = profile ? profile->imu_cooldown_ms : GESTURE_COOLDOWN_DEFAULT_MS;
    if ((now - _last_gesture_ms) < (uint32_t)cooldown) {
        return GESTURE_NONE;
    }

    float jerk_thresh   = profile ? profile->imu_thresholds[GESTURE_JERK_FORWARD] : JERK_THRESHOLD_DEFAULT;
    float twist_thresh  = profile ? profile->imu_thresholds[GESTURE_TWIST_LEFT]  : TWIST_THRESHOLD_DEFAULT;
    float tilt_thresh   = profile ? profile->imu_thresholds[GESTURE_TILT_LEFT]   : TILT_THRESHOLD_DEFAULT;

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
