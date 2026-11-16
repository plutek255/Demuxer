#ifndef USB_HANDLER_H
#define USB_HANDLER_H

#include <stdint.h>

/* usb_handler.h
 * Header file for USB communication functions in Demuxer.
 */

/**
 * Initializes the USB subsystem for communication.
 */
void usb_init(void);

/**
 * Sends data over USB.
 * 
 * @param data Pointer to the data buffer to send.
 * @param length Length of the data in bytes.
 * @return Status (0 for success, non-zero for errors).
 */
int usb_send(const uint8_t *data, uint32_t length);

/**
 * Receives data from USB.
 * 
 * @param buffer Buffer to store received data.
 * @param length Maximum number of bytes to receive.
 * @return Number of bytes received, or error code.
 */
int usb_receive(uint8_t *buffer, uint32_t length);

#endif // USB_HANDLER_H
