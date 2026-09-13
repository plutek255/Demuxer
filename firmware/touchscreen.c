#include "demuxer.h"

#include <stddef.h>

// 📡 touchscreen.c 📡

#define POINTER_EMITTER_COUNT 2U

static pointer_platform_ops_t _platform = {0};
static pointer_init_status_t _status = POINTER_INIT_NOT_CONFIGURED;

void pointer_device_set_platform(const pointer_platform_ops_t *ops) {
    if (ops == NULL) {
        _platform = (pointer_platform_ops_t){0};
        _status = POINTER_INIT_NOT_CONFIGURED;
        return;
    }

    _platform = *ops;
    _status = POINTER_INIT_NOT_CONFIGURED;
}

/**
 * @brief Initializes the dual-light pointer hardware and receiver path.
 * 
 * Sets up the necessary configurations for supporting cursor operations
 * with the dual-light pointer device.
 */
pointer_init_status_t initPointerDevice(void) {
    if (_platform.configure_emitters == NULL ||
        _platform.connect_receiver == NULL ||
        _platform.disable_emitters == NULL) {
        _status = POINTER_INIT_NOT_CONFIGURED;
        return _status;
    }

    if (!_platform.configure_emitters(POINTER_EMITTER_COUNT)) {
        _platform.disable_emitters();
        _status = POINTER_INIT_HARDWARE_FAILED;
        return _status;
    }

    if (!_platform.connect_receiver(POINTER_EMITTER_COUNT)) {
        _platform.disable_emitters();
        _status = POINTER_INIT_RECEIVER_FAILED;
        return _status;
    }

    _status = POINTER_INIT_READY;
    return _status;
}

pointer_init_status_t pointer_device_status(void) {
    return _status;
}
