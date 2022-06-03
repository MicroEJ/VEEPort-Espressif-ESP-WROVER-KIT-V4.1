/*
 * C
 *
 * Copyright 2016-2021 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
#ifndef _LLECOM_WIFI_IMPL
#define _LLECOM_WIFI_IMPL

/**
 * @file
 * @brief MicroEJ ECOM-WIFI low level API
 * @author MicroEJ Developer Team
 * @version 1.2.0
 * @date 17 June 2021
 */

#include <stdint.h>
#include <intern/LLECOM_WIFI_impl.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @brief list of Security Modes constants */
#define SECURITY_MODE_ENTERPRISE_NO_SECURITY		(0)
#define SECURITY_MODE_ENTERPRISE_WPA_MIXED			(1)   /* Mixed WPA1 / WPA2 */
#define SECURITY_MODE_ENTERPRISE_WPA1				(2)
#define SECURITY_MODE_ENTERPRISE_WPA2				(3)
#define SECURITY_MODE_ENTERPRISE_WEP				(4)
#define SECURITY_MODE_MIXED							(5)   /* Mixed WPA1 / WPA2 */
#define SECURITY_MODE_OPEN							(6)   /* Open (no authentification) */
#define SECURITY_MODE_WEP128						(7)
#define SECURITY_MODE_WEP64							(8)
#define SECURITY_MODE_WPA1							(9)
#define SECURITY_MODE_WPA2							(10)
#define SECURITY_MODE_WPA3							(11)
#define SECURITY_MODE_ENTERPRISE_WPA3				(12)
#define SECURITY_MODE_WPA2_WPA3_MIXED				(13)  /* Mixed WPA2 / WPA3 */
#define SECURITY_MODE_ENTERPRISE_WPA2_WPA3_MIXED	(14)  /* Mixed WPA2 / WPA3 */
#define SECURITY_MODE_UNKNOWN						(255) /* Unknown (for unsupported protocols) */

/** @brief list of WPS Modes constants */
#define WPS_MODE_NONE								(0)   /* No method */
#define WPS_MODE_NFC								(1)   /* Near Field Communication (NFC) method */
#define WPS_MODE_PIN								(2)   /* Personal Identification Number (PIN) method */
#define WPS_MODE_PUSHBUTTON							(4)   /* Push button method */
#define WPS_MODE_USB								(8)   /* USB method */

/** @brief list of Wifi Capabilities constants */
#define WIFI_CAPABILITY_BOTH_EXCLUSIVE 				(0)   /* The Wi-Fi module can act as a client and as an access point but not at the same time */
#define WIFI_CAPABILITY_BOTH_SIMULTANEOUS			(1)   /* The Wi-Fi module can act as a client and as an access point at the same time */
#define WIFI_CAPABILITY_CLIENT						(2)   /* Client. It is possible to join an existing wireless networks. The Wi-Fi module acts as a client. */
#define WIFI_CAPABILITY_SOFT_AP						(3)   /* SoftAP. It is possible to create a new wireless network. The Wi-Fi module acts as a access point. */

/** @brief list of Wifi State constants */
#define WIFI_STATE_IDLE                             (0)
#define WIFI_STATE_STARTED                          (1)
/** @brief list of Wifi Client State constants for compatibility */
#define WIFI_CLIENT_STATE_IDLE                      WIFI_STATE_IDLE
#define WIFI_CLIENT_STATE_STARTED                   WIFI_STATE_STARTED

/**
 * Disable SoftAP.
 *
 * @param getResult
 *            false to post the request, true to get the result
 *
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_disableSoftAP(int8_t getResult);

/**
 * Enable SoftAP.
 *
 * @param ssid
 *            the buffer into which the ssid will be stored
 * @param ssidOffset
 *            the offset where the ssid will be stored in the buffer
 * @param ssidLength
 *            the ssid buffer length
 * @param passphrase
 *            passphrase
 * @param passphraseOffset
 *            the offset where the passphrase will be store in the buffer
 * @param passphraseLength
 *            the passphrase buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_enableSoftAP(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult);

/**
 * Set Name of SoftAP.
 *
 * @param name
 *            the buffer into which the name will be stored
 * @param nameOffset
 *            the offset where the name will be stored in the buffer
 * @param nameLength
 *            the name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_setNameSoftAP(int8_t* name, int32_t nameOffset, int32_t nameLength, int8_t getResult);


/**
 * Get the BSSID of the access point.
 *
 * @param bssid
 *            the buffer into which the bssid will be stored (bssid is 48-bit long)
 * @param bssidOffset
 *            the offset where the bssid will be stored in the buffer
 * @param bssidLength
 *            the bssid buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getBSSID(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int8_t getResult);

/**
 * Get the channel of the access point.
 *
 * @param getResult
 *            false to post the request, true to get the result
 * @return a positive integer representing the channel if success, a negative error code if error
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getChannel(int8_t getResult);

/**
 * Get the state of the station.
 *
 * {@link WIFI_STATE_IDLE} is returned to signal Wi-Fi disconnection.
 *
 * @param getResult 0 to post the request, anything else to get the result
 * @return {@link WIFI_STATE_IDLE}, {@link WIFI_STATE_STARTED} or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getClientState(int8_t getResult);

/**
 * Get the state of the soft AP.
 *
 * @param getResult 0 to post the request, anything else to get the result
 * @return {@link WIFI_STATE_IDLE}, {@link WIFI_STATE_STARTED} or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getAccessPointState(int8_t getResult);

/**
 * Get the RSSI of the access point.
 *
 * @param rssi
 *            the buffer into which the rssi will be stored (data stored on 1 byte)
 * @param rssiOffset
 *            the offset where the rssi will be stored in the buffer
 * @param rssiLength
 *            the rssi buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getRSSI(float* rssi, int32_t rssiOffset, int32_t rssiLength, int8_t getResult);

/**
 * Get the security mode of the access point as a string.
 *
 * @param getResult
 *            false to post the request, true to get the result
 * @return a positive integer representing the security mode if success, a negative error code if error
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getSecurityMode(int8_t getResult);

/**
 * Get the SSID of the access point.
 *
 * @param ssid
 *            the buffer into which the ssid will be stored
 * @param ssidOffset
 *            the offset where the ssid will be stored in the buffer
 * @param ssidLength
 *            the ssid buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getSSID(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t getResult);

/**
 * Get the WPS modes of the access point.
 *
 * @param getResult
 *            false to post the request, true to get the result
 * @return a positive integer representing the WPS modes if success, a negative error code if error
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getWPSModes(int8_t getResult);

/**
 * Join a wireless network using SECURITY_MODE_WPA2.
 *
 * @param ssid
 *            the buffer into which the ssid will be stored
 * @param ssidOffset
 *            the offset where the ssid will be stored in the buffer
 * @param ssidLength
 *            the ssid buffer length
 * @param passphrase
 *            the buffer into which the passphrase will be stored
 * @param passphraseOffset
 *            the offset where the passphrase will be store in the buffer
 * @param passphraseLength
 *            the passphrase buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_join(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult);

/**
 * Join a wireless network.
 *
 * @param ssid
 *            the buffer into which the ssid will be stored
 * @param ssidOffset
 *            the offset where the ssid will be stored in the buffer
 * @param ssidLength
 *            the ssid buffer length
 * @param passphrase
 *            the buffer into which the passphrase will be stored
 * @param passphraseOffset
 *            the offset where the passphrase will be store in the buffer
 * @param passphraseLength
 *            the passphrase buffer length
 * @param securityModeValue
 *            the ssid security mode value. If it is SECURITY_MODE_UNKNOWN,
 *            join with any security mode when possible.
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_joinWithSecurityMode(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue, int8_t getResult);

/**
 * Leave wireless network.
 *
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_leave(int8_t getResult);

/**
 * Return the number of Wifi Access Point currently available.
 *
 * @param active
 *            true for an active scan, false for a passive scan
 * @param getResult
 *            false to post the request, true to get the result
 * @return the number of access point currently available or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_scanAPCount(int8_t active, int8_t getResult);

/**
 * Return the ssid currently available at index (in Wifi Access Point
 * previous listed).
 *
 * @param ssid
 *            the buffer into which the ssid will be stored
 * @param ssidOffset
 *            the offset where the ssid will be stored in the buffer
 * @param ssidLength
 *            the ssid buffer length
 * @param index
 *            index of the Access Point currently available
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 if success, a negative error code if error
 */
int32_t LLECOM_WIFI_IMPL_scanAPssidAt(int8_t* ssid, int32_t ssidOffset,	int32_t ssidLength, int32_t index, int8_t getResult);

/**
 * Return the bssid currently available at index (in Wifi Access Point
 * previous listed).
 *
 * @param bssid
 *            the buffer into which the bssid will be stored
 * @param bssidOffset
 *            the offset where the bssid will be stored in the buffer
 * @param bssidLength
 *            the bssid buffer length
 * @param index
 *            index of the Access Point currently available
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 if success, a negative error code if error
 */
int32_t LLECOM_WIFI_IMPL_scanAPbssidAt(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int32_t index, int8_t getResult);

/**
 * Return the channel currently available at index (in Wifi Access Point
 * previous listed).
 *
 * @param index
 *            index of the Access Point currently available
 * @param getResult
 *            false to post the request, true to get the result
 * @return a positive integer representing the channel if success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_scanAPchannelAt(int32_t index, int8_t getResult);

/**
 * Return the rssi currently available at index (in Wifi Access Point
 * previous listed).
 *
 * @param rssi
 *            the buffer into which the rssi will be stored
 * @param rssiOffset
 *            the offset where the rssi will be stored in the buffer
 * @param rssiLength
 *            the rssi buffer length
 * @param index
 *            index of the Access Point currently available
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 if success, a negative error code if error
 */
int32_t LLECOM_WIFI_IMPL_scanAPrssiAt(float* rssi, int32_t rssiOffset, int32_t rssiLength, int32_t index, int8_t getResult);

/**
 * Return the mode currently available at index (in Wifi Access Point
 * previous listed).
 *
 * @param index
 *            index of the Access Point currently available
 * @param getResult
 *            false to post the request, true to get the result
 * @return a positive integer representing the security mode if success, or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_scanAPsecurityModeAt(int32_t index, int8_t getResult);

/**
 * Return the wpsModes currently available at index (in Wifi Access Point
 * previous listed).
 *
 * @param index
 *            index of the Access Point currently available
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 if success, a negative error code if error
 */
int32_t LLECOM_WIFI_IMPL_scanAPwpsModeAt(int32_t index, int8_t getResult);

/**
 * Return Wifi capability of the underlying hardware.
 *
 * @param getResult
 *            false to post the request, true to get the result
 * @return a positive integer representing the Wifi capability if success, or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_WIFI_IMPL_getCapability(int8_t getResult);

#ifdef __cplusplus
}
#endif
#endif // _LLECOM_WIFI_IMPL
