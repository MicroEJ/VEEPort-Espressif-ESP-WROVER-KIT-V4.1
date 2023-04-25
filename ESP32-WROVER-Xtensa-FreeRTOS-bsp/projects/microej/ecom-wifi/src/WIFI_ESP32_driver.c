/*
 * C
 *
 * Copyright 2018-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Toolchain includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ESP32 includes */
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "lwip/inet.h"

/* External includes */
#include "LLECOM_WIFI_impl.h"

/* Internal includes */
#include "WIFI_ESP32_driver.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////Macro definitions//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**@brief Define this value to enable assert */
#undef ENABLE_ASSERT

/**@brief Assert macro */
#ifdef ENABLE_ASSERT
#define WIFI_ESP32_ASSERT(x)                                         \
    do                                                               \
    {                                                                \
        if (!(x))                                                    \
        {                                                            \
            WIFI_ESP32_ASSERT_TRACE("%s, %d\n", __FILE__, __LINE__); \
            while (1)                                                \
                ;                                                    \
        }                                                            \
    } while (0)
#else
#define WIFI_ESP32_ASSERT(x)                                         \
    do                                                               \
    {                                                                \
        if (!(x))                                                    \
        {                                                            \
            WIFI_ESP32_ASSERT_TRACE("%s, %d\n", __FILE__, __LINE__); \
        }                                                            \
    } while (0)
#endif

/**@brief Log priority levels */
#define WIFI_ESP32_LOG_DEBUG 0
#define WIFI_ESP32_LOG_INFO 1
#define WIFI_ESP32_LOG_WARNING 2
#define WIFI_ESP32_LOG_ERROR 3
#define WIFI_ESP32_LOG_ASSERT 4
#define WIFI_ESP32_LOG_NONE 5

/**@brief Current log level */
#define WIFI_ESP32_LOG_LEVEL WIFI_ESP32_LOG_WARNING

#ifndef WIFI_ESP32_LOG_LEVEL
#error "WIFI_ESP32_LOG_LEVEL must be defined"
#endif

/**@brief Debug logger */
#if (WIFI_ESP32_LOG_DEBUG >= WIFI_ESP32_LOG_LEVEL)
#define WIFI_ESP32_DEBUG_TRACE             \
    printf("[ESP32 WiFi Driver][DEBUG] "); \
    printf
#else
#define WIFI_ESP32_DEBUG_TRACE(...) ((void)0)
#endif

/**@brief Info logger */
#if (WIFI_ESP32_LOG_INFO >= WIFI_ESP32_LOG_LEVEL)
#define WIFI_ESP32_INFO_TRACE             \
    printf("[ESP32 WiFi Driver][INFO] "); \
    printf
#else
#define WIFI_ESP32_INFO_TRACE(...) ((void)0)
#endif

/**@brief Warning logger */
#if (WIFI_ESP32_LOG_WARNING >= WIFI_ESP32_LOG_LEVEL)
#define WIFI_ESP32_WARNING_TRACE             \
    printf("[ESP32 WiFi Driver][WARNING] "); \
    printf
#else
#define WIFI_ESP32_WARNING_TRACE(...) ((void)0)
#endif

/**@brief Error logger */
#if (WIFI_ESP32_LOG_ERROR >= WIFI_ESP32_LOG_LEVEL)
#define WIFI_ESP32_ERROR_TRACE             \
    printf("[ESP32 WiFi Driver][ERROR] "); \
    printf
#else
#define WIFI_ESP32_ERROR_TRACE(...) ((void)0)
#endif

/**@brief Assert logger */
#if (WIFI_ESP32_LOG_ASSERT >= WIFI_ESP32_LOG_LEVEL)
#define WIFI_ESP32_ASSERT_TRACE             \
    printf("[ESP32 WiFi Driver][ASSERT] "); \
    printf
#else
#define WIFI_ESP32_ASSERT_TRACE(...) ((void)0)
#endif

/** @brief Event bits */
#define CONNECTED_BIT BIT0
#define DISCONNECTED_BIT BIT1
#define START_BIT BIT2
#define SCAN_BIT BIT3
#define STOP_BIT BIT4
#define INITIALIZED_BIT BIT5

/** @brief Timeouts */
#define DISCONNECT_TIMEOUT_MS 5000
#define CONNECT_TIMEOUT_MS 5000
#define START_TIMEOUT_MS 5000
#define STOP_TIMEOUT_MS 5000
#define SCAN_TIMEOUT_MS 15000
#define INITIALIZED_TIMEOUT_MS 5000

/** @brief Minimum RSSI threshold to scan for */
#define WIFI_MIN_RSSI_THRESHOLD -128

/** @brief Max connections that can be made to the Soft AP */
#define WIFI_SOFT_AP_MAX_CONNECTIONS 4

/** @brief Beacon interval for the Soft AP */
#define WIFI_SOFT_AP_BEACON_INTERVAL 100

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////Type definitions//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief ESP32 mode connection state */
typedef enum
{
    WIFI_ESP32_MODE_NOT_STARTED = 0x00,          /**< mode not started */
    WIFI_ESP32_MODE_IDLE = (0x01 << 0),          /**< mode idle */
    WIFI_ESP32_MODE_CONNECTING = (0x01 << 1),    /**< mode connecting (STA to an AP) */
    WIFI_ESP32_MODE_CONNECTED = (0x01 << 2),     /**< mode connected (STA to an AP) */
    WIFI_ESP32_MODE_DISCONNECTING = (0x01 << 3), /**< mode disconnecting from an AP */
} WIFI_ESP32_connection_state_t;

/** @brief ESP32 driver configuration structure */
typedef struct
{
    WIFI_ESP32_connection_state_t state;
} WIFI_ESP32_mode_t;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////Global Variables//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief Global ESP32 driver status (true=initalized)*/
static bool esp32_driver_state_b = false;

/** @brief Global STA state */
static WIFI_ESP32_mode_t sta_mode;

/** @brief Global AP state */
static WIFI_ESP32_mode_t ap_mode;

/** @brief List of available access points */
static wifi_ap_record_t *gpst_available_ap_list;

/** @brief Number of available access points */
static uint16_t g_available_ap_count;

/** @brief Event group for WiFi management */
static EventGroupHandle_t gst_wifi_event_group;

/** @brief Default WiFi station configuration */
static const wifi_config_t g_wifi_sta_default_config = {
    .sta = {
        .ssid = {0},
        .password = {0},
        .scan_method = WIFI_ALL_CHANNEL_SCAN,
        .bssid_set = false,
        .bssid = {0},
        .channel = 0,
        .listen_interval = 0,
        .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        .threshold.rssi = WIFI_MIN_RSSI_THRESHOLD,
        .threshold.authmode = WIFI_AUTH_OPEN,
        .pmf_cfg.capable = false,
        .pmf_cfg.required = false}};

/** @brief Default WiFi access point configuration */
static const wifi_config_t g_wifi_ap_default_config = {
    .ap = {
        .ssid = {0},
        .password = {0},
        .ssid_len = 0,
        .channel = 0,
        .authmode = WIFI_AUTH_OPEN,
        .ssid_hidden = 0,
        .max_connection = WIFI_SOFT_AP_MAX_CONNECTIONS,
        .beacon_interval = WIFI_SOFT_AP_BEACON_INTERVAL}};

/** @brief Station network interface */
static esp_netif_t *sta_netif;

/** @brief Access Point network interface */
static esp_netif_t *ap_netif;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////Functions Prototypes////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief this function translates the security mode from LLNET mode to ESP32 mode
 *
 * @param[in]  _i_llnet_security_mode input security mode
 *
 * @return the ESP32 security mode
 */
static wifi_auth_mode_t convert_llnet_to_esp32_security_mode_f(int _i_llnet_security_mode);

/**
 * @brief this function translates the security mode from ESP32 mode to LLNET mode
 *
 * @param[in]  _esp32_security_type input security type
 * @param[in]  _esp32_cipher_type input cipher type
 *
 * @return the LLNET security mode
 */
static int convert_esp32_to_llnet_security_mode_f(wifi_auth_mode_t _esp32_security_type, wifi_cipher_type_t _esp32_cipher_type);

/**
 * @brief this function handles the received events from the WiFi stack
 *
 * @param[in] arg event context
 * @param[in] event_base event base (WiFi or IP)
 * @param[in] event_id event id (relative to a base)
 * @param[in] event_data event data (relative to the id)
 */
static void event_handler_f(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/**
 * @brief this function initializes the WiFi module
 *
 * @return true if success else false
 */
static bool initialize_wifi_f(void);

/**
 * @brief this function starts the WiFi module
 *
 * @param[in] mode operating mode to start
 * @param[in] wifi_config AP or SoftAP configuration
 *
 * @return true if success else false
 */
static bool start_wifi_f(wifi_mode_t mode, wifi_config_t *wifi_config);

/**
 * @brief this function stops the WiFi module
 *
 * @param[in] mode operating mode to stop
 *
 * @return true if success else false
 */
static bool stop_wifi_f(wifi_mode_t mode);

/**
 * @brief this function connects to an AP
 *
 * @return true if success else false
 */
static bool connect_f(void);

/**
 * @brief this function disconnects from an AP
 *
 * @return true if success else false
 */
static bool disconnect_f(void);

/**
 * @brief this function joins a WiFi AP or mounts a SoftAP
 *
 * @param[in] _pc_ssid SSID of the network
 * @param[in] _i_ssid_length SSID length
 * @param[in] _pc_passphrase password needed for network connection
 * @param[in] _i_passphrase_length password length
 * @param[in] _i_security_mode security mode
 * @param[in] _mode AP or SoftAP
 *
 * @return true if success else false
 */
static bool join_ap_or_softap_f(const char *_pc_ssid, int _i_ssid_length, const char *_pc_passphrase, int _i_passphrase_length, int _i_security_mode, wifi_mode_t _mode);

/**
 * @brief this function leaves a WiFi AP or unmounts a SoftAP
 *
 * @param[in] mode operating mode to leave
 *
 * @return true if success else false
 */
static bool leave_ap_or_softap_f(wifi_mode_t mode);

/**
 * @brief this function checks if the WiFi module is initialized
 *
 * @return true if initialized else false
 */
static bool is_wifi_initialized(void);

/**
 * @brief this function checks if the WiFi module is started in station mode
 *
 * @return true if started else false
 */
static bool is_wifi_sta_started(void);

/**
 * @brief this function checks if the WiFi module is started in access point mode
 *
 * @return true if started else false
 */
static bool is_wifi_ap_started(void);

/**
 * @brief this function checks if the WiFi module is connected to an AP
 *
 * @return true if started else false
 */
static bool is_wifi_sta_connected(void);

/**
 * @brief this function filters a list of Access Points based on identical SSID
 */
static void filter_duplicated_ap_records(void);

/**
 * @brief this function applies a network configuration to a specific network interface
 *
 * @param[in] netif network interface
 * @param[in] is_static true if static IP configuration needs to be applied on the netif, false otherwise
 * @param[in] ip static IP to be set
 * @param[in] netmask static netmask to be set
 * @param[in] gw static gateway to be set
 *
 * @return true if applied with success, false otherwise
 */
static bool apply_configuration_f(esp_netif_t *netif, bool is_static, ip4_addr_t ip, ip4_addr_t netmask, ip4_addr_t gw);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////Private Functions//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static wifi_auth_mode_t convert_llnet_to_esp32_security_mode_f(int _i_llnet_security_mode)
{
    wifi_auth_mode_t authmode;
    switch (_i_llnet_security_mode)
    {
    case SECURITY_MODE_OPEN:
        authmode = WIFI_AUTH_OPEN;
        break;

    case SECURITY_MODE_WEP64:
    case SECURITY_MODE_WEP128:
        authmode = WIFI_AUTH_WEP;
        break;

    case SECURITY_MODE_WPA1:
        authmode = WIFI_AUTH_WPA_PSK;
        break;

    case SECURITY_MODE_WPA2:
        authmode = WIFI_AUTH_WPA2_PSK;
        break;

    case SECURITY_MODE_MIXED:
        authmode = WIFI_AUTH_WPA_WPA2_PSK;
        break;

    case SECURITY_MODE_ENTERPRISE_WPA2:
        authmode = WIFI_AUTH_WPA2_ENTERPRISE;
        break;
    case SECURITY_MODE_WPA3:
        authmode = WIFI_AUTH_WPA3_PSK;
        break;
    case SECURITY_MODE_WPA2_WPA3_MIXED:
        authmode = WIFI_AUTH_WPA2_WPA3_PSK;
        break;
    case SECURITY_MODE_ENTERPRISE_NO_SECURITY:
    case SECURITY_MODE_ENTERPRISE_WPA_MIXED:
    case SECURITY_MODE_ENTERPRISE_WPA1:
    case SECURITY_MODE_ENTERPRISE_WEP:
    case SECURITY_MODE_ENTERPRISE_WPA3:
    case SECURITY_MODE_ENTERPRISE_WPA2_WPA3_MIXED:
    case SECURITY_MODE_UNKNOWN:
    default:
        authmode = (wifi_auth_mode_t)-1;
        break;
    }

    return authmode;
}

static int convert_esp32_to_llnet_security_mode_f(wifi_auth_mode_t _esp32_security_type, wifi_cipher_type_t _esp32_cipher_type)
{
    int _pi_llnet_security_mode;

    switch (_esp32_security_type)
    {
    case WIFI_AUTH_OPEN:
        _pi_llnet_security_mode = SECURITY_MODE_OPEN;
        break;

    case WIFI_AUTH_WEP:
        switch (_esp32_cipher_type)
        {
        case WIFI_CIPHER_TYPE_WEP40:
            _pi_llnet_security_mode = SECURITY_MODE_WEP64;
            break;
        case WIFI_CIPHER_TYPE_WEP104:
        case WIFI_CIPHER_TYPE_NONE: // Workaround for esp-idf bug, need to have a valid information for WEP AP
            _pi_llnet_security_mode = SECURITY_MODE_WEP128;
            break;
        default:
            _pi_llnet_security_mode = SECURITY_MODE_UNKNOWN;
            break;
        }
        break;

    case WIFI_AUTH_WPA_PSK:
        _pi_llnet_security_mode = SECURITY_MODE_WPA1;
        break;

    case WIFI_AUTH_WPA2_PSK:
        _pi_llnet_security_mode = SECURITY_MODE_WPA2;
        break;

    case WIFI_AUTH_WPA_WPA2_PSK:
        _pi_llnet_security_mode = SECURITY_MODE_MIXED;
        break;

    case WIFI_AUTH_WPA2_ENTERPRISE:
        _pi_llnet_security_mode = SECURITY_MODE_ENTERPRISE_WPA2;
        break;

    default:
        _pi_llnet_security_mode = SECURITY_MODE_UNKNOWN;
        break;
    }

    return _pi_llnet_security_mode;
}

static void event_handler_f(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_WIFI_READY:
            WIFI_ESP32_INFO_TRACE("WiFi module ready\n");
            break;
        case WIFI_EVENT_STA_AUTHMODE_CHANGE:
        {
            wifi_event_sta_authmode_change_t *event = (wifi_event_sta_authmode_change_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("The authentication mode of the connected AP changed from %d to %d\n", event->old_mode, event->new_mode);
            break;
        }

        case WIFI_EVENT_SCAN_DONE:
        {
            wifi_event_sta_scan_done_t *event = (wifi_event_sta_scan_done_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("Scan finished with status: %d, scan results: %d, scan id: %d\n",
                                  event->status, event->number, event->scan_id);

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, SCAN_BIT);
            break;
        }

        case WIFI_EVENT_STA_START:
            WIFI_ESP32_INFO_TRACE("Station started\n");

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, START_BIT);
            break;

        case WIFI_EVENT_AP_START:
        {
            esp_netif_ip_info_t ip_info = {0};
            esp_err_t status = esp_netif_get_ip_info(ap_netif, &ip_info);
            if (status == ESP_OK)
            {
                WIFI_ESP32_INFO_TRACE("SoftAP started with IP: %d.%d.%d.%d\n", IP2STR(&ip_info.ip));
            }
            else
            {
                WIFI_ESP32_ERROR_TRACE("Get interface's IP address information, (err=%x)\n", status);
            }

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, START_BIT);
            
            break;
        }

        case WIFI_EVENT_STA_STOP:
            WIFI_ESP32_INFO_TRACE("Station stopped\n");
            
            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, STOP_BIT);

            break;

        case WIFI_EVENT_AP_STOP:
            WIFI_ESP32_INFO_TRACE("SoftAP stopped\n");

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, STOP_BIT);

            break;

        case WIFI_EVENT_STA_CONNECTED:
        {
            wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("Station connected to AP with SSID: %s, BSSID: %02X:%02X:%02X:%02X:%02X:%02X, channel: %d, authmode: %d\n", event->ssid, event->bssid[0], event->bssid[1], event->bssid[2], event->bssid[3], event->bssid[4], event->bssid[5], event->channel, event->authmode);

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, CONNECTED_BIT);
            break;
        }

        case WIFI_EVENT_STA_DISCONNECTED:
        {
            wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("Station disconnected from AP with SSID: %s, BSSID: %02X:%02X:%02X:%02X:%02X:%02X, reason: %d\n", event->ssid, event->bssid[0], event->bssid[1], event->bssid[2], event->bssid[3], event->bssid[4], event->bssid[5], event->reason);

            sta_mode.state = WIFI_ESP32_MODE_IDLE;

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, DISCONNECTED_BIT);

            break;
        }

        case WIFI_EVENT_AP_STACONNECTED:
        {
            wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("A station with MAC: %02X:%02X:%02X:%02X:%02X:%02X connected to the softAP, aid: %d\n", event->mac[0], event->mac[1], event->mac[2], event->mac[3], event->mac[4], event->mac[5], event->aid);

            break;
        }

        case WIFI_EVENT_AP_STADISCONNECTED:
        {
            wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("A station with MAC: %02X:%02X:%02X:%02X:%02X:%02X disconnected from the softAP, aid: %d\n", event->mac[0], event->mac[1], event->mac[2], event->mac[3], event->mac[4], event->mac[5], event->aid);
                       
            break;
        }

        case WIFI_EVENT_AP_PROBEREQRECVED:
        {
            wifi_event_ap_probe_req_rx_t *event = (wifi_event_ap_probe_req_rx_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("A probe request with RSSI: %d was received on the softAP interface from a station with MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", event->rssi, event->mac[0], event->mac[1], event->mac[2], event->mac[3], event->mac[4], event->mac[5]);
            break;
        }

        case WIFI_EVENT_STA_WPS_ER_SUCCESS:
        case WIFI_EVENT_STA_WPS_ER_FAILED:
        case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
        case WIFI_EVENT_STA_WPS_ER_PIN:
        case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP:
            WIFI_ESP32_WARNING_TRACE("WPS event id %d received, not treated\n", event_id);
            break;

        default:
            WIFI_ESP32_WARNING_TRACE("Event id %d received for base %d, not treated\n", event_id, (int)event_base);
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("Station got IP: %d.%d.%d.%d\n", IP2STR(&event->ip_info.ip));
            break;
        }

        case IP_EVENT_STA_LOST_IP:
        {
            WIFI_ESP32_INFO_TRACE("Station lost IP\n");

            break;
        }
        case IP_EVENT_AP_STAIPASSIGNED:
        {
            ip_event_ap_staipassigned_t *event = (ip_event_ap_staipassigned_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("SoftAP assigns an IP: %d.%d.%d.%d to a connected station\n", IP2STR(&event->ip));
            break;
        }
        case IP_EVENT_GOT_IP6:
        {
            ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;

            (void)event;
            WIFI_ESP32_INFO_TRACE("Station got IPv6: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n", IPV62STR(event->ip6_info.ip));
            break;
        }

        case IP_EVENT_ETH_GOT_IP:
        case IP_EVENT_PPP_GOT_IP:
        case IP_EVENT_PPP_LOST_IP:
        {
            WIFI_ESP32_WARNING_TRACE("Ethernet/PPP event id %d received, not treated\n", event_id);
            break;
        }
        default:
        {
            WIFI_ESP32_WARNING_TRACE("Event id %d received for base %d, not treated\n", event_id, (int)event_base);
            break;
        }
        }
    }
    else
    {
        WIFI_ESP32_WARNING_TRACE("Event base %d received, not treated\n", (int)event_base);
    }
}

static bool initialize_wifi_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t status;


    /* Initialize WiFi module */
    status = esp_wifi_init(&cfg);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Init WiFi, (err=%x)\n", status);
        return false;
    }

    /* Set WiFi storage mode */
    status = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Set the WiFi API configuration storage type, (err=%x)\n", status);
        return false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

static bool start_wifi_f(wifi_mode_t mode, wifi_config_t *wifi_config)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);
    
    esp_interface_t wifi_interface;
    EventBits_t uxBits;
    wifi_mode_t _mode;
    esp_err_t status;

    /* Check WiFi mode */
    if (mode == WIFI_MODE_STA)
    {
        wifi_interface = ESP_IF_WIFI_STA;
        if (is_wifi_ap_started())
        {
            mode = WIFI_MODE_APSTA;
        }
    }
    else if (mode == WIFI_MODE_AP)
    {
        wifi_interface = ESP_IF_WIFI_AP;
        if (is_wifi_sta_started())
        {
            mode = WIFI_MODE_APSTA;
        }
    }
    else if (mode == WIFI_MODE_APSTA)
    {
        wifi_interface = ESP_IF_WIFI_AP;
    }
    else
    {
        WIFI_ESP32_ERROR_TRACE("Unsupported mode parameter = %d\n", mode);
        return false;
    }

    /* Get current WiFi mode */
    status = esp_wifi_get_mode(&_mode);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get current operating mode of WiFi, (err=%x)\n", status);
        return false;
    }

    /* Change the mode if it is different and only if the mixed mode is not already set */
    if ((_mode != mode) && (_mode != WIFI_MODE_APSTA))
    {
        /* Set new station mode */
        WIFI_ESP32_DEBUG_TRACE("(%s) Set new wifi mode: %d\n", __func__, mode);
        status = esp_wifi_set_mode(mode);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Set the WiFi operating mode, (err=%x)\n", status);
            return false;
        }
    }

    /* Set requested WiFi configuration */
    status = esp_wifi_set_config(wifi_interface, wifi_config);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Set the configuration of the ESP32 STA or AP, (err=%x)\n", status);
        return false;
    }

    /* CHeck if we need to start the module */
    if ((!is_wifi_sta_started() && !is_wifi_ap_started()) ||
        (!is_wifi_sta_started() && ((mode == WIFI_MODE_AP) || (mode == WIFI_MODE_APSTA))) ||
        (!is_wifi_ap_started() && ((mode == WIFI_MODE_AP) || (mode == WIFI_MODE_APSTA))))
    {
        /* Start wifi module */
        WIFI_ESP32_INFO_TRACE("(%s) Starting WiFi module...\n", __func__);
        status = esp_wifi_start();
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Start WiFi according to current configuration, (err=%x)\n", status);
            return false;
        }

        /* Wait for the start event */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, START_BIT, pdTRUE, pdTRUE, START_TIMEOUT_MS / portTICK_RATE_MS);
        if (!(uxBits & START_BIT))
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the start event ended in a timeout\n");
            return false;
        }
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

static bool stop_wifi_f(wifi_mode_t mode)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);
    
    EventBits_t uxBits;
    wifi_mode_t _mode;
    esp_err_t status;

    /* Check WiFi mode */
    if ((mode != WIFI_MODE_STA) && (mode != WIFI_MODE_AP) && (mode != WIFI_MODE_APSTA))
    {
        WIFI_ESP32_ERROR_TRACE("Unsupported mode parameter = %d\n", mode);
        return false;
    }

    /* Get current WiFi mode */
    status = esp_wifi_get_mode(&_mode);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get current operating mode of WiFi, (err=%x)\n", status);
        return false;
    }

    /* If the mixed mode is active and the requested mode is to stop a single interface, only change the mode */
    if ((_mode == WIFI_MODE_APSTA) && (mode != WIFI_MODE_APSTA))
    {
        if (mode == WIFI_MODE_STA)
        {
            /* Set AP WiFi mode */
            status = esp_wifi_set_mode(WIFI_MODE_AP);
            if (status != ESP_OK)
            {
                WIFI_ESP32_ERROR_TRACE("Set the WiFi operating mode, (err=%x)\n", status);
                return false;
            }
        }
        else
        {
            /* Set STA wifi mode */
            status = esp_wifi_set_mode(WIFI_MODE_STA);
            if (status != ESP_OK)
            {
                WIFI_ESP32_ERROR_TRACE("Set the WiFi operating mode, (err=%x)\n", status);
                return false;
            }
        }
    }
    else
    {
        /* Stop only if the current mode is equal to the requested mode or if the mixed mode is requested to be stopped */
        if ((_mode == mode) || (mode == WIFI_MODE_APSTA))
        {
            /* Stop WiFi module */
            WIFI_ESP32_INFO_TRACE("Stopping WiFi module...\n");
            status = esp_wifi_stop();
            if (status != ESP_OK)
            {
                WIFI_ESP32_ERROR_TRACE("Stop WiFi, (err=%x)\n", status);
                return false;
            }

            /* Wait for the stop event */
            uxBits = xEventGroupWaitBits(gst_wifi_event_group, STOP_BIT, pdTRUE, pdTRUE, STOP_TIMEOUT_MS / portTICK_RATE_MS);
            if (!(uxBits & STOP_BIT))
            {
                WIFI_ESP32_ERROR_TRACE("Waiting for the stop event ended in a timeout\n");
                return false;
            }
        }
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

static bool connect_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    EventBits_t uxBits;
    esp_err_t status;

    /* Connect */
    WIFI_ESP32_INFO_TRACE("Connecting to AP...\n");
    status = esp_wifi_connect();
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Connect the ESP32 WiFi station to the AP, (err=%x)\n", status);
        return false;
    }

    /* Wait for the connected event */
    uxBits = xEventGroupWaitBits(gst_wifi_event_group, CONNECTED_BIT, pdTRUE, pdTRUE, CONNECT_TIMEOUT_MS / portTICK_RATE_MS);
    if (!(uxBits & CONNECTED_BIT))
    {
        WIFI_ESP32_ERROR_TRACE("Waiting for the connect event ended in a timeout\n");
        status = esp_wifi_disconnect();
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Disconnect the ESP32 WiFi station from the AP, (err=%x)\n", status);
        }
        return false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

static bool disconnect_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    EventBits_t uxBits;
    esp_err_t status;

    WIFI_ESP32_INFO_TRACE("Disconnecting from AP...\n");
    status = esp_wifi_disconnect();
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Disconnect the ESP32 WiFi station from the AP, (err=%x)\n", status);
        return false;
    }

    /* Wait for the disconnected event */
    uxBits = xEventGroupWaitBits(gst_wifi_event_group, DISCONNECTED_BIT, pdTRUE, pdTRUE, DISCONNECT_TIMEOUT_MS / portTICK_RATE_MS);
    if (!(uxBits & DISCONNECTED_BIT))
    {
        WIFI_ESP32_ERROR_TRACE("Waiting for the disconnect event ended in a timeout\n");
        return false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

static bool join_ap_or_softap_f(const char *_pc_ssid, int _i_ssid_length, const char *_pc_passphrase, int _i_passphrase_length, int _i_security_mode, wifi_mode_t _mode)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start, mode: %d\n", __func__, _mode);

    wifi_config_t wifi_config;
    int password_len;
    void *password;
    int ssid_len;
    void *ssid;

    /* Check WiFi mode */
    if (_mode == WIFI_MODE_STA)
    {
        /* Initialize WiFi configuration */
        memset(&wifi_config.sta.ssid, 0x00, sizeof(wifi_config.sta.ssid));
        memset(&wifi_config.sta.password, 0x00, sizeof(wifi_config.sta.password));
        wifi_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
        wifi_config.sta.bssid_set = false;
        memset(&wifi_config.sta.bssid, 0x00, sizeof(wifi_config.sta.bssid));
        wifi_config.sta.channel = 0;
        wifi_config.sta.listen_interval = 0;
        wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
        wifi_config.sta.threshold.rssi = WIFI_MIN_RSSI_THRESHOLD;
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
        wifi_config.sta.pmf_cfg.capable = false;
        wifi_config.sta.pmf_cfg.required = false;

        /* Get specific fields */
        ssid = (void *)wifi_config.sta.ssid;
        ssid_len = sizeof(wifi_config.sta.ssid);
        password = (void *)wifi_config.sta.password;
        password_len = sizeof(wifi_config.sta.password);
    }
    else if (_mode == WIFI_MODE_AP || _mode == WIFI_MODE_APSTA)
    {
        /* Convert security mode */
        wifi_auth_mode_t authmode = convert_llnet_to_esp32_security_mode_f(_i_security_mode);

        if (0 > authmode)
        {
            return false;
        }

        /* Initialize WiFi configuration */
        memset(&wifi_config.ap.ssid, 0x00, sizeof(wifi_config.ap.ssid));
        memset(&wifi_config.ap.password, 0x00, sizeof(wifi_config.ap.password));
        wifi_config.ap.ssid_len = 0;
        wifi_config.ap.channel = 0;
        wifi_config.ap.authmode = authmode;
        wifi_config.ap.ssid_hidden = 0;
        wifi_config.ap.max_connection = WIFI_SOFT_AP_MAX_CONNECTIONS;
        wifi_config.ap.beacon_interval = WIFI_SOFT_AP_BEACON_INTERVAL;

        /* Get specific fields */
        ssid = (void *)wifi_config.ap.ssid;
        ssid_len = sizeof(wifi_config.ap.ssid);
        password = (void *)wifi_config.ap.password;
        password_len = sizeof(wifi_config.ap.password);
    }
    else
    {
        WIFI_ESP32_ERROR_TRACE("Unsupported mode parameter = %d\n", _mode);
        return false;
    }

    /* Consistency check of the SSID length */
    if (strlen(_pc_ssid) > ssid_len)
    {
        WIFI_ESP32_ERROR_TRACE("SSID length is too long, received %d bytes, required max %d bytes\n", strlen(_pc_ssid), ssid_len);
        return false;
    }

    /* Consistency check of the passphrase length */
    if (strlen(_pc_passphrase) > password_len)
    {
        WIFI_ESP32_ERROR_TRACE("Password length is too long, received %d bytes, required max %d bytes\n", strlen(_pc_passphrase), password_len);
        return false;
    }

    /* Fill connection details */
    memcpy(ssid, _pc_ssid, strlen(_pc_ssid));
    memcpy(password, _pc_passphrase, strlen(_pc_passphrase));

    /* Check the requested mode */
    if (_mode == WIFI_MODE_STA)
    {
        /* Disconnect if needed */
        if (is_wifi_sta_connected())
        {
            /* Set the current connection status */
            sta_mode.state = WIFI_ESP32_MODE_DISCONNECTING;

            /* Disconnect from AP */
            if (!disconnect_f())
            {
                return false;
            }
        }
    }

    /* Check the requested mode */
    if (_mode == WIFI_MODE_STA)
    {
        /* Set the current connection status */
        sta_mode.state = WIFI_ESP32_MODE_CONNECTING;
        /* Start WiFi module if needed */
        if (!start_wifi_f(_mode, &wifi_config))
        {
            sta_mode.state = WIFI_ESP32_MODE_IDLE;
            return false;
        }

        /* Connect to AP */
        if (!connect_f())
        {
            sta_mode.state = WIFI_ESP32_MODE_IDLE;
            return false;
        }

        /* Set the current connection status */
        sta_mode.state = WIFI_ESP32_MODE_CONNECTED;
    }
    else
    {
        /* Set the current connection status */
        ap_mode.state = WIFI_ESP32_MODE_CONNECTING;

        /* Start WiFi module if needed */
        if (!start_wifi_f(_mode, &wifi_config))
        {
            ap_mode.state = WIFI_ESP32_MODE_IDLE;
            return false;
        }

        /* Set the current connection status */
        ap_mode.state = WIFI_ESP32_MODE_CONNECTED;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

static bool leave_ap_or_softap_f(wifi_mode_t mode)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    /* Check the requested mode */
    if (mode == WIFI_MODE_STA)
    {
        /* Disconnect if needed */
        if (is_wifi_sta_connected())
        {
            /* Set the current connection status */
            sta_mode.state = WIFI_ESP32_MODE_DISCONNECTING;

            /* Disconnect from AP */
            if (!disconnect_f())
            {
                sta_mode.state = WIFI_ESP32_MODE_IDLE;
                return false;
            }

            /* Set the current connection status */
            sta_mode.state = WIFI_ESP32_MODE_IDLE;
        }
    }
    else if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA)
    {
        /* Set the current connection status */
        ap_mode.state = WIFI_ESP32_MODE_DISCONNECTING;

        /* Set STA WiFi mode */
        esp_err_t status = esp_wifi_set_mode(WIFI_MODE_STA);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Set the WiFi operating mode, (err=%x)\n", status);
            return false;
        }

        /* Set the current connection status */
        ap_mode.state = WIFI_ESP32_MODE_IDLE;
    }
    else
    {
        WIFI_ESP32_ERROR_TRACE("Unsupported mode parameter = %d\n", mode);
        return false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

static bool is_wifi_initialized(void)
{
    return (esp32_driver_state_b);
}

static bool is_wifi_sta_started(void)
{
    return (sta_mode.state & (WIFI_ESP32_MODE_IDLE | WIFI_ESP32_MODE_CONNECTING | WIFI_ESP32_MODE_CONNECTED | WIFI_ESP32_MODE_DISCONNECTING));
}

static bool is_wifi_ap_started(void)
{
    return ap_mode.state & (WIFI_ESP32_MODE_IDLE | WIFI_ESP32_MODE_CONNECTING | WIFI_ESP32_MODE_CONNECTED | WIFI_ESP32_MODE_DISCONNECTING);
}

static bool is_wifi_sta_connected(void)
{
    return (sta_mode.state & WIFI_ESP32_MODE_CONNECTED);
}

static void filter_duplicated_ap_records(void)
{
    bool *map = calloc(g_available_ap_count, sizeof(bool)); 
    uint16_t filtered = 0;

    /*
     * In case of router repeaters, we can have the same SSID but with different BSSID in the list.
     * In this case, we have to filter out identical Access Points and keep only the one with the highest RSSI.
     *
     * The input list is sorted by RSSI as configured during WiFi initialization.
     */
    for (uint16_t from_index = 0; from_index < g_available_ap_count; from_index++)
    {
        /* For every record, go through the rest of the list. */
        for (uint16_t to_index = (from_index + 1); to_index < g_available_ap_count; to_index++)
        {
            /* If the current item is not filtered already. */
            if (map[to_index] == false)
            {
                /* If the SSID matches, keep the reference and filter out the item with smaller RSSI. */
                if (strcmp((char *)gpst_available_ap_list[from_index].ssid, (char *)gpst_available_ap_list[to_index].ssid) == 0)
                {
                    WIFI_ESP32_INFO_TRACE("Filter out Access Point[SSID=%s, BSSID=%02x:%02x:%02x:%02x:%02x:%02x, RSSI=%d] "
                                          "in favor of Access Point[SSID=%s, BSSID=%02x:%02x:%02x:%02x:%02x:%02x, RSSI=%d]\n",
                                          gpst_available_ap_list[to_index].ssid, gpst_available_ap_list[to_index].bssid[0],
                                          gpst_available_ap_list[to_index].bssid[1], gpst_available_ap_list[to_index].bssid[2],
                                          gpst_available_ap_list[to_index].bssid[3], gpst_available_ap_list[to_index].bssid[4],
                                          gpst_available_ap_list[to_index].bssid[5], gpst_available_ap_list[to_index].rssi,
                                          gpst_available_ap_list[from_index].ssid, gpst_available_ap_list[from_index].bssid[0],
                                          gpst_available_ap_list[from_index].bssid[1], gpst_available_ap_list[from_index].bssid[2],
                                          gpst_available_ap_list[from_index].bssid[3], gpst_available_ap_list[from_index].bssid[4],
                                          gpst_available_ap_list[from_index].bssid[5], gpst_available_ap_list[from_index].rssi);
                    map[to_index] = true;
                    filtered++;
                }
            }
        }
    }

    /* Re-generate the records if we have at least one filtered Access Point. */
    if (filtered > 0)
    {
        wifi_ap_record_t *records = calloc(g_available_ap_count - filtered, sizeof(wifi_ap_record_t));
        uint16_t to_index = 0;

        for (uint16_t from_index = 0; from_index < g_available_ap_count; from_index++)
        {
            if (map[from_index] == false)
            {
                memcpy((void *)&records[to_index], (void *)&gpst_available_ap_list[from_index], sizeof(wifi_ap_record_t));
                to_index++;
            }
        }

        free(gpst_available_ap_list);
        gpst_available_ap_list = records;
        g_available_ap_count -= filtered;
    }

    free(map);
}

static bool apply_configuration_f(esp_netif_t *netif, bool is_static, ip4_addr_t ip, ip4_addr_t netmask, ip4_addr_t gw)
{
    esp_err_t status;

    WIFI_ESP32_ASSERT(is_wifi_initialized());

    /* Take entire configuration from network interface and apply it */
    if (is_static)
    {
        esp_netif_ip_info_t static_ip;
        esp_netif_dns_info_t dns_main;

        /* Stop the DHCP Client */
        status = esp_netif_dhcpc_stop(netif);
        if ((status != ESP_OK) && (status != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED))
        {
            WIFI_ESP32_ERROR_TRACE("Stop DHCP client, (err=%x)\n", status);
            return false;
        }

        /* get DNS info */
        status = esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_main);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Get network interface DNS information, (err=%x)\n", status);
            return false;
        }

        /* Set the static IP configuration */
        memcpy((void *)&static_ip.ip, (void *)&ip.addr, sizeof(static_ip.ip));
        memcpy((void *)&static_ip.netmask, (void *)&netmask.addr, sizeof(static_ip.netmask));
        memcpy((void *)&static_ip.gw, (void *)&gw.addr, sizeof(static_ip.gw));
        status = esp_netif_set_ip_info(netif, &static_ip);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Set network interface's IP address information , (err=%x)\n", status);
            return false;
        }

        /* Check if the TCP/IP stack supports at least 1 DNS Server */
        if (DNS_MAX_SERVERS > 0)
        {
            /* Update the DNS Server */
            status = esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_main);
            if (status != ESP_OK)
            {
                WIFI_ESP32_ERROR_TRACE("Set network interface DNS information, (err=%x)\n", status);
                return false;
            }
        }
    }
    else
    {
        /* The DHCP Client is started by default when the interface is started */
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////Public Functions///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool WIFI_ESP32_initialize_f(void)
{
    if (gst_wifi_event_group != NULL)
    {
        /* Initialization ongoing, wait for it to be finished. */
        EventBits_t uxBits = xEventGroupWaitBits(gst_wifi_event_group, INITIALIZED_BIT, pdFALSE, pdTRUE, INITIALIZED_TIMEOUT_MS / portTICK_RATE_MS);
        if (!(uxBits & INITIALIZED_BIT))
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the initialized event ended in a timeout\n");
            return false;
        }
        else
        {
            return true;
        }
    }

    /* Create the WiFi event group */
    gst_wifi_event_group = xEventGroupCreate();

    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    esp_event_handler_instance_t wifi_instance;
    esp_event_handler_instance_t ip_instance;
    esp_err_t status;

    /* STA and AP modes are not started */
    sta_mode.state = WIFI_ESP32_MODE_NOT_STARTED;
    ap_mode.state = WIFI_ESP32_MODE_NOT_STARTED;
    
    /*
     * Initializing the TCP/IP stack, initializing the WiFi module and starting the WiFi module must be done here
     * and not in the "start" function, in order that LwIP creates the underlying netif already at this point.
     *
     * Otherwise, if the LwIP CCO makes any socket access before the underlying netif is created, this would lead to errors.
     */

    /* Initialize the TCP/IP stack */
    status = esp_netif_init();
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Initialize the underlying TCP/IP stack , (err=%x)\n", status);
        return false;
    }

    /* Create the event loop */
    status = esp_event_loop_create_default();
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Create default event loop , (err=%x)\n", status);
        return false;
    }

    /* Create STA netif */
    sta_netif = esp_netif_create_default_wifi_sta();
    if (sta_netif == NULL)
    {
        WIFI_ESP32_ERROR_TRACE("Creates default WIFI STA, (err=%x)\n", status);
        return false;
    }

    /* Create AP netif */
    ap_netif = esp_netif_create_default_wifi_ap();
    if (ap_netif == NULL)
    {
        WIFI_ESP32_ERROR_TRACE("Creates default WIFI AP, (err=%x)\n", status);
        return false;
    }

    /* Initialize WiFi module */
    if (!initialize_wifi_f())
    {
        return false;
    }

    /* Register event handler for WiFi events */
    status = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler_f, NULL, &wifi_instance);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Register the WiFi instance of event handler to the default loop, (err=%x)\n", status);
        return false;
    }

    /* Register event handler for IP events */
    status = esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler_f, NULL, &ip_instance);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Register the IP instance of event handler to the default loop, (err=%x)\n", status);
        return false;
    }

    /* Start the WiFi module in default station configuration (need to do it at initialization otherwise LwIP will have no registered netif) */
    if (!start_wifi_f(WIFI_MODE_STA, (wifi_config_t *)&g_wifi_sta_default_config))
    {
        return false;
    }

    /* Change adapter hostname for the station interface */
    char hostname[] = "wrover";
    WIFI_ESP32_INFO_TRACE("WiFi station interface adapter name set to %s\n", hostname);
    status = esp_netif_set_hostname(sta_netif, hostname);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Set the hostname of an interface, (err=%x)\n", status);
        return false;
    }

    /* Set the current connection status (STA mode is started) */
    sta_mode.state = WIFI_ESP32_MODE_IDLE;

    /* Initialization complete flag, useful if there's another task that waits for it */
    xEventGroupSetBits(gst_wifi_event_group, INITIALIZED_BIT);

    /* driver is configured */
    esp32_driver_state_b = true;

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_sta_apply_configuration_f(bool is_static, ip4_addr_t ip, ip4_addr_t netmask, ip4_addr_t gw)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);
    bool status = apply_configuration_f(sta_netif, is_static, ip, netmask, gw);
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return status;
}

bool WIFI_ESP32_ap_apply_configuration_f(bool is_static, ip4_addr_t ip, ip4_addr_t netmask, ip4_addr_t gw)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);
    bool status = apply_configuration_f(ap_netif, is_static, ip, netmask, gw);
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return status;
}

bool WIFI_ESP32_sta_start_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    /* Start WiFi module */
    if (!start_wifi_f(WIFI_MODE_STA, (wifi_config_t *)&g_wifi_sta_default_config))
    {
        return false;
    }

    /* Set the current connection status */
    sta_mode.state=WIFI_ESP32_MODE_IDLE;

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_ap_start_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    /* Start the WiFi module in default configuration */
    if (!start_wifi_f(WIFI_MODE_AP, (wifi_config_t *)&g_wifi_ap_default_config))
    {
        return false;
    }

    /* Set the current connection status */
    ap_mode.state=WIFI_ESP32_MODE_IDLE;

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_sta_stop_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);    
    WIFI_ESP32_ASSERT(is_wifi_initialized());

    /* Stop WiFi module */
    if (!stop_wifi_f(WIFI_MODE_STA))
    {
        return false;
    }

    /* Module must be started again for further usage */
    sta_mode.state=WIFI_ESP32_MODE_NOT_STARTED;
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_ap_stop_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    WIFI_ESP32_ASSERT(is_wifi_initialized());

    /* Stop WiFi module */
    if (!stop_wifi_f(WIFI_MODE_AP))
    {
        return false;
    }

    /* Module must be started again for further usage */
    ap_mode.state=WIFI_ESP32_MODE_NOT_STARTED;

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_sta_netif_is_started_f(unsigned char *_puc_netif_started)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);
    WIFI_ESP32_ASSERT(_puc_netif_started != NULL);

    *_puc_netif_started = is_wifi_sta_started();

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_ap_netif_is_started_f(unsigned char *_puc_netif_started)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    WIFI_ESP32_ASSERT(_puc_netif_started != NULL);

    *_puc_netif_started = is_wifi_ap_started();

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_join_f(const char *_pc_ssid, int _i_ssid_length, const char *_pc_passphrase, int _i_passphrase_length, int _i_security_mode)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pc_ssid != NULL);
    WIFI_ESP32_ASSERT(_i_ssid_length > 0);
    WIFI_ESP32_ASSERT(_pc_passphrase != NULL);
    WIFI_ESP32_ASSERT(_i_passphrase_length > 0);
    WIFI_ESP32_ASSERT(_i_security_mode >= 0);


    bool status = join_ap_or_softap_f(_pc_ssid, _i_ssid_length, _pc_passphrase, _i_passphrase_length, _i_security_mode, WIFI_MODE_STA);

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return status;
}

bool WIFI_ESP32_leave_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    WIFI_ESP32_ASSERT(is_wifi_initialized());

    bool status = leave_ap_or_softap_f(WIFI_MODE_STA); 

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return status;
}

bool WIFI_ESP32_get_bssid_f(unsigned char *_puc_bssid, int _i_bssid_length)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);
    
    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_puc_bssid != NULL);
    WIFI_ESP32_ASSERT(_i_bssid_length >= sizeof(ap_info.bssid));

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get information of AP which the ESP32 station is associated with (err=%x)\n", status);
        return false;
    }

    /* Store BSSID information */
    memcpy((void *)_puc_bssid, (void *)ap_info.bssid, (size_t)_i_bssid_length);
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_get_channel_f(short *_ps_channel)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_ps_channel != NULL);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get information of AP which the ESP32 station is associated with (err=%x)\n", status);
        return false;
    }

    /* Store channel information */
    *_ps_channel = ap_info.primary;

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_get_rssi_f(float *_pf_rssi, int _i_rssi_length)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pf_rssi != NULL);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get information of AP which the ESP32 station is associated with (err=%x)\n", status);
        return false;
    }

    /* Store RSSI information */
    *_pf_rssi = ap_info.rssi;

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_get_security_mode_f(short *_ps_mode)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    wifi_ap_record_t ap_info;
    int security_mode;
    esp_err_t status;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_ps_mode != NULL);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get information of AP which the ESP32 station is associated with (err=%x)\n", status);
        return false;
    }

    /* Translate the security mode */
    security_mode = convert_esp32_to_llnet_security_mode_f(ap_info.authmode, ap_info.group_cipher);
    if (0 > security_mode)
    {
        return false;
    }

    *_ps_mode = (short)(security_mode & 0xFFFF);
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_get_ssid_f(char *_pc_ssid, int _i_ssid_length)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pc_ssid != NULL);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get information of AP which the ESP32 station is associated with (err=%x)\n", status);
        return false;
    }

    /* Store SSID information */
    memcpy((void *)_pc_ssid, (void *)ap_info.ssid, (size_t)_i_ssid_length);
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_get_wps_modes_f(short *_ps_wps_modes)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_ps_wps_modes != NULL);

    /* Not implemented */
    *_ps_wps_modes = WPS_MODE_NONE;
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_enable_softap_f(const char *_pc_ssid, int _i_ssid_length, const char *_pc_passphrase, int _i_passphrase_length)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pc_ssid != NULL);
    WIFI_ESP32_ASSERT(_i_ssid_length > 0);
    WIFI_ESP32_ASSERT(_pc_passphrase != NULL);
    WIFI_ESP32_ASSERT(_i_passphrase_length > 0);

    bool status = join_ap_or_softap_f(_pc_ssid, _i_ssid_length, _pc_passphrase, _i_passphrase_length, strlen(_pc_passphrase) == 0 ? SECURITY_MODE_OPEN : SECURITY_MODE_WPA2, WIFI_MODE_APSTA);
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return status;
}

bool WIFI_ESP32_set_name_softap_f(const char *_pc_name, int _i_name_length)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pc_name != NULL);

    /* Change adapter hostname for the access point interface */
    WIFI_ESP32_INFO_TRACE("WiFi access point interface adapter name set to %s\n", _pc_name);
    esp_err_t status = esp_netif_set_hostname(ap_netif, _pc_name);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Set the hostname of an interface, (err=%x)\n", status);
        return false;
    }
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_disable_softap_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);    
    WIFI_ESP32_ASSERT(is_wifi_initialized());

    bool status = leave_ap_or_softap_f(WIFI_MODE_APSTA);

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return status;
}

bool WIFI_ESP32_get_ap_count_f(short *const _ps_ap_count)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 0,
        .scan_time.active.max = 0};
    EventBits_t uxBits;
    uint16_t num_scan;
    esp_err_t status;
    bool scan_succeeded = false;
    int8_t retry_scan = 1;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_ps_ap_count != NULL);

    /* Start a non-blocking scan. Warning: scanning is not possible in SoftAP (WIFI_MODE_AP) mode */
    WIFI_ESP32_INFO_TRACE("Scanning AP...\n");

    do
    {
        status = esp_wifi_scan_start(&scan_config, false);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Scan all available APs (err= %x)\n", status);
            return false;
        }

        /* Wait for the scan event */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, SCAN_BIT, pdTRUE, pdTRUE, SCAN_TIMEOUT_MS / portTICK_RATE_MS);
        if (uxBits & SCAN_BIT)
        {
            scan_succeeded = true;
        }
        else
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the scan event ended in a timeout\n");

            if (retry_scan > 0)
            {
                /* Workaround for ESP-IDF bug: WiFi scan impossible after a failed attempt to reconnect to a hotspot. Disconnect from AP */

                /* Set the current connection status */
                sta_mode.state=WIFI_ESP32_MODE_DISCONNECTING;

                /* Disconnect from AP */
                disconnect_f();

                /* Set the current connection status */
                sta_mode.state=WIFI_ESP32_MODE_IDLE;
            }
            else
            {
                return false;
            }
        }
    } while (!scan_succeeded && (retry_scan-- > 0));

    /* Get the number of scanned AP */
    status = esp_wifi_scan_get_ap_num(&num_scan);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get number of APs found in last scan, (err=%x)\n", status);
        return false;
    }

    /* Free the previously saved AP list */
    if (gpst_available_ap_list != NULL)
    {
        free(gpst_available_ap_list);
        gpst_available_ap_list = NULL;
        g_available_ap_count = 0;
    }

    /* If we have at least one scanned AP */
    if (num_scan > 0)
    {
        /* Allocate memory for saving the AP list */
        g_available_ap_count = num_scan;
        gpst_available_ap_list = calloc(g_available_ap_count, sizeof(wifi_ap_record_t));
        if (gpst_available_ap_list == NULL)
        {
            WIFI_ESP32_ERROR_TRACE("Cannot allocate %d bytes of memory for the entire AP list found in last scan\n", g_available_ap_count * sizeof(wifi_ap_record_t));
            return false;
        }

        /* Get the AP records */
        status = esp_wifi_scan_get_ap_records(&g_available_ap_count, gpst_available_ap_list);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Get AP list found in last scan (err= %x)\n", status);
            return false;
        }

        /* Filter duplicated AP's */
        filter_duplicated_ap_records();
    }
    else
    {
        WIFI_ESP32_WARNING_TRACE("Last scan resulted in 0 AP\n");
    }

    *_ps_ap_count = (short)(g_available_ap_count & 0xFFFF);

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return true;
}

bool WIFI_ESP32_get_ap_ssid_f(unsigned int _ui_index, unsigned char *_puc_ssid, int _i_ssid_length)
{
    WIFI_ESP32_DEBUG_TRACE("Start: (%s)\n", __func__);

    bool b_return;
    unsigned int ui_copy_length;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_puc_ssid != NULL);

    if ((_ui_index < g_available_ap_count) && (_i_ssid_length > 0))
    {
        b_return = true;

        int ap_ssid_length = strlen((char *)gpst_available_ap_list[_ui_index].ssid);
        if (_i_ssid_length >= ap_ssid_length + 1) // buffer is large enough
        {
            ui_copy_length = ap_ssid_length;
        }
        else // buffer is not big enough
        {
            ui_copy_length = _i_ssid_length - 1;
        }

        memcpy(_puc_ssid, gpst_available_ap_list[_ui_index].ssid, ui_copy_length);
        _puc_ssid[ui_copy_length] = 0;
    }
    else
    {
        b_return = false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return b_return;
}

bool WIFI_ESP32_get_ap_bssid_f(unsigned int _ui_index, unsigned char *_puc_bssid, int _i_bssid_length)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    bool b_return;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_puc_bssid != NULL);

    if ((_ui_index < g_available_ap_count) && (_i_bssid_length > 0))
    {
        int copy_length;

        b_return = true;

        if ((unsigned int)_i_bssid_length < sizeof(gpst_available_ap_list[_ui_index].bssid))
        {
            copy_length = _i_bssid_length;
        }
        else
        {
            copy_length = sizeof(gpst_available_ap_list[_ui_index].bssid);
        }

        strncpy((char *)_puc_bssid, (char *)gpst_available_ap_list[_ui_index].bssid, (size_t)copy_length);
    }
    else
    {
        b_return = false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return b_return;
}

bool WIFI_ESP32_get_ap_channel_f(unsigned int _ui_index, int *_pi_channel)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);

    bool b_return;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pi_channel != NULL);

    if (_ui_index < g_available_ap_count)
    {
        b_return = true;
        *_pi_channel = gpst_available_ap_list[_ui_index].primary;
        WIFI_ESP32_DEBUG_TRACE("(%s) AP channel: %d; channel=%d\n", __func__, b_return, *_pi_channel);
    }
    else
    {
        b_return = false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return b_return;
}

bool WIFI_ESP32_get_ap_rssi_f(unsigned int _ui_index, float *_pf_rssi, int _i_rssi_length)
{

    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);    
    
    bool b_return;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pf_rssi != NULL);

    if ((_ui_index < g_available_ap_count) && (_i_rssi_length > 0))
    {
        b_return = true;
        *_pf_rssi = gpst_available_ap_list[_ui_index].rssi;
        WIFI_ESP32_DEBUG_TRACE("(%s) AP RSSI: %d; rssi=%f\n", __func__, b_return, *_pf_rssi);
    }
    else
    {
        b_return = false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);    
    return b_return;
}

bool WIFI_ESP32_get_ap_security_mode_f(unsigned int _ui_index, int *_pi_mode)
{
    WIFI_ESP32_DEBUG_TRACE("(%s) start, index = %u\n", __func__, _ui_index);

    bool b_return;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pi_mode != NULL);
    
    if (_ui_index < g_available_ap_count)
    {
        int security_mode = convert_esp32_to_llnet_security_mode_f(gpst_available_ap_list[_ui_index].authmode, gpst_available_ap_list[_ui_index].group_cipher);
        b_return = 0 <= security_mode;
        if (false != b_return)
        {
            *_pi_mode = security_mode;
        }
        WIFI_ESP32_DEBUG_TRACE("(%s) AP security: %d; mode=%d\n", __func__, b_return, *_pi_mode);
    }
    else
    {
        b_return = false;
    }
    
    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);
    return b_return;
}

bool WIFI_ESP32_get_ap_wps_modes_f(unsigned int _ui_index, int *_pi_wps_cap)
{

    WIFI_ESP32_DEBUG_TRACE("(%s) start\n", __func__);    
    bool b_return;

    WIFI_ESP32_ASSERT(is_wifi_initialized());
    WIFI_ESP32_ASSERT(_pi_wps_cap != NULL);

    if (_ui_index < g_available_ap_count)
    {
        b_return = true;
        *_pi_wps_cap = WPS_MODE_NONE;
        WIFI_ESP32_DEBUG_TRACE("(%s) AP wps mode true. \n", __func__);
    }
    else
    {
        b_return = false;
    }

    WIFI_ESP32_DEBUG_TRACE("(%s) end\n", __func__);    
    return b_return;
}

bool WIFI_ESP32_get_client_state_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    if (sta_mode.state == WIFI_ESP32_MODE_CONNECTED)
    {
        WIFI_ESP32_DEBUG_TRACE("(%s) STA: connected. \n", __func__);
        return true;
    }
    else
    {
        WIFI_ESP32_DEBUG_TRACE("(%s) STA: disconnected \n", __func__);
        return false;
    }
}

bool WIFI_ESP32_get_access_point_state_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    if (ap_mode.state == WIFI_ESP32_MODE_CONNECTED)
    {
        WIFI_ESP32_DEBUG_TRACE("(%s) AP: connected \n", __func__);
        return true;
    }
    else
    {
        WIFI_ESP32_DEBUG_TRACE("(%s) AP: disconnected \n", __func__);
        return false;
    }
}
