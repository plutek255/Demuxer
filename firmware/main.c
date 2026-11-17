#include "demuxer.h"
#include "wifi_brick.h"
#include "iot_brick.h"

// Initialization of IoT and WiFi modules
void demuxer_connectivity_init(void) {
    wifi_brick_init();
    iot_brick_init();
}

// Main loop modification to include IoT and WiFi task handling
void demuxer_loop(void) {
    // Existing code...

    // IoT and WiFi task handling
    wifi_brick_task();
    iot_brick_task();

    // Existing code...
}
