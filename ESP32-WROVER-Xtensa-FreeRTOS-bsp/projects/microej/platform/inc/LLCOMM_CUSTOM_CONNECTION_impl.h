/* 
 * Icetea
 * 
 * Copyright 2013-2014 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms. 
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/* 
 * Implementation header file. Shall only be included by client implementation C files.
 */
#include <LLCOMM_CUSTOM_CONNECTION.h>
#include <stdint.h>
#include <intern/LLCOMM_CUSTOM_CONNECTION_impl.h>
#ifndef LLCOMM_CUSTOM_CONNECTION_IMPL
#warning "Implementation tag has not been defined. Default implementation tag is LLCOMM_CUSTOM_CONNECTION_IMPL"
#endif
// --------------------------------------------------------------------------------
// -                      Functions that must be implemented                      -
// --------------------------------------------------------------------------------

/**
 * Initialize the comm driver and hardware to support this stream.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_initialize(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable RX hardware and start accepting data.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_enableRX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable RX hardware and stop accepting data.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_disableRX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when new data is received.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_enableRXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when new data is received.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_disableRXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable TX hardware and start sending data.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_enableTX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable TX hardware and stop sending data.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_disableTX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 * 
 * When the interrupt occurs the interrupt handler should call transmitBufferReady().
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_enableTXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_disableTXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Configure a serial stream. A stream can be re-configured after it is closed.
 * @param baudrate a positive integer
 * @param bitsperchar {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_5} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_6} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_7} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_8} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_9}
 * @param stopbits {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_1} | {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_2} | {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_1_5}
 * @param parity {@link LLCOMM_CUSTOM_CONNECTION_PARITY_NO} | {@link LLCOMM_CUSTOM_CONNECTION_PARITY_EVEN} | {@link LLCOMM_CUSTOM_CONNECTION_PARITY_ODD}
 * @return {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} if initialization is successful, or negative error code ({@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_CANNOT_OPEN})
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_configureDevice(LLCOMM_CUSTOM_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity);

/**
 * Get the name of this stream, if it can be open using a name.
 * @return a null terminated string, or <code>NULL</code> if this stream can only be open using a platform id
 */
void* LLCOMM_CUSTOM_CONNECTION_IMPL_getName(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the platform id of this stream, if it can be open using a platform id.
 * @return a positive integer, or {@link LLCOMM_CUSTOM_CONNECTION_NO_PLATFORM_ID} if this stream can only be open using a name
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_getPlatformId(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * This function is called after the connection has been removed using {@link LLCOMM_removeConnection}, 
 * when it is no more handled by the application. If the connection was open when it was removed, this function will be called only after the connection 
 * will be closed by the application
 */
void LLCOMM_CUSTOM_CONNECTION_IMPL_released(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the number of properties defined to identify this stream.
 * @return a positive integer (0 if this stream does not define any properties)
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_getNbProperties(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the value of the given property.
 * @param propertyName the property name (null terminated string)
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property
 */
void* LLCOMM_CUSTOM_CONNECTION_IMPL_getProperty(LLCOMM_CUSTOM_CONNECTION* env, void* propertyName);

/**
 * Get the property name from property id.
 * @param propertyId an id between <code>0</code> and <code>{@link LLCOMM_CUSTOM_CONNECTION_getNbProperties}-1</code>.
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property with the given id
 */
void* LLCOMM_CUSTOM_CONNECTION_IMPL_getPropertyName(LLCOMM_CUSTOM_CONNECTION* env, int32_t propertyId);

/**
 * Initialize the buffer used for receiving data.
 * @return {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} on success, and an implementation defined error code on failure.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_initializeRXData(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * @return the number of bytes waiting in the RX buffer
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_isRXDataAvailable(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the next character from the RX buffer
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_getNextRXData(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Initialize the buffer used for sending data.
 * @return {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} on success, and an implementation defined error code on failure.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_initializeTXData(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Try to send a character over the serial line.
 * @return {@link LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_ERROR} if the character cannot be sent (try again later), {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} otherwise.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_sendData(LLCOMM_CUSTOM_CONNECTION* env, int32_t b);

/**
 * Wait until the transmit buffers (if any) are empty and the transmission unit is idle, or a timeout occurs. 
 * @return {@link LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_COMPLETE} when the transmission is complete, {@link LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_TIMEOUT} when a timeout occurs. 
 */
int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_isTransmissionComplete(LLCOMM_CUSTOM_CONNECTION* env);
