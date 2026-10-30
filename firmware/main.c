#include "demuxer.h"
#include "imu.h"

// 🟠 main.c 🟠

// entry point for demuxer init firmware
// handles main loop, calibration flow, and finger-walk mode logic

// 🟠 finger-walk config 🟠
#define FINGERWALK_WALK_THRESHOLD 1.5f // taps/sec below this = walk

/*
 * Main function that initializes the hardware and enters the main loop.
 * Includes setup for haptic feedback and periodic handling.
 */
int main() {
     // Initialize necessary components
     init_imu();
     init_haptic_system();

     while (1) {
         // Handle main loop
         update_imu_data();
         int gesture = detect_imu_gestures();

         // Respond to detected gestures
         switch (gesture) {
             case 1:
                 // Handle push gesture
                 activate_haptic_feedback(1);
                 break;
             case 2:
                 // Handle pull gesture
                 activate_haptic_feedback(2);
                 break;
             case 3:
                 // Handle twist gesture
                 break;
             case 4:
                 // Handle tilt gesture
                 break;
             default:
                 // No gesture detected
                 break;
         }
     }
     return 0;
}