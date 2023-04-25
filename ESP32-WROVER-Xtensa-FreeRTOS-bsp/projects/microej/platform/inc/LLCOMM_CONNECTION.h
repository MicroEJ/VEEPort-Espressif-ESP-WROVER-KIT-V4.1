/* 
 * IceTea
 * 
 * Copyright 2013-2016 IS2T. All rights reserved.
 * Modification and distribution is permitted under certain conditions.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/**
 * Defines communication stream APIs for any kind of COMM connection (UART, USB CDC, IRDA, ...).
 */
#ifndef _LLCOMM_CONNECTION
#define _LLCOMM_CONNECTION 
#include <intern/LLCOMM_CONNECTION.h>
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
#define LLCOMM_CONNECTION_PARITY_NO (0)

/**
 * Parity mode: even.
 */
#define LLCOMM_CONNECTION_PARITY_EVEN (1)

/**
 * Parity mode: odd.
 */
#define LLCOMM_CONNECTION_PARITY_ODD (2)

/**
 * Number of stop bits: 1
 */
#define LLCOMM_CONNECTION_STOPBITS_1 (0)

/**
 * Number of stop bits: 2
 */
#define LLCOMM_CONNECTION_STOPBITS_2 (1)

/**
 * Number of stop bits: 1.5
 */
#define LLCOMM_CONNECTION_STOPBITS_1_5 (2)

/**
 * Character length: 5 bits.
 */
#define LLCOMM_CONNECTION_DATABITS_5 (0)

/**
 * Character length: 6 bits.
 */
#define LLCOMM_CONNECTION_DATABITS_6 (1)

/**
 * Character length: 7 bits.
 */
#define LLCOMM_CONNECTION_DATABITS_7 (2)

/**
 * Character length: 8 bits.
 */
#define LLCOMM_CONNECTION_DATABITS_8 (3)

/**
 * Character length: 9 bits.
 */
#define LLCOMM_CONNECTION_DATABITS_9 (4)

/**
 * Return code meaning the operation was successfully performed.
 */
#define LLCOMM_CONNECTION_SUCCESS (0)

/**
 * Return code meaning the driver can not open a connection on given port
 */
#define LLCOMM_CONNECTION_ERROR_CANNOT_OPEN (-1)

/**
 * Error code returned by open requests to indicate that the requested baudrate is unsupported. 
 */
#define LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE (-2)

/**
 * Error code returned by open requests to indicate that the requested number of bits per character is unsupported. 
 */
#define LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR (-3)

/**
 * Error code returned by open requests to indicate that the requested number of stop bits is unsupported. 
 */
#define LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS (-4)

/**
 * Error code returned by open requests to indicate that the requested parity type is unsupported. 
 */
#define LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY (-5)

/**
 * Error code indicating memory allocation failure.
 */
#define LLCOMM_CONNECTION_ERROR_OUT_OF_MEMORY (-1)

/**
 * Return code meaning the stream cannot be open from a platform Id.
 */
#define LLCOMM_CONNECTION_NO_PLATFORM_ID (-1)

// --------------------------------------------------------------------------------
// -                      Functions provided by the library                       -
// --------------------------------------------------------------------------------

/**
 * Close the stream in both read and write directions.
 */
void LLCOMM_CONNECTION_close(LLCOMM_CONNECTION* env);

/**
 * @return true if stream is currently closed.
 */
uint8_t LLCOMM_CONNECTION_isClosed(LLCOMM_CONNECTION* env);

/**
 * Enable receiving bytes.
 */
int32_t LLCOMM_CONNECTION_enableRead(LLCOMM_CONNECTION* env);

/**
 * Enable sending bytes.
 */
int32_t LLCOMM_CONNECTION_enableWrite(LLCOMM_CONNECTION* env);

/**
 * Close read connection.
 */
void LLCOMM_CONNECTION_closeRead(LLCOMM_CONNECTION* env);

/**
 * Close write connection.
 */
void LLCOMM_CONNECTION_closeWrite(LLCOMM_CONNECTION* env);

/**
 * @return the number of bytes available in the read buffer.
 */
int32_t LLCOMM_CONNECTION_available(LLCOMM_CONNECTION* env);

/**
 * Read the next byte from the stream.
 * If there is nothing to read, this function blocks until a new byte is received.
 * @return the next byte; or a negative value if an error occurs.
 */
int32_t LLCOMM_CONNECTION_read(LLCOMM_CONNECTION* env);

/**
 * Send data.
 * If the write buffer is full or there is no buffer and the hardware is
 * busy, this function must lock until there is some space.
 * This function returns as soon as the value is added to the buffer.
 * Use flush() to ensure the data has been sent. 
 * The number of bits to send is stream dependant.
 * 
 * @param b the value to send.
 * @return 0 on success, or an implementation specific negative error code.
 */
int32_t LLCOMM_CONNECTION_write(LLCOMM_CONNECTION* env, int32_t b);

/**
 * Flush the write buffers (if any).
 * This function is locking until all bytes waiting in the write buffers
 * (hardware and software) have been sent.
 * @return 0 on success, or an implementation specific negative error code.
 */
int32_t LLCOMM_CONNECTION_flush(LLCOMM_CONNECTION* env);

/**
 * Initialize the comm driver and hardware to support this stream.
 */
void LLCOMM_CONNECTION_initialize(LLCOMM_CONNECTION* env);

/**
 * Configure a serial stream.
 * This function can only be used while the stream is in the closed state.
 * @param baudrate a positive integer
 * @param bitsperchar one of {@link #DATABITS_*}
 * @param stopbits one of {@link #STOPBITS_*}
 * @param parity one of {@link #PARITY_*}
 * @return 0 if initialization is correct, or negative error code within {@link #ERROR_INIT_*}
 */
int32_t LLCOMM_CONNECTION_configure(LLCOMM_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity);

/**
 * Enable RX hardware and start accepting data.
 */
void LLCOMM_CONNECTION_enableRX(LLCOMM_CONNECTION* env);

/**
 * Disable RX hardware and stop accepting data.
 */
void LLCOMM_CONNECTION_disableRX(LLCOMM_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when new data is received.
 */
void LLCOMM_CONNECTION_enableRXDeviceInterrupt(LLCOMM_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when new data is received.
 */
void LLCOMM_CONNECTION_disableRXDeviceInterrupt(LLCOMM_CONNECTION* env);

/**
 * Enable TX hardware and start sending data.
 */
void LLCOMM_CONNECTION_enableTX(LLCOMM_CONNECTION* env);

/**
 * Disable TX hardware and stop sending data.
 */
void LLCOMM_CONNECTION_disableTX(LLCOMM_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 * 
 * When the interrupt occurs the interrupt handler should call transmitBufferReady().
 */
void LLCOMM_CONNECTION_enableTXDeviceInterrupt(LLCOMM_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 */
void LLCOMM_CONNECTION_disableTXDeviceInterrupt(LLCOMM_CONNECTION* env);

/**
 * Configure a serial stream. A stream can be re-configured after it is closed.
 * @param baudrate a positive integer
 * @param bitsperchar {@link LLCOMM_CONNECTION_DATABITS_5} | {@link LLCOMM_CONNECTION_DATABITS_6} | {@link LLCOMM_CONNECTION_DATABITS_7} | {@link LLCOMM_CONNECTION_DATABITS_8} | {@link LLCOMM_CONNECTION_DATABITS_9}
 * @param stopbits {@link LLCOMM_CONNECTION_STOPBITS_1} | {@link LLCOMM_CONNECTION_STOPBITS_2} | {@link LLCOMM_CONNECTION_STOPBITS_1_5}
 * @param parity {@link LLCOMM_CONNECTION_PARITY_NO} | {@link LLCOMM_CONNECTION_PARITY_EVEN} | {@link LLCOMM_CONNECTION_PARITY_ODD}
 * @return {@link LLCOMM_CONNECTION_SUCCESS} if initialization is successful, or negative error code ({@link LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE} | {@link LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR} | {@link LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS} | {@link LLCOMM_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY} | {@link LLCOMM_CONNECTION_ERROR_CANNOT_OPEN})
 */
int32_t LLCOMM_CONNECTION_configureDevice(LLCOMM_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity);

/**
 * Get the name of this stream, if it can be open using a name.
 * @return a null terminated string, or <code>NULL</code> if this stream can only be open using a platform id
 */
void* LLCOMM_CONNECTION_getName(LLCOMM_CONNECTION* env);

/**
 * Get the platform id of this stream, if it can be open using a platform id.
 * @return a positive integer, or {@link LLCOMM_CONNECTION_NO_PLATFORM_ID} if this stream can only be open using a name
 */
int32_t LLCOMM_CONNECTION_getPlatformId(LLCOMM_CONNECTION* env);

/**
 * This function is called after the connection has been removed using {@link LLCOMM_removeConnection}, 
 * when it is no more handled by the application. If the connection was open when it was removed, this function will be called only after the connection 
 * will be closed by the application
 */
void LLCOMM_CONNECTION_released(LLCOMM_CONNECTION* env);

/**
 * Get the number of properties defined to identify this stream.
 * @return a positive integer (0 if this stream does not define any properties)
 */
int32_t LLCOMM_CONNECTION_getNbProperties(LLCOMM_CONNECTION* env);

/**
 * Get the value of the given property.
 * @param propertyName the property name (null terminated string)
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property
 */
void* LLCOMM_CONNECTION_getProperty(LLCOMM_CONNECTION* env, void* propertyName);

/**
 * Get the property name from property id.
 * @param propertyId an id between <code>0</code> and <code>{@link LLCOMM_CONNECTION_getNbProperties}-1</code>.
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property with the given id
 */
void* LLCOMM_CONNECTION_getPropertyName(LLCOMM_CONNECTION* env, int32_t propertyId);

#ifdef __cplusplus
	}
#endif
#endif
