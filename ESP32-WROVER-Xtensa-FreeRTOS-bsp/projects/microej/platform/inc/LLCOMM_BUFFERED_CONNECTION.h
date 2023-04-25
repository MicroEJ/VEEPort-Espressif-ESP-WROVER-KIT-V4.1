/* 
 * Icetea
 * 
 * Copyright 2013-2016 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms. 
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/**
 * Communication stream implementation that can manage RAM-based FIFO buffers for
 * reception and transmission.
 */
#ifndef _LLCOMM_BUFFERED_CONNECTION
#define _LLCOMM_BUFFERED_CONNECTION 
#include <intern/LLCOMM_BUFFERED_CONNECTION.h>
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
#define LLCOMM_BUFFERED_CONNECTION_PARITY_NO (0)

/**
 * Parity mode: even.
 */
#define LLCOMM_BUFFERED_CONNECTION_PARITY_EVEN (1)

/**
 * Parity mode: odd.
 */
#define LLCOMM_BUFFERED_CONNECTION_PARITY_ODD (2)

/**
 * Number of stop bits: 1
 */
#define LLCOMM_BUFFERED_CONNECTION_STOPBITS_1 (0)

/**
 * Number of stop bits: 2
 */
#define LLCOMM_BUFFERED_CONNECTION_STOPBITS_2 (1)

/**
 * Number of stop bits: 1.5
 */
#define LLCOMM_BUFFERED_CONNECTION_STOPBITS_1_5 (2)

/**
 * Character length: 5 bits.
 */
#define LLCOMM_BUFFERED_CONNECTION_DATABITS_5 (0)

/**
 * Character length: 6 bits.
 */
#define LLCOMM_BUFFERED_CONNECTION_DATABITS_6 (1)

/**
 * Character length: 7 bits.
 */
#define LLCOMM_BUFFERED_CONNECTION_DATABITS_7 (2)

/**
 * Character length: 8 bits.
 */
#define LLCOMM_BUFFERED_CONNECTION_DATABITS_8 (3)

/**
 * Character length: 9 bits.
 */
#define LLCOMM_BUFFERED_CONNECTION_DATABITS_9 (4)

/**
 * Return code meaning the operation was successfully performed.
 */
#define LLCOMM_BUFFERED_CONNECTION_SUCCESS (0)

/**
 * Return code meaning the driver can not open a connection on given port
 */
#define LLCOMM_BUFFERED_CONNECTION_ERROR_CANNOT_OPEN (-1)

/**
 * Error code returned by open requests to indicate that the requested baudrate is unsupported. 
 */
#define LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE (-2)

/**
 * Error code returned by open requests to indicate that the requested number of bits per character is unsupported. 
 */
#define LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR (-3)

/**
 * Error code returned by open requests to indicate that the requested number of stop bits is unsupported. 
 */
#define LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS (-4)

/**
 * Error code returned by open requests to indicate that the requested parity type is unsupported. 
 */
#define LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY (-5)

/**
 * Error code indicating memory allocation failure.
 */
#define LLCOMM_BUFFERED_CONNECTION_ERROR_OUT_OF_MEMORY (-1)

/**
 * Return code meaning the stream cannot be open from a platform Id.
 */
#define LLCOMM_BUFFERED_CONNECTION_NO_PLATFORM_ID (-1)

/**
 * Return code indicating the transmission is complete.
 */
#define LLCOMM_BUFFERED_CONNECTION_TRANSMISSION_COMPLETE (0)

/**
 * Return code indicating a transmission error.
 */
#define LLCOMM_BUFFERED_CONNECTION_TRANSMISSION_ERROR (-1)

/**
 * Return code indicating the transmission is not complete due a timeout.
 */
#define LLCOMM_BUFFERED_CONNECTION_TRANSMISSION_TIMEOUT (-2)

/**
 * Return code indicating the hardware does not have a hardware FIFO.
 */
#define LLCOMM_BUFFERED_CONNECTION_NO_HARDWARE_FIFO (-1)

/**
 * Return code indicating the hardware FIFO is full.
 */
#define LLCOMM_BUFFERED_CONNECTION_HARDWARE_FIFO_FULL (0)

/**
 * Return code indicating the hardware FIFO is not full.
 */
#define LLCOMM_BUFFERED_CONNECTION_HARDWARE_FIFO_NOT_FULL (1)

/**
 * Return code indicating this FIFO is full.
 */
#define LLCOMM_BUFFERED_CONNECTION_FIFO_FULL (-1)

/**
 * Return code indicating this closed.
 */
#define LLCOMM_BUFFERED_CONNECTION_RX_CLOSED (-2)

// --------------------------------------------------------------------------------
// -                      Functions provided by the library                       -
// --------------------------------------------------------------------------------

/**
 * Configure and enable receiving characters from the stream
 */
int32_t LLCOMM_BUFFERED_CONNECTION_enableRead(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Start sending bytes on the TX stream.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_enableWrite(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Disable receiving characters from the stream.
 */
void LLCOMM_BUFFERED_CONNECTION_closeRead(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Close the write channel of the stream. It will not be possible to write to it anymore.
 */
void LLCOMM_BUFFERED_CONNECTION_closeWrite(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Close the stream in both read and write directions.
 */
void LLCOMM_BUFFERED_CONNECTION_close(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Return the number of characters received that have not been read yet by the user
 */
int32_t LLCOMM_BUFFERED_CONNECTION_available(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Read the next character from the stream.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_read(LLCOMM_BUFFERED_CONNECTION* env);

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
int32_t LLCOMM_BUFFERED_CONNECTION_write(LLCOMM_BUFFERED_CONNECTION* env, int32_t b);

/**
 * Flush the write buffers (if any).
 * This function is locking until all bytes waiting in the write buffers
 * (hardware and software) have been sent.
 * @return 0 on success, or an implementation specific negative error code.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_flush(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * @return true if stream is currently closed.
 */
uint8_t LLCOMM_BUFFERED_CONNECTION_isClosed(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Configure a serial stream. A stream can be re-configured after it is closed.
 * @param baudrate a positive integer
 * @param bitsperchar {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_5} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_6} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_7} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_8} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_9}
 * @param stopbits {@link LLCOMM_BUFFERED_CONNECTION_STOPBITS_1} | {@link LLCOMM_BUFFERED_CONNECTION_STOPBITS_2} | {@link LLCOMM_BUFFERED_CONNECTION_STOPBITS_1_5}
 * @param parity {@link LLCOMM_BUFFERED_CONNECTION_PARITY_NO} | {@link LLCOMM_BUFFERED_CONNECTION_PARITY_EVEN} | {@link LLCOMM_BUFFERED_CONNECTION_PARITY_ODD}
 * @return {@link LLCOMM_BUFFERED_CONNECTION_SUCCESS} if initialization is successful, or negative error code ({@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE} | {@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR} | {@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS} | {@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY})
 */
int32_t LLCOMM_BUFFERED_CONNECTION_configure(LLCOMM_BUFFERED_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity);

/**
 * Call this method to signal the arrival of new received data. Normally called from
 * the interrupt handler.
 * @param data The data just received.
 * @return {@link LLCOMM_BUFFERED_CONNECTION_SUCCESS} if the data has been added, {@link LLCOMM_BUFFERED_CONNECTION_FIFO_FULL} if this stream is full or {@link LLCOMM_BUFFERED_CONNECTION_RX_CLOSED} if this connection has been closed.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_dataReceived(LLCOMM_BUFFERED_CONNECTION* env, int32_t data);

/**
 * Called under IT when the hardware transmit buffer is ready to receive new data to send
 */
void LLCOMM_BUFFERED_CONNECTION_transmitBufferReady(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Initialize the buffer used for receiving data.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_initializeRXData(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * @return the next byte from the reception buffer.
 * This method does not check if the buffer has data in it.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_getNextRXData(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Initialize the transmission buffer.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_initializeTXData(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Initialize the comm driver and hardware to support this stream.
 */
void LLCOMM_BUFFERED_CONNECTION_initialize(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Enable RX hardware and start accepting data.
 */
void LLCOMM_BUFFERED_CONNECTION_enableRX(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Disable RX hardware and stop accepting data.
 */
void LLCOMM_BUFFERED_CONNECTION_disableRX(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when new data is received.
 */
void LLCOMM_BUFFERED_CONNECTION_enableRXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when new data is received.
 */
void LLCOMM_BUFFERED_CONNECTION_disableRXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Enable TX hardware and start sending data.
 */
void LLCOMM_BUFFERED_CONNECTION_enableTX(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Disable TX hardware and stop sending data.
 */
void LLCOMM_BUFFERED_CONNECTION_disableTX(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Enable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 * 
 * When the interrupt occurs the interrupt handler should call transmitBufferReady().
 */
void LLCOMM_BUFFERED_CONNECTION_enableTXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Disable the interrupt that is triggered when there is free space in the hardware
 * transmit FIFO or register.
 */
void LLCOMM_BUFFERED_CONNECTION_disableTXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Configure a serial stream. A stream can be re-configured after it is closed.
 * @param baudrate a positive integer
 * @param bitsperchar {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_5} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_6} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_7} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_8} | {@link LLCOMM_BUFFERED_CONNECTION_DATABITS_9}
 * @param stopbits {@link LLCOMM_BUFFERED_CONNECTION_STOPBITS_1} | {@link LLCOMM_BUFFERED_CONNECTION_STOPBITS_2} | {@link LLCOMM_BUFFERED_CONNECTION_STOPBITS_1_5}
 * @param parity {@link LLCOMM_BUFFERED_CONNECTION_PARITY_NO} | {@link LLCOMM_BUFFERED_CONNECTION_PARITY_EVEN} | {@link LLCOMM_BUFFERED_CONNECTION_PARITY_ODD}
 * @return {@link LLCOMM_BUFFERED_CONNECTION_SUCCESS} if initialization is successful, or negative error code ({@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE} | {@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR} | {@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS} | {@link LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY} | {@link LLCOMM_BUFFERED_CONNECTION_ERROR_CANNOT_OPEN})
 */
int32_t LLCOMM_BUFFERED_CONNECTION_configureDevice(LLCOMM_BUFFERED_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity);

/**
 * Get the name of this stream, if it can be open using a name.
 * @return a null terminated string, or <code>NULL</code> if this stream can only be open using a platform id
 */
void* LLCOMM_BUFFERED_CONNECTION_getName(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Get the platform id of this stream, if it can be open using a platform id.
 * @return a positive integer, or {@link LLCOMM_BUFFERED_CONNECTION_NO_PLATFORM_ID} if this stream can only be open using a name
 */
int32_t LLCOMM_BUFFERED_CONNECTION_getPlatformId(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * This function is called after the connection has been removed using {@link LLCOMM_removeConnection}, 
 * when it is no more handled by the application. If the connection was open when it was removed, this function will be called only after the connection 
 * will be closed by the application
 */
void LLCOMM_BUFFERED_CONNECTION_released(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Get the number of properties defined to identify this stream.
 * @return a positive integer (0 if this stream does not define any properties)
 */
int32_t LLCOMM_BUFFERED_CONNECTION_getNbProperties(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Get the value of the given property.
 * @param propertyName the property name (null terminated string)
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property
 */
void* LLCOMM_BUFFERED_CONNECTION_getProperty(LLCOMM_BUFFERED_CONNECTION* env, void* propertyName);

/**
 * Get the property name from property id.
 * @param propertyId an id between <code>0</code> and <code>{@link LLCOMM_BUFFERED_CONNECTION_getNbProperties}-1</code>.
 * @return a null terminated string, or <code>NULL</code> if this stream does not define the property with the given id
 */
void* LLCOMM_BUFFERED_CONNECTION_getPropertyName(LLCOMM_BUFFERED_CONNECTION* env, int32_t propertyId);

/**
 * The driver is expected to allocate space for a RX buffer, used to hold incoming data.
 * @return the address of the RX buffer, or {@link LLCOMM_BUFFERED_CONNECTION_ERROR_OUT_OF_MEMORY} if allocation failed.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_getRXBufferStartAddress(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * @return the capacity of the RX buffer, in bytes. Negative values are reserved for errors.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_getRXBufferLength(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * The driver is expected to allocate space for a TX buffer, used to hold outgoing data.
 * @return the address of the TX buffer, or {@link LLCOMM_BUFFERED_CONNECTION_ERROR_OUT_OF_MEMORY} if allocation failed.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_getTXBufferStartAddress(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * @return the capacity of the TX buffer, in bytes. Negative values are reserved for errors.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_getTXBufferLength(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * The hardware might have a FIFO that buffers outgoing data (not to be confused with
 * the RAM FIFO buffer managed by the stream).
 * @return {@link LLCOMM_BUFFERED_CONNECTION_SUCCESS} if the hardware has a transmit FIFO, which may need to be flushed., {@link LLCOMM_BUFFERED_CONNECTION_NO_HARDWARE_FIFO} otherwise
 */
int32_t LLCOMM_BUFFERED_CONNECTION_useTXHardwareFIFO(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Check the state of the hardware transmit FIFO (if any).
 * @return {@link LLCOMM_BUFFERED_CONNECTION_HARDWARE_FIFO_FULL} if the FIFO is full, {@link LLCOMM_BUFFERED_CONNECTION_HARDWARE_FIFO_NOT_FULL} otherwise
 */
int32_t LLCOMM_BUFFERED_CONNECTION_txFifoFull(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Actually send the given value to the hardware for output on the serial line.
 * The most significant bits of the given value must be ignored, and only the
 * least significant ones used. The number of bits to use is given when configuring the port. 
 * @param data
 */
void LLCOMM_BUFFERED_CONNECTION_sendData(LLCOMM_BUFFERED_CONNECTION* env, int32_t data);

/**
 * Wait until the transmit buffers (hardware and software) are empty and the
 * transmission unit is idle, or a timeout occurs.
 * @return {@link LLCOMM_BUFFERED_CONNECTION_TRANSMISSION_COMPLETE} when the transmission is complete, {@link LLCOMM_BUFFERED_CONNECTION_TRANSMISSION_TIMEOUT} when a timeout occurs.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_isTransmissionComplete(LLCOMM_BUFFERED_CONNECTION* env);

/**
 * Call this method to signal the arrival of new received data. Normally called from
 * the interrupt handler.
 * @param data The data just received.
 */
int32_t LLCOMM_BUFFERED_CONNECTION_dataReceived(LLCOMM_BUFFERED_CONNECTION* env, int32_t data);

/**
 * Call this method to signal the availability of space in the transmit buffer. Normally
 * called from the interrupt handler.
 */
void LLCOMM_BUFFERED_CONNECTION_transmitBufferReady(LLCOMM_BUFFERED_CONNECTION* env);

#ifdef __cplusplus
	}
#endif
#endif
