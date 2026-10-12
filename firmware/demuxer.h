/*
 * demuxer.h
 * Definitions and function declarations for the Demuxer hardware interface.
 */

/**
 * Initializes the Demuxer hardware.
 * Should be called at the start before other operations.
 * 
 * @return 0 on success, non-zero error code on failure.
 */
int init_demuxer();

/**
 * Reads data from the sensor.
 * This function interfaces the Demuxer to gather sensor readouts.
 *
 * @return Sensor data.
 */
int read_sensor_data();

/**
 * Shutdown operations for Demuxer.
 * Ensures safe power down and status saving.
 * 
 * @return 0 on success, non-zero error code on failure.
 */
int shutdown_demuxer();