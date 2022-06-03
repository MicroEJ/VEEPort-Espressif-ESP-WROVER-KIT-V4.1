/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef WIFI_ESP32_DRIVER_H
#define WIFI_ESP32_DRIVER_H

#include <stdbool.h>
#include <stdint.h>
#include "lwip/inet.h"

/** @brief Wifi station interface name */
#define WLAN_STA_INTERFACE_NAME "st1"

/** @brief Wifi access point interface name */
#define WLAN_AP_INTERFACE_NAME  "ap2"

/**
 * @brief function to initialize the wifi module
 *
 * @return true if success else false
 */
bool WIFI_ESP32_initialize_f(void);

/**
 * @brief this function apply IP configuration from network interface
 *
 * @param[in] is_static IP configuration, true for static ip or false for dhcp
 * @param[in] ip ip address
 * @param[in] netmask netmask
 * @param[in] gw gateway
 *
 * @return true if success else false
 */
bool WIFI_ESP32_sta_apply_configuration_f(bool is_static, ip4_addr_t ip, ip4_addr_t netmask, ip4_addr_t gw);

/**
 * @brief this function apply IP configuration from network interface
 *
 * @param[in] is_static IP configuration, true for static ip or false for dhcp
 * @param[in] ip ip address
 * @param[in] netmask netmask
 * @param[in] gw gateway
 *
 * @return true if success else false
 */
bool WIFI_ESP32_ap_apply_configuration_f(bool is_static, ip4_addr_t ip, ip4_addr_t netmask, ip4_addr_t gw);

/**
 * @brief this function start the wifi module in STA mode
 *
 * @return true if success else false
 */
bool WIFI_ESP32_sta_start_f(void);

/**
 * @brief this function start the wifi module in AP mode
 *
 * @return true if success else false
 */
bool WIFI_ESP32_ap_start_f(void);

/**
 * @brief this function stop the STA wifi module
 *
 * @return true if success else false
 */
bool WIFI_ESP32_sta_stop_f(void);

/**
 * @brief this function stop the AP wifi module
 *
 * @return true if success else false
 */
bool WIFI_ESP32_ap_stop_f(void);

/**
 * @brief this function return if the STA network interface is started or not
 *
 * @param[out] _puc_netif_started '1' returned if started, '0' otherwise
 *
 * @return true if success else false
 */
bool WIFI_ESP32_sta_netif_is_started_f(unsigned char * _puc_netif_started);

/**
 * @brief this function return if the AP network interface is started or not
 *
 * @param[out] _puc_netif_started '1' returned if started, '0' otherwise
 *
 * @return true if success else false
 */
bool WIFI_ESP32_ap_netif_is_started_f(unsigned char * _puc_netif_started);

/**
 * @brief this function joins a WiFi network
 *
 * @param[in] _pc_ssid ssid of the network
 * @param[in] _i_ssid_length ssid length
 * @param[in] _pc_passphrase password needed for network connection
 * @param[in] _i_passphrase_length password length
 * @param[in] _i_security_mode security mode
 *
 * @return true if success else false
 */
bool WIFI_ESP32_join_f(const char * _pc_ssid, int _i_ssid_length, const char * _pc_passphrase, int _i_passphrase_length, int _i_security_mode);

/**
 * @brief this function leaves a WiFi network
 *
 * @return true if success else false
 */
bool WIFI_ESP32_leave_f(void);

/**
 * @brief this function returns the bssid of the WiFi network
 *
 * @param[out] _puc_bssid the buffer to be filled with the bssid
 * @param[in]  _i_bssid_length length to be filled in the buffer
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_bssid_f(unsigned char * _puc_bssid, int _i_bssid_length);

/**
 * @brief this function returns the channel of the WiFi network
 *
 * @param[out] _ps_channel the buffer to be filled
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_channel_f(short * _ps_channel);

/**
 * @brief this function returns the rssi of the WiFi network
 *
 * @param[out] _pf_rssi the buffer to be filled
 * @param[in]  _i_rssi_length length to be filled in the buffer
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_rssi_f(float * _pf_rssi, int _i_rssi_length);

/**
 * @brief this function returns the security mode of the WiFi network
 *
 * @param[out] _ps_mode the buffer to be filled
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_security_mode_f(short * _ps_mode);

/**
 * @brief this function returns the ssid of the WiFi network
 *
 * @param[out] _pc_ssid the buffer to be filled
 * @param[in]  _i_ssid_length length to be filled in the buffer
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ssid_f(char * _pc_ssid, int _i_ssid_length);

/**
 * @brief this function returns the WPS modes of the WiFi network
 *
 * @param[out] _ps_wps_modes the buffer to be filled
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_wps_modes_f(short * _ps_wps_modes);

/**
 * @brief this function enables the SoftAP for the module
 *
 * @param[in] _pc_ssid the ssid of the SoftAP
 * @param[in] _i_ssid_length the ssid length
 * @param[in] _pc_passphrase the password of the SoftAP
 * @param[in] _i_passphrase_length the password length
 *
 * @return true if success else false
 */
bool WIFI_ESP32_enable_softap_f(const char * _pc_ssid, int _i_ssid_length, const char * _pc_passphrase, int _i_passphrase_length);

/**
 * @brief this function changes the name of the SoftAP
 *
 * @param[in] _pc_name the ssid of the SoftAP
 * @param[in] _i_name_length the ssid length
 *
 * @return true if success else false
 */
bool WIFI_ESP32_set_name_softap_f(const char * _pc_name, int _i_name_length);

/**
 * @brief this function disables the SoftAP
 *
 * @return true if success else false
 */
bool WIFI_ESP32_disable_softap_f(void);

/**
 * @brief this function returns the access point count discovered
 *
 * @param[out] _ps_ap_count the access point count to be returned
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ap_count_f(short * const _ps_ap_count);

/**
 * @brief this function returns the ssid from the saved list
 *
 * @param[in]  _ui_index the index from the saved list
 * @param[out] _puc_ssid the buffer to be returned
 * @param[in]  _i_ssid_length the length to copy in the return buffer
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ap_ssid_f(unsigned int _ui_index, unsigned char * _puc_ssid, int _i_ssid_length);

/**
 * @brief this function returns the bssid from the saved list
 *
 * @param[in]  _ui_index the index from the saved list
 * @param[out] _puc_bssid the buffer to be returned
 * @param[in]  _i_bssid_length the length to copy in the return buffer
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ap_bssid_f(unsigned int _ui_index, unsigned char * _puc_bssid, int _i_bssid_length);

/**
 * @brief this function returns the channel from the saved list
 *
 * @param[in]  _ui_index the index from the saved list
 * @param[out] _pi_channel the buffer to be returned
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ap_channel_f(unsigned int _ui_index, int * _pi_channel);

/**
 * @brief this function returns the rssi from the saved list
 *
 * @param[in]  _ui_index the index from the saved list
 * @param[out] _pf_rssi the buffer to be returned
 * @param[in]  _i_rssi_length the length to copy in the return buffer
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ap_rssi_f(unsigned int _ui_index, float * _pf_rssi, int _i_rssi_length);

/**
 * @brief this function returns the security mode from the saved list
 *
 * @param[in]  _ui_index the index from the saved list
 * @param[out] _pi_mode the buffer to be returned
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ap_security_mode_f(unsigned int _ui_index, int * _pi_mode);

/**
 * @brief this function returns the wps mode from the saved list
 *
 * @param[in]  _ui_index the index from the saved list
 * @param[out] _pi_wps_cap the buffer to be returned
 *
 * @return true if success else false
 */
bool WIFI_ESP32_get_ap_wps_modes_f(unsigned int _ui_index, int * _pi_wps_cap);

/**
 * @brief this function returns the client state
 *
 * @return true if the client is started else false
 */
bool WIFI_ESP32_get_client_state_f(void);

/**
 * @brief this function returns the access point state
 *
 * @return true if the client is started else false
 */
bool WIFI_ESP32_get_access_point_state_f(void);

#endif /* WIFI_ESP32_DRIVER_H */
