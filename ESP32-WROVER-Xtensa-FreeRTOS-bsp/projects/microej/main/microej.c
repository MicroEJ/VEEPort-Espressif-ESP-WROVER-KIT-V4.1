/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_task_wdt.h"
#include "nvs_flash.h"
#include "microej_main.h"
#include "esp_ota_ops.h"
#include "esp32/rom/rtc.h"
#include "sdkconfig.h"

#if CONFIG_SYSVIEW_ENABLE
#include "SEGGER_SYSVIEW.h"
#endif // CONFIG_SYSVIEW_ENABLE

#include "uart_switch.h"

/******************************************************
 *                    Constants
 ******************************************************/

/* MicroJvm task constants */
#define MICROJVM_STACK_SIZE                (21 * 1024)
#define JAVA_TASK_PRIORITY                 ( 8 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define JAVA_TASK_STACK_SIZE               (MICROJVM_STACK_SIZE / 4)
#define JAVA_TASK_NAME	                   "MicroJvm"

/******************************************************
 *               Static Function Declarations
 ******************************************************/

void __attribute__((weak)) LLCOMM_stack_initialize(void)
{
	/* Does nothing by default, override when ECOM-COMM is used */
}

static void xJavaTaskFunction(void * pvParameters)
{
	LLCOMM_stack_initialize();
	microej_main(0, NULL);
	vTaskDelete(xTaskGetCurrentTaskHandle());
}

void reset_reason(RESET_REASON reason)
{
	switch (reason)
	{
	case POWERON_RESET:
		printf("POWERON_RESET \n");
		break;
	case SW_RESET:
		printf("SW_RESET \n");
		break;
	case OWDT_RESET:
		printf("OWDT_RESET \n");
		break;
	case DEEPSLEEP_RESET:
		printf("DEEPSLEEP_RESET \n");
		break;
	case SDIO_RESET:
		printf("SDIO_RESET \n");
		break;
	case TG0WDT_SYS_RESET:
		printf("TG0WDT_SYS_RESET \n");
		break;
	case TG1WDT_SYS_RESET:
		printf("TG1WDT_SYS_RESET \n");
		break;
	case RTCWDT_SYS_RESET:
		printf("RTCWDT_SYS_RESET \n");
		break;
	case INTRUSION_RESET:
		printf("INTRUSION_RESET \n");
		break;
	case TGWDT_CPU_RESET:
		printf("TGWDT_CPU_RESET \n");
		break;
	case SW_CPU_RESET:
		printf("SW_CPU_RESET \n");
		break;
	case RTCWDT_CPU_RESET:
		printf("RTCWDT_CPU_RESET \n");
		break;
	case EXT_CPU_RESET:
		printf("EXT_CPU_RESET \n");
		break;
	case RTCWDT_BROWN_OUT_RESET:
		printf("RTCWDT_BROWN_OUT_RESET \n");
		break;
	case RTCWDT_RTC_RESET:
		printf("RTCWDT_RTC_RESET \n");
		break;
	default:
		printf("NO_MEAN \n");
	}
}

void print_reset_reason()
{
	RESET_REASON rst_reson[2];
	//get the reset reason for CPU 0
	rst_reson[0] = rtc_get_reset_reason(0);
	printf("Reset reason CPU 0:");
	//print the reset reason for CPU 0
	reset_reason(rst_reson[0]);
#if portNUM_PROCESSORS > 1
	//get the reset reason for CPU 1
	rst_reson[1] = rtc_get_reset_reason(1);
	printf("Reset reason CPU 1:");
	//print the reset reason for CPU 1
	reset_reason(rst_reson[1]);
#endif
}

void app_main()
{
	uart_switch();

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    print_reset_reason();

    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    /* Print partition information */
    const esp_partition_t *partition = esp_ota_get_running_partition();
    if (partition != NULL) {
        switch (partition->subtype) {
        case ESP_PARTITION_SUBTYPE_APP_FACTORY:
            printf("Running application from factory partition\n");
            break;
        case ESP_PARTITION_SUBTYPE_APP_OTA_0:
            printf("Running application from ota_0 partition\n");
            break;
        case ESP_PARTITION_SUBTYPE_APP_OTA_1:
            printf("Running application from ota_1 partition\n");
            break;
        default:
            printf("Running application from unknown partition\n");
            break;
        }
    } else {
        printf("Cannot retrieve running partition\n");
    }

    /* Start the MicroJvm thread */
    TaskHandle_t pvCreatedTask;
    xTaskCreate(xJavaTaskFunction, JAVA_TASK_NAME, JAVA_TASK_STACK_SIZE, NULL, JAVA_TASK_PRIORITY, &pvCreatedTask);
#if CONFIG_SYSVIEW_ENABLE == 1
start_sysview_logging:
    SEGGER_SYSVIEW_setMicroJVMTask((U32)pvCreatedTask);
#endif // CONFIG_SYSVIEW_ENABLE
}
