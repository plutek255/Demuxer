#ifndef DEMUXER_H
#define DEMUXER_H

/* Header file including all necessary declarations needed
* throughout the Demuxer firmware. */

#define MODE_GAMEPAD 0
#define MODE_KEYBOARD 1
#define FINGER_COUNT 5

typedef struct {
    float flex_sensitivity[FINGER_COUNT];
    unsigned int imu_cooldown_ms;
} profile_t;

void flex_init(void);
void imu_init(void);
void foil_init(void);
void haptic_init(void);
void touchscreen_init(void);
void demuxer_init(void);
void expansion_handler(void);

#endif
