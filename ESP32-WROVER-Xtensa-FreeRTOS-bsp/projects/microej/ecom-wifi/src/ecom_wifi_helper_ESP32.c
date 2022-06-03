/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdint.h>
#include <string.h>
#include "WIFI_ESP32_driver.h"
#include "ecom_wifi_helper.h"
#include "microej_async_worker.h"
#include "LLNET_ERRORS.h"
#include "LLECOM_WIFI_impl.h"

#ifdef LLECOM_WIFI_DEBUG
#include <stdio.h>
#endif

#ifdef LLECOM_WIFI_DEBUG
#define LLECOM_WIFI_DEBUG_TRACE printf("[DEBUG] ");printf
#else
#define LLECOM_WIFI_DEBUG_TRACE(...) ((void) 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void LLECOM_WIFI_IMPL_enableSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_enable_softap_t* param = (ECOM_WIFI_enable_softap_t*) job->params;

	if ((param->ssid == NULL) || (param->passphrase == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {

		int8_t* ssid_buffer = param->ssid;
		int8_t* pass_buffer = param->passphrase;
		bool result;

		result = WIFI_ESP32_enable_softap_f((char*)ssid_buffer, param->ssidLength, (char*)pass_buffer, param->passphraseLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] enable result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_disableSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_info_t* param = (ECOM_WIFI_get_info_t*) job->params;

	bool result;

	result = WIFI_ESP32_disable_softap_f();
	if (result == true) {
		param->result = 0;
	} else {
		param->result = J_EUNKNOWN;
		if (param->error_message != NULL) {
			param->error_message = "wifi esp32 driver error";
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] disable result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_setNameSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_set_name_t* param = (ECOM_WIFI_set_name_t*) job->params;

	if (param->name == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		int8_t* buffer = param->name;
		bool result;

		result = WIFI_ESP32_set_name_softap_f((char*)buffer, param->nameLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] name to set : %s (err %d)\n", __func__, __LINE__, param->buffer_name, param->error_code);
}

void LLECOM_WIFI_IMPL_getBSSID_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_xssid_t* param = (ECOM_WIFI_get_xssid_t*) job->params;

	if (param->xssid == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		int8_t* buffer = param->buffer_xssid;
		bool result;

		result = WIFI_ESP32_get_bssid_f((unsigned char*)buffer, param->xssidLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] bssid (err %d)\n", __func__, __LINE__, param->error_code);
}

void LLECOM_WIFI_IMPL_getChannel_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_info_t* param = (ECOM_WIFI_get_info_t*) job->params;

	short channel;
	bool result;

	result = WIFI_ESP32_get_channel_f(&channel);
	if (result == true) {
		param->result = (int32_t)channel;
	} else {
		param->result = J_EUNKNOWN;
		if (param->error_message != NULL) {
			param->error_message = "wifi esp32 driver error";
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] channel : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_getClientState_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_info_t* param = (ECOM_WIFI_get_info_t*) job->params;

	bool result;

	result = WIFI_ESP32_get_client_state_f();
	if (result == true) {
		param->result = WIFI_STATE_STARTED;
	} else {
		param->result = WIFI_STATE_IDLE;
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] client state : %d \n", __func__, __LINE__, param->result);
}

void LLECOM_WIFI_IMPL_getAccessPointState_action(MICROEJ_ASYNC_WORKER_job_t* job) {
	
	ECOM_WIFI_get_info_t* param = (ECOM_WIFI_get_info_t*) job->params;
	
	bool result;

	result = WIFI_ESP32_get_access_point_state_f();
	if (result == true) {
		param->result = WIFI_STATE_STARTED;
	} else {
		param->result = WIFI_STATE_IDLE;
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] AP state : %d \n", __func__, __LINE__, param->result);
}

void LLECOM_WIFI_IMPL_getRSSI_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_rssi_t* param = (ECOM_WIFI_get_rssi_t*) job->params;

	if (param->rssi == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		float* buffer = (float *)param->rssi;
		bool result;

		result = WIFI_ESP32_get_rssi_f(buffer, param->rssiLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] rssi : %.2f (err %d)\n", __func__, __LINE__, *(param->rssi), param->error_code);
}

void LLECOM_WIFI_IMPL_getSecurityMode_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_info_t* param = (ECOM_WIFI_get_info_t*) job->params;

	short security_mode;
	bool result;

	result = WIFI_ESP32_get_security_mode_f(&security_mode);
	if (result == true) {
		param->result = (int32_t)security_mode;
	} else {
		param->result = J_EUNKNOWN;
		if (param->error_message != NULL) {
			param->error_message = "wifi esp32 driver error";
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] security mode : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_getSSID_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_xssid_t* param = (ECOM_WIFI_get_xssid_t*) job->params;

	if (param->xssid == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		int8_t* buffer = param->xssid;
		bool result;

		result = WIFI_ESP32_get_ssid_f((char*)buffer, param->xssidLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] ssid : %s (err %d)\n", __func__, __LINE__, param->buffer_xssid, param->error_code);
}

void LLECOM_WIFI_IMPL_getWPSModes_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_info_t* param = (ECOM_WIFI_get_info_t*) job->params;

	short wps_modes;
	bool result;

	result = WIFI_ESP32_get_wps_modes_f(&wps_modes);
	if (result == true) {
		param->result = (int32_t)wps_modes;
	} else {
		param->result = J_EUNKNOWN;
		if (param->error_message != NULL) {
			param->error_message = "wifi esp32 driver error";
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] wps modes : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_joinWithSecurityMode_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_join_security_mode_t* param = (ECOM_WIFI_join_security_mode_t*) job->params;

	if ((param->ssid == NULL) || (param->passphrase == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		int8_t* ssid_buffer = param->ssid;
		int8_t* pass_buffer = param->passphrase;
		bool result;

		result = WIFI_ESP32_join_f((char*)ssid_buffer, param->ssidLength, (char*)pass_buffer, param->passphraseLength, param->securityModeValue);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] join with security mode %d res %d (err %d)\n", __func__, __LINE__, param->securityModeValue, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_leave_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_get_info_t* param = (ECOM_WIFI_get_info_t*) job->params;
	bool result;

	result = WIFI_ESP32_leave_f();
	if (result == true) {
		param->result = 0;
	} else {
		param->result = J_EUNKNOWN;
		if (param->error_message != NULL) {
			param->error_message = "wifi esp32 driver error";
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] leave result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_scanAPCount_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_scanAP_count_t* param = (ECOM_WIFI_scanAP_count_t*) job->params;
	short ap_count;
	bool result;

	result = WIFI_ESP32_get_ap_count_f(&ap_count);
	if (result == true) {
		param->result = (int32_t)ap_count;
	} else {
		param->result = J_EUNKNOWN;
		if (param->error_message != NULL) {
			param->error_message = "wifi esp32 driver error";
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] AP counted : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_scanAPssidAt_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_scanAP_xssid_t* param = (ECOM_WIFI_scanAP_xssid_t*) job->params;

	if ((param->xssid == NULL) || (param->index < 0)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer or negative index";
		}
	} else {
		int8_t* buffer = param->xssid;
		bool result;

		result = WIFI_ESP32_get_ap_ssid_f(param->index, (unsigned char*)buffer, param->xssidLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] ssid : %s (err %d)\n", __func__, __LINE__, param->buffer_xssid, param->error_code);
}

void LLECOM_WIFI_IMPL_scanAPbssidAt_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_scanAP_xssid_t* param = (ECOM_WIFI_scanAP_xssid_t*) job->params;

	if ((param->xssid == NULL) || (param->index < 0)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer or negative index";
		}
	} else {
		int8_t* buffer = param->xssid;
		bool result;

		result = WIFI_ESP32_get_ap_bssid_f(param->index, (unsigned char*)buffer, param->xssidLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] bssid (err %d)\n", __func__, __LINE__, param->error_code);
}

void LLECOM_WIFI_IMPL_scanAPchannelAt_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_scanAP_channel_t* param = (ECOM_WIFI_scanAP_channel_t*) job->params;

	if (param->index < 0) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "negative index";
		}
	} else {
		int channel;
		bool result;

		result = WIFI_ESP32_get_ap_channel_f(param->index, &channel);
		if (result == true) {
			param->result = (int32_t)channel;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] channel : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_scanAPrssiAt_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_scanAP_rssi_t* param = (ECOM_WIFI_scanAP_rssi_t*) job->params;

	if ((param->rssi == NULL) || (param->index < 0)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer or negative index";
		}
	} else {
		float* buffer = (float*) param->buffer_rssi;
		bool result;

		result = WIFI_ESP32_get_ap_rssi_f(param->index, buffer, param->rssiLength);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] rssi : %.2f (err %d)\n", __func__, __LINE__, *param->rssi, param->error_code);
}

void LLECOM_WIFI_IMPL_scanAPsecurityModeAt_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_scanAP_channel_t* param = (ECOM_WIFI_scanAP_channel_t*) job->params;

	if (param->index < 0) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "negative index";
		}
	} else {
		int security_mode;
		bool result;

		result = WIFI_ESP32_get_ap_security_mode_f(param->index, &security_mode);
		if (result == true) {
			param->result = (int32_t)security_mode;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] security mode : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_WIFI_IMPL_scanAPwpsModeAt_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_WIFI_scanAP_channel_t* param = (ECOM_WIFI_scanAP_channel_t*) job->params;

	if (param->index < 0) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "negative index";
		}
	} else {
		int wps_mode;
		bool result;

		result = WIFI_ESP32_get_ap_wps_modes_f(param->index, &wps_mode);
		if (result == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_WIFI_DEBUG_TRACE("[%s:%u] wps mode : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

#ifdef __cplusplus
}
#endif
