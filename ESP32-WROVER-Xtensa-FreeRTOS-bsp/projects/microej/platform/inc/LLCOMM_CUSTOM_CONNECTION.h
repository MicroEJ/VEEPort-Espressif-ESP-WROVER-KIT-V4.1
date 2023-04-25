/* 
 * Icetea
 * 
 * Copyright 2013-2014 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms. 
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/**
 * Unbuffered implementation of a communication stream. Only use this one if you want to
 * handle buffering of sent and received data yourself, or if the hardware FIFOs
 * are big enough for your use.
 */
#ifndef _LLCOMM_CUSTOM_CONNECTION
#define _LLCOMM_CUSTOM_CONNECTION 
#include <intern/LLCOMM_CUSTOM_CONNECTION.h>
#include <stdint.h>
#ifdef __cplusplus
	extern "C" {
#endif
// --------------------------------------------------------------------------------
// -                                  Constants                                   -
// --------------------------------------------------------------------------------

/**
 * Parity mode: no parity.
 */
#define LLCOMM_CUSTOM_CONNECTION_PARITY_NO (0)

/**
 * Parity mode: even.
 */
#define LLCOMM_CUSTOM_CONNECTION_PARITY_EVEN (1)

/**
 * Parity mode: odd.
 */
#define LLCOMM_CUSTOM_CONNECTION_PARITY_ODD (2)

/**
 * Number of stop bits: 1
 */
#define LLCOMM_CUSTOM_CONNECTION_STOPBITS_1 (0)

/**
 * Number of stop bits: 2
 */
#define LLCOMM_CUSTOM_CONNECTION_STOPBITS_2 (1)

/**
 * Number of stop bits: 1.5
 */
#define LLCOMM_CUSTOM_CONNECTION_STOPBITS_1_5 (2)

/**
 * Character length: 5 bits.
 */
#define LLCOMM_CUSTOM_CONNECTION_DATABITS_5 (0)

/**
 * Character length: 6 bits.
 */
#define LLCOMM_CUSTOM_CONNECTION_DATABITS_6 (1)

/**
 * Character length: 7 bits.
 */
#define LLCOMM_CUSTOM_CONNECTION_DATABITS_7 (2)

/**
 * Character length: 8 bits.
 */
#define LLCOMM_CUSTOM_CONNECTION_DATABITS_8 (3)

/**
 * Character length: 9 bits.
 */
#define LLCOMM_CUSTOM_CONNECTION_DATABITS_9 (4)

/**
 * Return code meaning the operation was successfully performed.
 */
#define LLCOMM_CUSTOM_CONNECTION_SUCCESS (0)

/**
 * Return code meaning the driver can not open a connection on given port
 */
#define LLCOMM_CUSTOM_CONNECTION_ERROR_CANNOT_OPEN (-1)

/**
 * Error code returned by open requests to indicate that the requested baudrate is unsupported. 
 */
#define LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE (-2)

/**
 * Error code returned by open requests to indicate that the requested number of bits per character is unsupported. 
 */
#define LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR (-3)

/**
 * Error code returned by open requests to indicate that the requested number of stop bits is unsupported. 
 */
#define LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS (-4)

/**
 * Error code returned by open requests to indicate that the requested parity type is unsupported. 
 */
#define LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY (-5)

/**
 * Error code indicating memory allocation failure.
 */
#define LLCOMM_CUSTOM_CONNECTION_ERROR_OUT_OF_MEMORY (-1)

/**
 * Return code meaning the stream cannot be open from a platform Id.
 */
#define LLCOMM_CUSTOM_CONNECTION_NO_PLATFORM_ID (-1)

/**
 * Return code indicating the transmission is complete.
 */
#define LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_COMPLETE (0)

/**
 * Return code indicating a transmission error.
 */
#define LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_ERROR (-1)

/**
 * Return code indicating the transmission is not complete due a timeout.
 */
#define LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_TIMEOUT (-2)

// --------------------------------------------------------------------------------
// -                      Functions provided by the library                       -
// --------------------------------------------------------------------------------

/**
 * Configure and enable receiving characters from the stream
 */
int32_t LLCOMM_CUSTOM_CONNECTION_enableRead(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Start sending bytes on the TX stream.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_enableWrite(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable receiving characters from the stream.
 */
void LLCOMM_CUSTOM_CONNECTION_closeRead(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Stop sending bytes on the TX stream.
 */
void LLCOMM_CUSTOM_CONNECTION_closeWrite(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Close the stream in both read and write directions.
 */
void LLCOMM_CUSTOM_CONNECTION_close(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Return the number of characters received that have not been read yet by the user
 */
int32_t LLCOMM_CUSTOM_CONNECTION_available(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Read the next character from the stream.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_read(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Send data.
 * If it is not possible to write the byte right away, this function should
 * wait until the situation is resolved.
 * Use flush() to ensure the data has been sent. 
 * The number of bits to send is stream dependant.
 * 
 * @param b the value to send.
 * @return 0 on success, or an implementation specific negative error code.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_write(LLCOMM_CUSTOM_CONNECTION* env, int32_t b);

/**
 * Flush the write buffers (if any).
 * This function is locking until all bytes waiting in the write buffers
 * (hardware and software) have been sent.
 * @return 0 on success, or an implementation specific negative error code.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_flush(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * @return true if stream is currently closed.
 */
uint8_t LLCOMM_CUSTOM_CONNECTION_isClosed(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Configure a serial stream. A stream can be re-configured after it is closed.
 * @param baudrate a positive integer
 * @param bitsperchar {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_5} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_6} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_7} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_8} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_9}
 * @param stopbits {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_1} | {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_2} | {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_1_5}
 * @param parity {@link LLCOMM_CUSTOM_CONNECTION_PARITY_NO} | {@link LLCOMM_CUSTOM_CONNECTION_PARITY_EVEN} | {@link LLCOMM_CUSTOM_CONNECTION_PARITY_ODD}
 * @return {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} if initialization is successful, or negative error code ({@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY})
 */
int32_t LLCOMM_CUSTOM_CONNECTION_configure(LLCOMM_CUSTOM_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity);

/**
 * Call this method to signal the arrival of new data in the RX stream.
 */
void LLCOMM_CUSTOM_CONNECTION_dataReceived(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Call this method to signal availability of space in the TX buffer.
 */
void LLCOMM_CUSTOM_CONNECTION_transmitBufferReady(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Initialize the comm driver and hardware to support this stream.
 */
void LLCOMM_CUSTOM_CONNECTION_initialize(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable RX hardware and start accepting data.
 */
void LLCOMM_CUSTOM_CONNECTION_enableRX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable RX hardware and stop accepting data.
 */
void LLCOMM_CUSTOM_CONNECTION_disableRX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when new data is received.
 */
void LLCOMM_CUSTOM_CONNECTION_enableRXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when new data is received.
 */
void LLCOMM_CUSTOM_CONNECTION_disableRXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable TX hardware and start sending data.
 */
void LLCOMM_CUSTOM_CONNECTION_enableTX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable TX hardware and stop sending data.
 */
void LLCOMM_CUSTOM_CONNECTION_disableTX(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 * 
 * When the interrupt occurs the interrupt handler should call transmitBufferReady().
 */
void LLCOMM_CUSTOM_CONNECTION_enableTXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 */
void LLCOMM_CUSTOM_CONNECTION_disableTXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Configure a serial stream. A stream can be re-configured after it is closed.
 * @param baudrate a positive integer
 * @param bitsperchar {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_5} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_6} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_7} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_8} | {@link LLCOMM_CUSTOM_CONNECTION_DATABITS_9}
 * @param stopbits {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_1} | {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_2} | {@link LLCOMM_CUSTOM_CONNECTION_STOPBITS_1_5}
 * @param parity {@link LLCOMM_CUSTOM_CONNECTION_PARITY_NO} | {@link LLCOMM_CUSTOM_CONNECTION_PARITY_EVEN} | {@link LLCOMM_CUSTOM_CONNECTION_PARITY_ODD}
 * @return {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} if initialization is successful, or negative error code ({@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY} | {@link LLCOMM_CUSTOM_CONNECTION_ERROR_CANNOT_OPEN})
 */
int32_t LLCOMM_CUSTOM_CONNECTION_configureDevice(LLCOMM_CUSTOM_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity);

/**
 * Get the name of this stream, if it can be open using a name.
 * @return a null terminated string, or <code>NULL</code> if this stream can only be open using a platform id
 */
void* LLCOMM_CUSTOM_CONNECTION_getName(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the platform id of this stream, if it can be open using a platform id.
 * @return a positive integer, or {@link LLCOMM_CUSTOM_CONNECTION_NO_PLATFORM_ID} if this stream can only be open using a name
 */
int32_t LLCOMM_CUSTOM_CONNECTION_getPlatformId(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * This function is called after the connection has been removed using {@link LLCOMM_removeConnection}, 
 * when it is no more handled by the application. If the connection was open when it was removed, this function will be called only after the connection 
 * will be closed by the application
 */
void LLCOMM_CUSTOM_CONNECTION_released(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the number of properties defined to identify this stream.
 * @return a positive integer (0 if this stream does not define any properties)
 */
int32_t LLCOMM_CUSTOM_CONNECTION_getNbProperties(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the value of the given property.
 * @param propertyName the property name (null terminated string)
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property
 */
void* LLCOMM_CUSTOM_CONNECTION_getProperty(LLCOMM_CUSTOM_CONNECTION* env, void* propertyName);

/**
 * Get the property name from property id.
 * @param propertyId an id between <code>0</code> and <code>{@link LLCOMM_CUSTOM_CONNECTION_getNbProperties}-1</code>.
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property with the given id
 */
void* LLCOMM_CUSTOM_CONNECTION_getPropertyName(LLCOMM_CUSTOM_CONNECTION* env, int32_t propertyId);

/**
 * Initialize the buffer used for receiving data.
 * @return {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} on success, and an implementation defined error code on failure.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_initializeRXData(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * @return the number of bytes waiting in the RX buffer
 */
int32_t LLCOMM_CUSTOM_CONNECTION_isRXDataAvailable(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Get the next character from the RX buffer
 */
int32_t LLCOMM_CUSTOM_CONNECTION_getNextRXData(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Initialize the buffer used for sending data.
 * @return {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} on success, and an implementation defined error code on failure.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_initializeTXData(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Try to send a character over the serial line.
 * @return {@link LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_ERROR} if the character cannot be sent (try again later), {@link LLCOMM_CUSTOM_CONNECTION_SUCCESS} otherwise.
 */
int32_t LLCOMM_CUSTOM_CONNECTION_sendData(LLCOMM_CUSTOM_CONNECTION* env, int32_t b);

/**
 * Wait until the transmit buffers (if any) are empty and the transmission unit is idle, or a timeout occurs. 
 * @return {@link LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_COMPLETE} when the transmission is complete, {@link LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_TIMEOUT} when a timeout occurs. 
 */
int32_t LLCOMM_CUSTOM_CONNECTION_isTransmissionComplete(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Call this method to signal the arrival of new data in the RX stream.
 * @param data The character just received.
 */
void LLCOMM_CUSTOM_CONNECTION_dataReceived(LLCOMM_CUSTOM_CONNECTION* env);

/**
 * Call this method to signal the availability of space in the transmit buffer.
 */
void LLCOMM_CUSTOM_CONNECTION_transmitBufferReady(LLCOMM_CUSTOM_CONNECTION* env);

#ifdef __cplusplus
	}
#endif
#endif
