/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Toolchain includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ESP32 includes */
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "lwip/inet.h"
#include "lwip/dns.h"

/* External includes */
#include "LLECOM_WIFI_impl.h"
#include "LLNET_configuration.h"

/* Internal includes */
#include "WIFI_ESP32_driver.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////Macro definitions//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**@brief Define this value to enable assert */
#undef ENABLE_ASSERT

/**@brief Assert macro */
#ifdef ENABLE_ASSERT
#define WIFI_ESP32_ASSERT(x) do { if (!(x)) { WIFI_ESP32_ASSERT_TRACE("%s, %d\n", __FILE__, __LINE__); while(1); } } while (0)
#else
#define WIFI_ESP32_ASSERT(x) do { if (!(x)) { WIFI_ESP32_ASSERT_TRACE("%s, %d\n", __FILE__, __LINE__); } } while (0)
#endif

/**@brief Log priority levels */
#define WIFI_ESP32_LOG_DEBUG      0
#define WIFI_ESP32_LOG_INFO       1
#define WIFI_ESP32_LOG_WARNING    2
#define WIFI_ESP32_LOG_ERROR      3
#define WIFI_ESP32_LOG_ASSERT     4
#define WIFI_ESP32_LOG_NONE       5

/**@brief Current log level */
#define WIFI_ESP32_LOG_LEVEL WIFI_ESP32_LOG_WARNING

#ifndef WIFI_ESP32_LOG_LEVEL
    #error "WIFI_ESP32_LOG_LEVEL must be defined"
#endif

/**@brief Debug logger */
#if (WIFI_ESP32_LOG_DEBUG >= WIFI_ESP32_LOG_LEVEL)
    #define WIFI_ESP32_DEBUG_TRACE        printf("[ESP32 Wifi Driver][DEBUG] ");printf
#else
    #define WIFI_ESP32_DEBUG_TRACE(...)   ((void) 0)
#endif

/**@brief Info logger */
#if (WIFI_ESP32_LOG_INFO >= WIFI_ESP32_LOG_LEVEL)
    #define WIFI_ESP32_INFO_TRACE         printf("[ESP32 Wifi Driver][INFO] ");printf
#else
    #define WIFI_ESP32_INFO_TRACE(...)    ((void) 0)
#endif

/**@brief Warning logger */
#if (WIFI_ESP32_LOG_WARNING >= WIFI_ESP32_LOG_LEVEL)
    #define WIFI_ESP32_WARNING_TRACE      printf("[ESP32 Wifi Driver][WARNING] ");printf
#else
    #define WIFI_ESP32_WARNING_TRACE(...) ((void) 0)
#endif

/**@brief Error logger */
#if (WIFI_ESP32_LOG_ERROR >= WIFI_ESP32_LOG_LEVEL)
    #define WIFI_ESP32_ERROR_TRACE        printf("[ESP32 Wifi Driver][ERROR] ");printf
#else
    #define WIFI_ESP32_ERROR_TRACE(...)   ((void) 0)
#endif

/**@brief Assert logger */
#if (WIFI_ESP32_LOG_ASSERT >= WIFI_ESP32_LOG_LEVEL)
    #define WIFI_ESP32_ASSERT_TRACE       printf("[ESP32 Wifi Driver][ASSERT] ");printf
#else
    #define WIFI_ESP32_ASSERT_TRACE(...)  ((void) 0)
#endif

/** @brief Max size of the AP scan buffers */
#define WIFI_ESP32_MAX_SCAN_BUFFERS         12

/** @brief Event bits */
#define CONNECTED_BIT                     BIT0
#define DISCONNECTED_BIT                  BIT1
#define START_BIT                         BIT2
#define SCAN_BIT                          BIT3
#define STOP_BIT                          BIT4

/** @brief Timeouts */
#define DISCONNECT_TIMEOUT_MS             5000
#define CONNECT_TIMEOUT_MS                5000
#define START_TIMEOUT_MS                  5000
#define STOP_TIMEOUT_MS                   5000
#define SCAN_TIMEOUT_MS                  10000

/** @brief Minimum RSSI threshold to scan for */
#define WIFI_MIN_RSSI_THRESHOLD           -127

/** @brief Max connections that can be made to the Soft AP */
#define WIFI_SOFT_AP_MAX_CONNECTIONS         4

/** @brief Beacon interval for the Soft AP */
#define WIFI_SOFT_AP_BEACON_INTERVAL       100

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////Type definitions//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief ESP32 connection mode */
typedef enum {
    WIFI_ESP32_NOT_INITIALIZED   = 0x00,        /**< Module not initialized */
    WIFI_ESP32_STA_IDLE          = (0x01 << 0), /**< Station mode, idle */
    WIFI_ESP32_STA_CONNECTING    = (0x01 << 1), /**< Station mode, connecting to an AP */
    WIFI_ESP32_STA_CONNECTED     = (0x01 << 2), /**< Station mode, connected to an AP */
    WIFI_ESP32_STA_DISCONNECTING = (0x01 << 3), /**< Station mode, disconnecting from an AP */
    WIFI_ESP32_AP_IDLE           = (0x01 << 4), /**< Access Point mode, idle */
    WIFI_ESP32_AP_CONNECTING     = (0x01 << 5), /**< Access Point mode, mounting a SoftAP */
    WIFI_ESP32_AP_CONNECTED      = (0x01 << 6), /**< Access Point mode, mounted a SoftAP */
    WIFI_ESP32_AP_DISCONNECTING  = (0x01 << 7), /**< Access Point mode, unmounting a SoftAP */
} WIFI_ESP32_connection_state_t;

/** @brief ESP32 driver configuration structure */
typedef struct
{
    WIFI_ESP32_connection_state_t connect_state;
} WIFI_ESP32_driver_t;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////Global Variables//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief Global ESP32 driver configuration */
static WIFI_ESP32_driver_t g_esp32_driver;

/** @brief List of available access points */
static wifi_ap_record_t gpst_available_ap_list[WIFI_ESP32_MAX_SCAN_BUFFERS];

/** @brief Event group for wifi management */
static EventGroupHandle_t gst_wifi_event_group;

/** @brief Default wifi configuration */
static const wifi_config_t g_wifi_default_config = {
        .sta = {
                .ssid = {0},
                .password = {0},
                .scan_method = WIFI_ALL_CHANNEL_SCAN,
                .bssid_set = 0,
                .bssid = {0},
                .channel = 0,
                .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
                .threshold.rssi = WIFI_MIN_RSSI_THRESHOLD,
                .threshold.authmode = WIFI_AUTH_OPEN
        }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////External Variables/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Variable used to notify the soft DNS foundation library that DNS servers list has changed */
extern uint8_t dns_servers_list_updated;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////Functions Prototypes////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief this function translates the security mode from LLNET mode to ESP32 mode
 *
 * @param[in]  _i_llnet_security_mode input security mode
 * @param[out] _esp32_security_type output security type
 * @param[out] _esp32_cipher_type output cipher type
 *
 * @return true if the security mode was converted, false otherwise
 */
static bool convert_llnet_to_esp32_security_mode_f(int _i_llnet_security_mode,
        wifi_auth_mode_t * _esp32_security_type,
        wifi_cipher_type_t * _esp32_cipher_type);

/**
 * @brief this function translates the security mode from ESP32 mode to LLNET mode
 *
 * @param[out] _pi_llnet_security_mode output security mode
 * @param[in]  _esp32_security_type input security type
 * @param[in]  _esp32_cipher_type input cipher type
 *
 * @return true if the security mode was converted, false otherwise
 */
static bool convert_esp32_to_llnet_security_mode_f(int * _pi_llnet_security_mode,
        wifi_auth_mode_t _esp32_security_type,
        wifi_cipher_type_t _esp32_cipher_type);

/**
 * @brief this function handles the received events from the wifi stack
 *
 * @param[in] ctx event context
 * @param[in] event event content
 *
 * @return esp error code
 */
static esp_err_t event_handler_f(void * ctx, system_event_t * event);

/**
 * @brief this function initializes the wifi module
 *
 * @return true if success else false
 */
static bool initialize_wifi_f(void);

/**
 * @brief this function starts the wifi module
 *
 * @param[in] mode AP or SoftAP
 * @param[in] wifi_config AP or SoftAP configuration
 * @param[in] force if the start has to be forced, otherwise only if the requested mode is different
 *
 * @return true if success else false
 */
static bool start_wifi_f(wifi_mode_t mode, wifi_config_t * wifi_config, bool force);

/**
 * @brief this function stops the wifi module
 *
 * @param[in] mode AP or SoftAP
 * @param[in] force if the start has to be forced, otherwise only if the requested mode is different
 *
 * @return true if success else false
 */
static bool stop_wifi_f(wifi_mode_t mode, bool force);

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
 * @param[in] _pc_ssid ssid of the network
 * @param[in] _i_ssid_length ssid length
 * @param[in] _pc_passphrase password needed for network connection
 * @param[in] _i_passphrase_length password length
 * @param[in] _i_security_mode security mode
 * @param[in] _mode AP or SoftAP
 *
 * @return true if success else false
 */
static bool join_ap_or_softap_f(const char * _pc_ssid, int _i_ssid_length, const char * _pc_passphrase, int _i_passphrase_length,
        int _i_security_mode, wifi_mode_t _mode);

/**
 * @brief this function leaves a WiFi network
 *
 * @param[in] mode AP or SoftAP
 *
 * @return true if success else false
 */
static bool leave_ap_or_softap_f(wifi_mode_t mode);

/**
 * @brief this function sets the current connecting state of the wifi module
 *
 * @param[in] state new state to set
 */
static void set_connecting_state(WIFI_ESP32_connection_state_t state);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////Private Functions//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool convert_llnet_to_esp32_security_mode_f(int _i_llnet_security_mode,
        wifi_auth_mode_t * _esp32_security_type,
        wifi_cipher_type_t * _esp32_cipher_type)
{
    switch (_i_llnet_security_mode)
    {
        case SECURITY_MODE_OPEN:
            *_esp32_security_type = WIFI_AUTH_OPEN;
            *_esp32_cipher_type = WIFI_CIPHER_TYPE_NONE;
        break;

        case SECURITY_MODE_WEP64:
            *_esp32_security_type = WIFI_AUTH_WEP;
            *_esp32_cipher_type = WIFI_CIPHER_TYPE_WEP40;
        break;

        case SECURITY_MODE_WEP128:
            *_esp32_security_type = WIFI_AUTH_WEP;
            *_esp32_cipher_type = WIFI_CIPHER_TYPE_WEP104;
        break;

        case SECURITY_MODE_WPA1:
            *_esp32_security_type = WIFI_AUTH_WPA_PSK;
            *_esp32_cipher_type = WIFI_CIPHER_TYPE_TKIP;
        break;

        case SECURITY_MODE_WPA2:
            *_esp32_security_type = WIFI_AUTH_WPA2_PSK;
            *_esp32_cipher_type = WIFI_CIPHER_TYPE_CCMP;
        break;

        case SECURITY_MODE_MIXED:
            *_esp32_security_type = WIFI_AUTH_WPA_WPA2_PSK;
            *_esp32_cipher_type = WIFI_CIPHER_TYPE_TKIP_CCMP;
        break;

        case SECURITY_MODE_ENTERPRISE_WPA2:
            *_esp32_security_type = WIFI_AUTH_WPA2_ENTERPRISE;
            *_esp32_cipher_type = WIFI_CIPHER_TYPE_CCMP;
        break;

        default:
            /* Not supported security mode */
            WIFI_ESP32_ERROR_TRACE("Security mode not supported (mode = %d)\n", _i_llnet_security_mode);
            return false;
    }

    return true;
}

static bool convert_esp32_to_llnet_security_mode_f(int * _pi_llnet_security_mode,
        wifi_auth_mode_t _esp32_security_type,
        wifi_cipher_type_t _esp32_cipher_type)
{
    switch (_esp32_security_type)
    {
        case WIFI_AUTH_OPEN:
            *_pi_llnet_security_mode = SECURITY_MODE_OPEN;
        break;

        case WIFI_AUTH_WEP:
            switch (_esp32_cipher_type)
            {
                case WIFI_CIPHER_TYPE_WEP40:
                    *_pi_llnet_security_mode = SECURITY_MODE_WEP64;
                    break;
                case WIFI_CIPHER_TYPE_WEP104:
                    *_pi_llnet_security_mode = SECURITY_MODE_WEP128;
                    break;
                case WIFI_CIPHER_TYPE_NONE:
                    /* Workaround to have a valid information */
                    *_pi_llnet_security_mode = SECURITY_MODE_WEP128;
                    break;
                default:
                    /* Not supported security mode */
                    WIFI_ESP32_ERROR_TRACE("Cipher mode not supported (mode = %d)\n", _esp32_cipher_type);
                    return false;
            }
        break;

        case WIFI_AUTH_WPA_PSK:
            *_pi_llnet_security_mode = SECURITY_MODE_WPA1;
        break;

        case WIFI_AUTH_WPA2_PSK:
            *_pi_llnet_security_mode = SECURITY_MODE_WPA2;
        break;

        case WIFI_AUTH_WPA_WPA2_PSK:
            *_pi_llnet_security_mode = SECURITY_MODE_MIXED;
        break;

        case WIFI_AUTH_WPA2_ENTERPRISE:
            *_pi_llnet_security_mode = SECURITY_MODE_ENTERPRISE_WPA2;
        break;

        default:
            /* Not supported security mode */
            WIFI_ESP32_ERROR_TRACE("Security mode not supported (mode = %d)\n", _esp32_security_type);
            return false;
    }

    return true;
}

static esp_err_t event_handler_f(void * ctx, system_event_t * event)
{
    esp_err_t status = ESP_OK;

    switch (event->event_id)
    {
        case SYSTEM_EVENT_SCAN_DONE:
            WIFI_ESP32_INFO_TRACE("Scan finished\n");

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, SCAN_BIT);
            break;

        case SYSTEM_EVENT_STA_START:
        case SYSTEM_EVENT_AP_START:
            WIFI_ESP32_INFO_TRACE("Station/SoftAP started\n");

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, START_BIT);
            break;

        case SYSTEM_EVENT_STA_STOP:
        case SYSTEM_EVENT_AP_STOP:
            WIFI_ESP32_INFO_TRACE("Station/SoftAP stopped\n");

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, STOP_BIT);
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            WIFI_ESP32_INFO_TRACE("Station connected\n");

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, CONNECTED_BIT);
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            WIFI_ESP32_INFO_TRACE("Station disconnected\n");

            /* Unlock any waiting threads */
            xEventGroupSetBits(gst_wifi_event_group, DISCONNECTED_BIT);
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            WIFI_ESP32_INFO_TRACE("Station got IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

            /* DNS servers are also updated, so update this variable to notify the soft DNS foundation library */
            dns_servers_list_updated = 1;
            break;

        case SYSTEM_EVENT_STA_LOST_IP:
            WIFI_ESP32_INFO_TRACE("Station lost IP\n");
            break;

        case SYSTEM_EVENT_AP_STACONNECTED:
            WIFI_ESP32_INFO_TRACE("A station connected to the softAP\n");
            break;

        case SYSTEM_EVENT_AP_STADISCONNECTED:
            WIFI_ESP32_INFO_TRACE("A station disconnected from the softAP\n");
            break;

        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            WIFI_ESP32_INFO_TRACE("SoftAP assigns an IP to a connected station\n");
            break;

        default:
            WIFI_ESP32_WARNING_TRACE("Event %d received, not treated\n", event->event_id);
            break;
    }

    return status;
}

static bool initialize_wifi_f(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t status;

    /* Initialize wifi module */
    status = esp_wifi_init(&cfg);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Initialize wifi failed, (err=%x)\n", status);
        return false;
    }

    /* Set wifi storage mode */
    status = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Set wifi storage mode, (err=%x)\n", status);
        return false;
    }

    return true;
}

static bool start_wifi_f(wifi_mode_t mode, wifi_config_t * wifi_config, bool force)
{
    esp_interface_t wifi_interface;
    EventBits_t uxBits;
    wifi_mode_t _mode;
    esp_err_t status;

    /* Check wifi mode */
    if (mode == WIFI_MODE_STA)
    {
        wifi_interface = ESP_IF_WIFI_STA;
    }
    else if (mode == WIFI_MODE_AP)
    {
        wifi_interface = ESP_IF_WIFI_AP;
    }
    else
    {
        WIFI_ESP32_ERROR_TRACE("Unsupported mode parameter = %d\n", mode);
        return false;
    }

    /* Get current wifi mode */
    status = esp_wifi_get_mode(&_mode);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get station mode failed, (err=%x)\n", status);
        return false;
    }

    /* Change the mode if it is different */
    if (_mode != mode)
    {
        /* Set new station mode */
        status = esp_wifi_set_mode(mode);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Set station mode failed, (err=%x)\n", status);
            return false;
        }
    }

    /* Set requested wifi config */
    status = esp_wifi_set_config(wifi_interface, wifi_config);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Set wifi config failed, (err=%x)\n", status);
        return false;
    }

    /* Start only if the current mode is different than the new mode or if forced to do so */
    if (force || (_mode != mode))
    {
        /* Start wifi module */
        WIFI_ESP32_INFO_TRACE("Starting wifi module...\n");
        status = esp_wifi_start();
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Wifi start failed, (err=%x)\n", status);
            return false;
        }

        /* Wait for the start event */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, START_BIT, pdTRUE, pdTRUE, START_TIMEOUT_MS / portTICK_RATE_MS);
        if (uxBits & START_BIT)
        {
            WIFI_ESP32_INFO_TRACE("Wifi module started\n");
        }
        else
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the start event ended in a timeout\n");
            return false;
        }
    }

    return true;
}

static bool stop_wifi_f(wifi_mode_t mode, bool force)
{
    EventBits_t uxBits;
    wifi_mode_t _mode;
    esp_err_t status;

    /* Check wifi mode */
    if ((mode != WIFI_MODE_STA) && (mode != WIFI_MODE_AP))
    {
        WIFI_ESP32_ERROR_TRACE("Unsupported mode parameter = %d\n", mode);
        return false;
    }

    /* Get current wifi mode */
    status = esp_wifi_get_mode(&_mode);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get station mode failed, (err=%x)\n", status);
        return false;
    }

    /* Stop only if the current mode is different than the new mode or if forced to do so */
    if (force || (_mode != mode))
    {
        /* Stop wifi module */
        WIFI_ESP32_INFO_TRACE("Stopping wifi module...\n");
        status = esp_wifi_stop();
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Stopping wifi failed, (err=%x)\n", status);
            return false;
        }

        /* Wait for the stop event */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, STOP_BIT, pdTRUE, pdTRUE, STOP_TIMEOUT_MS / portTICK_RATE_MS);
        if (uxBits & STOP_BIT)
        {
            WIFI_ESP32_INFO_TRACE("Wifi module stopped\n");
        }
        else
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the stop event ended in a timeout\n");
            return false;
        }
    }

    return true;
}

static bool connect_f(void)
{
    EventBits_t uxBits;
    esp_err_t status;

    /* Connect */
    WIFI_ESP32_INFO_TRACE("Connecting to AP...\n");
    status = esp_wifi_connect();
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Connect failed, (err=%x)\n", status);
        return false;
    }

    /* Wait for the connected event */
    uxBits = xEventGroupWaitBits(gst_wifi_event_group, CONNECTED_BIT, pdFALSE, pdTRUE, CONNECT_TIMEOUT_MS / portTICK_RATE_MS);
    if (uxBits & CONNECTED_BIT)
    {
        WIFI_ESP32_INFO_TRACE("Connected to AP\n");
    }
    else
    {
        WIFI_ESP32_ERROR_TRACE("Waiting for the connect event ended in a timeout\n");
        status = esp_wifi_disconnect();
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Disconnect failed, (err=%x)\n", status);
        }
        return false;
    }

    return true;
}

static bool disconnect_f(void)
{
    EventBits_t uxBits;
    esp_err_t status;

    WIFI_ESP32_INFO_TRACE("Disconnecting from AP...\n");
    status = esp_wifi_disconnect();
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Disconnect failed, (err=%x)\n", status);
        return false;
    }

    /* Wait for the disconnected event */
    uxBits = xEventGroupWaitBits(gst_wifi_event_group, DISCONNECTED_BIT, pdTRUE, pdTRUE, DISCONNECT_TIMEOUT_MS / portTICK_RATE_MS);
    if (uxBits & DISCONNECTED_BIT)
    {
        /* Station disconnected, clear the connected event bit */
        xEventGroupClearBits(gst_wifi_event_group, CONNECTED_BIT);
        WIFI_ESP32_INFO_TRACE("Disconnected from AP\n");
    }
    else
    {
        WIFI_ESP32_ERROR_TRACE("Waiting for the disconnect event ended in a timeout\n");
        return false;
    }

    return true;
}

static bool join_ap_or_softap_f(const char * _pc_ssid, int _i_ssid_length, const char * _pc_passphrase, int _i_passphrase_length,
        int _i_security_mode, wifi_mode_t _mode)
{
    wifi_config_t wifi_config;
    int password_len;
    void * password;
    int ssid_len;
    void * ssid;

    /* Check wifi mode */
    if (_mode == WIFI_MODE_STA)
    {
        /* Initialize wifi config */
        memset(&wifi_config.sta.ssid, 0x00, sizeof(wifi_config.sta.ssid));
        memset(&wifi_config.sta.password, 0x00, sizeof(wifi_config.sta.password));
        wifi_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
        wifi_config.sta.bssid_set = 0;
        memset(&wifi_config.sta.bssid, 0x00, sizeof(wifi_config.sta.bssid));
        wifi_config.sta.channel = 0;
        wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
        wifi_config.sta.threshold.rssi = WIFI_MIN_RSSI_THRESHOLD;
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;

        /* Get specific fields */
        ssid = (void *)wifi_config.sta.ssid;
        ssid_len = sizeof(wifi_config.sta.ssid);
        password = (void *)wifi_config.sta.password;
        password_len = sizeof(wifi_config.sta.password);
    }
    else if (_mode == WIFI_MODE_AP)
    {
        wifi_auth_mode_t authmode;
        wifi_cipher_type_t ciphertype;

        /* Convert security mode */
        if (!convert_llnet_to_esp32_security_mode_f(_i_security_mode, &authmode, &ciphertype))
        {
            WIFI_ESP32_ERROR_TRACE("Cannot convert security mode\n");
            return false;
        }

        /* Initialize wifi config */
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

    /* Consistency check of the ssid length */
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
        EventBits_t uxBits;

        /* Get connected status */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, CONNECTED_BIT, pdFALSE, pdTRUE, 0);

        /* Disconnect if needed */
        if (uxBits & CONNECTED_BIT)
        {
            /* Set the current connection status */
            set_connecting_state(WIFI_ESP32_STA_DISCONNECTING);

            /* Disconnect from AP */
            if (!disconnect_f())
            {
                WIFI_ESP32_ERROR_TRACE("Disconnecting from AP failed\n");
                set_connecting_state(WIFI_ESP32_STA_IDLE);
                return false;
            }

            /* Set the current connection status */
            set_connecting_state(WIFI_ESP32_STA_IDLE);
        }
    }

    /* Stop wifi module */
    if (!stop_wifi_f(_mode, false))
    {
        WIFI_ESP32_ERROR_TRACE("Stopping wifi failed\n");
        return false;
    }

    /* Check the requested mode */
    if (_mode == WIFI_MODE_STA)
    {
        /* Set the current connection status */
        set_connecting_state(WIFI_ESP32_STA_CONNECTING);

        /* Start wifi module if needed */
        if (!start_wifi_f(_mode, &wifi_config, false))
        {
            WIFI_ESP32_ERROR_TRACE("Starting wifi failed\n");
            set_connecting_state(WIFI_ESP32_STA_IDLE);
            return false;
        }

        /* Connect to AP */
        if (!connect_f())
        {
            WIFI_ESP32_ERROR_TRACE("Connecting to AP failed\n");
            set_connecting_state(WIFI_ESP32_STA_IDLE);
            return false;
        }

        /* Set the current connection status */
        set_connecting_state(WIFI_ESP32_STA_CONNECTED);
    }
    else
    {
        /* Set the current connection status */
        set_connecting_state(WIFI_ESP32_AP_CONNECTING);

        /* Start wifi module in AP mode */
        if (!start_wifi_f(_mode, &wifi_config, true))
        {
            WIFI_ESP32_ERROR_TRACE("Starting wifi failed\n");
            set_connecting_state(WIFI_ESP32_AP_IDLE);
            return false;
        }

        /* Set the current connection status */
        set_connecting_state(WIFI_ESP32_AP_CONNECTED);
    }

    return true;
}

static bool leave_ap_or_softap_f(wifi_mode_t mode)
{
    /* Check the requested mode */
    if (mode == WIFI_MODE_STA)
    {
        EventBits_t uxBits;

        /* Get connected status */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, CONNECTED_BIT, pdFALSE, pdTRUE, 0);

        /* Disconnect if needed */
        if (uxBits & CONNECTED_BIT)
        {
            /* Set the current connection status */
            set_connecting_state(WIFI_ESP32_STA_DISCONNECTING);

            /* Disconnect from AP */
            if (!disconnect_f())
            {
                WIFI_ESP32_ERROR_TRACE("Disconnecting from AP failed\n");
                set_connecting_state(WIFI_ESP32_STA_IDLE);
                return false;
            }

            /* Set the current connection status */
            set_connecting_state(WIFI_ESP32_STA_IDLE);
        }
        else
        {
            /* Nothing to do */
            WIFI_ESP32_INFO_TRACE("The station is not connected to an AP. Nothing to do.\n");
        }
    }
    else if (mode == WIFI_MODE_AP)
    {
        /* Set the current connection status */
        set_connecting_state(WIFI_ESP32_AP_DISCONNECTING);

        /* Stop wifi module */
        if (!stop_wifi_f(mode, true))
        {
            WIFI_ESP32_ERROR_TRACE("Stopping wifi failed\n");
            set_connecting_state(WIFI_ESP32_AP_IDLE);
            return false;
        }

        /* Start the wifi module in default configuration */
        if (!start_wifi_f(WIFI_MODE_STA, (wifi_config_t *) &g_wifi_default_config, true))
        {
            WIFI_ESP32_ERROR_TRACE("Start wifi failed\n");
            set_connecting_state(WIFI_ESP32_AP_IDLE);
            return false;
        }

        /* Set the current connection status */
        set_connecting_state(WIFI_ESP32_AP_IDLE);
    }
    else
    {
        WIFI_ESP32_ERROR_TRACE("Unsupported mode parameter = %d\n", mode);
        return false;
    }

    return true;
}

static void set_connecting_state(WIFI_ESP32_connection_state_t state)
{
    switch (state)
    {
    case WIFI_ESP32_NOT_INITIALIZED:
        g_esp32_driver.connect_state = WIFI_ESP32_NOT_INITIALIZED;
        break;

    case WIFI_ESP32_STA_IDLE:
    case WIFI_ESP32_STA_CONNECTING:
    case WIFI_ESP32_STA_CONNECTED:
    case WIFI_ESP32_STA_DISCONNECTING:
        g_esp32_driver.connect_state &= ~(WIFI_ESP32_STA_IDLE | WIFI_ESP32_STA_CONNECTING | WIFI_ESP32_STA_CONNECTED | WIFI_ESP32_STA_DISCONNECTING);
        g_esp32_driver.connect_state |= state;
        break;

    case WIFI_ESP32_AP_IDLE:
    case WIFI_ESP32_AP_CONNECTING:
    case WIFI_ESP32_AP_CONNECTED:
    case WIFI_ESP32_AP_DISCONNECTING:
        g_esp32_driver.connect_state &= ~(WIFI_ESP32_AP_IDLE | WIFI_ESP32_AP_CONNECTING | WIFI_ESP32_AP_CONNECTED | WIFI_ESP32_AP_DISCONNECTING);
        g_esp32_driver.connect_state |= state;
        break;

    default:
        WIFI_ESP32_ERROR_TRACE("WiFI state %d not handled\n", state);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////Public Functions///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool WIFI_ESP32_initialize_f(void)
{
    esp_err_t status;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state == WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Initialize the driver state */
    g_esp32_driver.connect_state = WIFI_ESP32_NOT_INITIALIZED;

    /* Initialize buffers */
    memset((void *) &gpst_available_ap_list, 0, sizeof(gpst_available_ap_list));

    /* Create the wifi event group */
    gst_wifi_event_group = xEventGroupCreate();

    /* Initialize the event loop */
    status = esp_event_loop_init(event_handler_f, NULL);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Initialize event loop failed, (err=%x)\n", status);
        return false;
    }

    /*
     * Initializing the TCP/IP stack, initializing the wifi module and starting the wifi module must be done here
     * and not in the "start" function, in order that LwIP creates the underlying netif already at this point.
     *
     * Otherwise, if the LwIP CCO makes any socket access before the underlying netif is created, this would lead to errors.
     */

    /* Initialize the TCP/IP stack */
    tcpip_adapter_init();

    /* Initialize wifi module */
    if (!initialize_wifi_f())
    {
        WIFI_ESP32_ERROR_TRACE("Initialize wifi failed\n");
        return false;
    }

    /* Start the wifi module in default configuration */
    if (!start_wifi_f(WIFI_MODE_STA, (wifi_config_t *) &g_wifi_default_config, true))
    {
        WIFI_ESP32_ERROR_TRACE("Start wifi failed\n");
        return false;
    }

    /* Set the current connection status */
    set_connecting_state(WIFI_ESP32_STA_IDLE);
    set_connecting_state(WIFI_ESP32_AP_IDLE);

    return true;
}

bool WIFI_ESP32_apply_configuration_f(bool is_static, ip4_addr_t ip, ip4_addr_t netmask, ip4_addr_t gw)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Stop the DHCP client if the requested configuration is static */
    if (is_static)
    {
        tcpip_adapter_dns_info_t dns_addr[TCPIP_ADAPTER_DNS_MAX];
        tcpip_adapter_ip_info_t static_ip;
        esp_err_t status;

        /* Stop the DHCP Client */
        status = tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
        if ((status != ESP_OK) && (status != ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED))
        {
            WIFI_ESP32_ERROR_TRACE("Stop DHCP Client failed, (err=%x)\n", status);
            return false;
        }

        /* Setting the static IP will clear the DNS server list, so save it to be able to restore afterward */
        for (uint8_t idx = 0; idx < (sizeof(dns_addr) / sizeof(dns_addr[0])); idx++)
        {
            dns_addr[idx].ip = *dns_getserver(idx);
        }

        /* Set the static IP configuration */
        static_ip.ip = ip;
        static_ip.netmask = netmask;
        static_ip.gw = gw;
        status = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &static_ip);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Set static IP failed, (err=%x)\n", status);
            return false;
        }

        /* Restore the DNS server list in the tcpip_adapter */
        for (uint8_t idx = 0; idx < (sizeof(dns_addr) / sizeof(dns_addr[0])); idx++)
        {
            if (ip_2_ip4(&dns_addr[idx].ip)->addr != IPADDR_ANY)
            {
                status = tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_STA, idx, &dns_addr[idx]);
                if (status != ESP_OK)
                {
                    WIFI_ESP32_ERROR_TRACE("Set static DNS failed, (err=%x)\n", status);
                    return false;
                }
            }
            else
            {
                break;
            }
        }

        /* DNS servers are also updated, so update this variable to notify the soft DNS foundation library */
        dns_servers_list_updated = 1;
    }
    else
    {
        /* The DHCP Client is started by default when the interface is started */
    }

    return true;
}

bool WIFI_ESP32_start_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Start the module only if it was stopped before */
    if (g_esp32_driver.connect_state == WIFI_ESP32_NOT_INITIALIZED)
    {
        /* Start the wifi module in default configuration */
        if (!start_wifi_f(WIFI_MODE_STA, (wifi_config_t *) &g_wifi_default_config, true))
        {
            WIFI_ESP32_ERROR_TRACE("Start wifi failed\n");
            return false;
        }

        /* Set the current connection status */
        set_connecting_state(WIFI_ESP32_STA_IDLE);
        set_connecting_state(WIFI_ESP32_AP_IDLE);
    }

    return true;
}

bool WIFI_ESP32_stop_f(void)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Stop wifi module */
    if (!stop_wifi_f(WIFI_MODE_STA, true))
    {
        WIFI_ESP32_ERROR_TRACE("Stopping wifi failed\n");
        return false;
    }

    /* Module must be initialized again for further usage */
    set_connecting_state(WIFI_ESP32_NOT_INITIALIZED);

    return true;
}

bool WIFI_ESP32_netif_is_started_f(unsigned char * _puc_netif_started)
{
    WIFI_ESP32_ASSERT(_puc_netif_started != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    if (g_esp32_driver.connect_state == WIFI_ESP32_NOT_INITIALIZED)
    {
        *_puc_netif_started = 0;
    }
    else
    {
        *_puc_netif_started = 1;
    }

    return true;
}

bool WIFI_ESP32_join_f(const char * _pc_ssid, int _i_ssid_length, const char * _pc_passphrase, int _i_passphrase_length, int _i_security_mode)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pc_ssid != NULL);
    WIFI_ESP32_ASSERT(_i_ssid_length > 0);
    WIFI_ESP32_ASSERT(_pc_passphrase != NULL);
    WIFI_ESP32_ASSERT(_i_passphrase_length > 0);
    WIFI_ESP32_ASSERT(_i_security_mode >= 0);
    WIFI_ESP32_DEBUG_TRACE("(%s) ssid = %s\n", __func__, _pc_ssid);

    return join_ap_or_softap_f(_pc_ssid, _i_ssid_length, _pc_passphrase, _i_passphrase_length, _i_security_mode, WIFI_MODE_STA);
}

bool WIFI_ESP32_leave_f(void)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    return leave_ap_or_softap_f(WIFI_MODE_STA);
}

bool WIFI_ESP32_get_bssid_f(unsigned char * _puc_bssid, int _i_bssid_length)
{
    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_puc_bssid != NULL);
    WIFI_ESP32_ASSERT(_i_bssid_length >= sizeof(gpst_available_ap_list[0].bssid));
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Cannot get the ap info (err=%x)\n", status);
        return false;
    }

    /* Store bssid information */
    memcpy((void *) _puc_bssid, (void *) ap_info.bssid, (size_t) _i_bssid_length);

    return true;
}

bool WIFI_ESP32_get_channel_f(short * _ps_channel)
{
    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_ps_channel != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Cannot get the ap info (err=%x)\n", status);
        return false;
    }

    /* Store channel information */
    *_ps_channel = ap_info.primary;

    return true;
}

bool WIFI_ESP32_get_rssi_f(float * _pf_rssi, int _i_rssi_length)
{
    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pf_rssi != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Cannot get the ap info (err=%x)\n", status);
        return false;
    }

    /* Store rssi information */
    * _pf_rssi = ap_info.rssi;

    return true;
}

bool WIFI_ESP32_get_security_mode_f(short * _ps_mode)
{
    wifi_ap_record_t ap_info;
    int security_mode;
    esp_err_t status;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_ps_mode != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Cannot get the ap info (err=%x)\n", status);
        return false;
    }

    /* Translate the security mode */
    if (!convert_esp32_to_llnet_security_mode_f(&security_mode, ap_info.authmode, ap_info.group_cipher))
    {
        WIFI_ESP32_ERROR_TRACE("Cannot translate the security mode\n");
        return false;
    }

    /* Check if the security mode cannot be handled. In this case we need to change the whole NETIF */
    if (security_mode > 0xFFFF)
    {
        WIFI_ESP32_ERROR_TRACE("Cannot handle so many Security Modes = %d\n", security_mode);
        return false;
    }

    *_ps_mode = (short) (security_mode & 0xFFFF);

    return true;
}

bool WIFI_ESP32_get_ssid_f(char * _pc_ssid, int _i_ssid_length)
{
    wifi_ap_record_t ap_info;
    esp_err_t status;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pc_ssid != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Get AP info */
    status = esp_wifi_sta_get_ap_info(&ap_info);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Cannot get the ap info (err=%x)\n", status);
        return false;
    }

    /* Store ssid information */
    memcpy((void *) _pc_ssid, (void *) ap_info.ssid, (size_t) _i_ssid_length);

    return true;
}

bool WIFI_ESP32_get_wps_modes_f(short * _ps_wps_modes)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_ps_wps_modes != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Not supported by the ESP32 APIs */
    *_ps_wps_modes = WPS_MODE_NONE;

    return true;
}

bool WIFI_ESP32_enable_softap_f(const char * _pc_ssid, int _i_ssid_length, const char * _pc_passphrase, int _i_passphrase_length)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pc_ssid != NULL);
    WIFI_ESP32_ASSERT(_i_ssid_length > 0);
    WIFI_ESP32_ASSERT(_pc_passphrase != NULL);
    WIFI_ESP32_ASSERT(_i_passphrase_length > 0);
    WIFI_ESP32_DEBUG_TRACE("(%s) ssid = %s\n", __func__, _pc_ssid);

    return join_ap_or_softap_f(_pc_ssid, _i_ssid_length, _pc_passphrase, _i_passphrase_length, SECURITY_MODE_WPA2, WIFI_MODE_AP);
}

bool WIFI_ESP32_set_name_softap_f(const char * _pc_name, int _i_name_length)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pc_name != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s) ssid = %s\n", __func__, _pc_name);

    /* Not supported by the ESP32 APIs */

    return true;
}

bool WIFI_ESP32_disable_softap_f(void)
{
    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    return leave_ap_or_softap_f(WIFI_MODE_AP);
}

bool WIFI_ESP32_get_ap_count_f(short * const _ps_ap_count, int8_t active)
{
    wifi_scan_config_t scan_config = {
            .ssid = NULL,
            .bssid = NULL,
            .channel = 0,
            .show_hidden = false,
            .scan_time.active.min = 0,
            .scan_time.active.max = 0
    };
    EventBits_t uxBits;
    uint16_t num_scan;
    esp_err_t status;
    bool scan_succeeded = false;
    int8_t retry_scan = 1;

    if (active)
    {
        scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    }
    else
    {
        scan_config.scan_type = WIFI_SCAN_TYPE_PASSIVE;
    }

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_ps_ap_count != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    /* Scanning is not possible in SoftAP mode */
    if (g_esp32_driver.connect_state & WIFI_ESP32_AP_CONNECTED)
    {
        /* Change the wifi mode to allow scanning in SoftAP mode */
        status = esp_wifi_set_mode(WIFI_MODE_APSTA);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Set station mode failed, (err=%x)\n", status);
            set_connecting_state(WIFI_ESP32_AP_IDLE);
            return false;
        }

        /* Wait for the start event */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, START_BIT, pdTRUE, pdTRUE, START_TIMEOUT_MS / portTICK_RATE_MS);
        if (uxBits & START_BIT)
        {
            WIFI_ESP32_INFO_TRACE("Wifi module started\n");
        }
        else
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the start event ended in a timeout\n");
            set_connecting_state(WIFI_ESP32_AP_IDLE);
            return false;
        }
    }

    /* Start a non-blocking scan */
    WIFI_ESP32_INFO_TRACE("Scanning AP...\n");

    do {
        status = esp_wifi_scan_start(&scan_config, false);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Scan start failed with error (err= %x)\n", status);
            return false;
        }

        /* Wait for the scan event */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, SCAN_BIT, pdTRUE, pdTRUE, SCAN_TIMEOUT_MS / portTICK_RATE_MS);
        if (uxBits & SCAN_BIT)
        {
            WIFI_ESP32_INFO_TRACE("AP scan ok\n");
            scan_succeeded = true;
        }
        else
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the scan event ended in a timeout\n");
            if (retry_scan > 0)
            {
                /* Workaround for ESP-IDF bug: wifi scan impossible after a failed attempt to reconnect to a hotspot. Disconnect from AP */

                /* Set the current connection status */
                set_connecting_state(WIFI_ESP32_STA_DISCONNECTING);

                /* Disconnect from AP */
                if (!disconnect_f())
                {
                    WIFI_ESP32_ERROR_TRACE("Disconnecting from AP failed\n");
                }

                /* Set the current connection status */
                set_connecting_state(WIFI_ESP32_STA_IDLE);
            }
            else
            {
                return false;
            }
        }
    } while (!scan_succeeded && (retry_scan-- > 0));

    /* Scanning is not possible in SoftAP mode */
    if (g_esp32_driver.connect_state & WIFI_ESP32_AP_CONNECTED)
    {
        /* Restore the wifi mode */
        status = esp_wifi_set_mode(WIFI_MODE_AP);
        if (status != ESP_OK)
        {
            WIFI_ESP32_ERROR_TRACE("Set station mode failed, (err=%x)\n", status);
            set_connecting_state(WIFI_ESP32_AP_IDLE);
            return false;
        }

        /* Wait for the stop event */
        uxBits = xEventGroupWaitBits(gst_wifi_event_group, STOP_BIT, pdTRUE, pdTRUE, STOP_TIMEOUT_MS / portTICK_RATE_MS);
        if (uxBits & STOP_BIT)
        {
            WIFI_ESP32_INFO_TRACE("Wifi module stopped\n");
        }
        else
        {
            WIFI_ESP32_ERROR_TRACE("Waiting for the stop event ended in a timeout\n");
            set_connecting_state(WIFI_ESP32_AP_IDLE);
            return false;
        }
    }

    /* Get the number of scanned AP */
    status = esp_wifi_scan_get_ap_num(&num_scan);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get scan ap num failed, (err=%x)\n", status);
        return false;
    }

    /* Limit the number of scanned AP to the max supported by this driver */
    if (num_scan > (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0])))
    {
        WIFI_ESP32_WARNING_TRACE("Scanned %d AP, handling the first %d AP\n", num_scan, (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0])));
        num_scan = (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0]));
    }

    /* Get the AP records */
    status = esp_wifi_scan_get_ap_records(&num_scan, gpst_available_ap_list);
    if (status != ESP_OK)
    {
        WIFI_ESP32_ERROR_TRACE("Get ap records failed with error (err= %x)\n", status);
        return false;
    }

    *_ps_ap_count = (short) (num_scan & 0xFFFF);

    return true;
}

bool WIFI_ESP32_get_ap_ssid_f(unsigned int _ui_index, unsigned char * _puc_ssid, int _i_ssid_length)
{
    bool b_return;
    unsigned int ui_copy_length;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_puc_ssid != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s) index = %u\n", __func__, _ui_index);

    if ((_ui_index < (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0]))) && (_i_ssid_length > 0))
    {
        b_return = true;

        int ap_ssid_length = strlen((char *) gpst_available_ap_list[_ui_index].ssid);
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

    return b_return;
}

bool WIFI_ESP32_get_ap_bssid_f(unsigned int _ui_index, unsigned char * _puc_bssid, int _i_bssid_length)
{
    bool b_return;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_puc_bssid != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s) index = %u\n", __func__, _ui_index);

    if ((_ui_index < (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0]))) && (_i_bssid_length > 0))
    {
        int copy_length;

        b_return = true;

        if ((unsigned int) _i_bssid_length < sizeof(gpst_available_ap_list[_ui_index].bssid))
        {
            copy_length = _i_bssid_length;
        }
        else
        {
            copy_length = sizeof(gpst_available_ap_list[_ui_index].bssid) - 1;
        }

        strncpy((char *) _puc_bssid, (char *) gpst_available_ap_list[_ui_index].bssid, (size_t) copy_length);
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_ESP32_get_ap_channel_f(unsigned int _ui_index, int * _pi_channel)
{
    bool b_return;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pi_channel != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s) index = %u\n", __func__, _ui_index);

    if (_ui_index < (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0])))
    {
        b_return = true;

        *_pi_channel = gpst_available_ap_list[_ui_index].primary;
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_ESP32_get_ap_rssi_f(unsigned int _ui_index, float * _pf_rssi, int _i_rssi_length)
{
    bool b_return;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pf_rssi != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s) index = %u\n", __func__, _ui_index);

    if ((_ui_index < (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0]))) && (_i_rssi_length > 0))
    {
        b_return = true;

        *_pf_rssi = gpst_available_ap_list[_ui_index].rssi;
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_ESP32_get_ap_security_mode_f(unsigned int _ui_index, int * _pi_mode)
{
    bool b_return;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pi_mode != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s) index = %u\n", __func__, _ui_index);

    if (_ui_index < (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0])))
    {
        b_return = convert_esp32_to_llnet_security_mode_f(_pi_mode, gpst_available_ap_list[_ui_index].authmode, gpst_available_ap_list[_ui_index].group_cipher);
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_ESP32_get_ap_wps_modes_f(unsigned int _ui_index, int * _pi_wps_cap)
{
    bool b_return;

    WIFI_ESP32_ASSERT(g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED);
    WIFI_ESP32_ASSERT(_pi_wps_cap != NULL);
    WIFI_ESP32_DEBUG_TRACE("(%s) index = %u\n", __func__, _ui_index);

    if (_ui_index < (sizeof(gpst_available_ap_list) / sizeof(gpst_available_ap_list[0])))
    {
        b_return = true;

        *_pi_wps_cap = WPS_MODE_NONE;
    }
    else
    {
        b_return = false;
    }

    return b_return;
}

bool WIFI_ESP32_get_client_state_f(void)
{
    WIFI_ESP32_DEBUG_TRACE("(%s)\n", __func__);

    if (g_esp32_driver.connect_state != WIFI_ESP32_NOT_INITIALIZED)
    {
        return true;
    }
    else
    {
        return false;
    }
}
