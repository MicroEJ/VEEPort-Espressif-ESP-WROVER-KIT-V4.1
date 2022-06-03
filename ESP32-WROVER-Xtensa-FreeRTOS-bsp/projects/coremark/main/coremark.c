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

/******************************************************
 *                    Constants
 ******************************************************/

/* CoreMark tasks constants */
#define COREMARK_STACK_SIZE                (8 * 1024)
#define COREMARK_TASK_STACK_SIZE           (COREMARK_STACK_SIZE / 4)

/* CoreMark core 0 task constants */
#define COREMARK_CORE0_TASK_PRIORITY       ( 21 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define COREMARK_CORE0_TASK_NAME           "CoreMarkCore0"

#if portNUM_PROCESSORS > 1
/* CoreMark core 1 task constants */
#define COREMARK_CORE1_TASK_PRIORITY       ( 20 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define COREMARK_CORE1_TASK_NAME           "CoreMarkCore1"
#endif

/******************************************************
 *               External Function Declarations
 ******************************************************/
extern int core_main(void);

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void xCoreMarkCore0Function(void * pvParameters)
{
    /* Run CoreMark */
	printf("CoreMark Start on Core 0\n");
    core_main();
	printf("CoreMark Finish on Core 0\n");

    /* If task exited, delete it */
    vTaskDelete(xTaskGetCurrentTaskHandle());
}

#if portNUM_PROCESSORS > 1
static void xCoreMarkCore1Function(void * pvParameters)
{
    /* Run CoreMark */
	printf("CoreMark Start on Core 1\n");
    core_main();
	printf("CoreMark Finish on Core 1\n");

    /* If task exited, delete it */
    vTaskDelete(xTaskGetCurrentTaskHandle());
}
#endif

void app_main()
{
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

    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    printf("SysClkFreq = %d MHz\n", CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ);

    /* Start the CoreMark threads */
    xTaskCreatePinnedToCore(xCoreMarkCore0Function, COREMARK_CORE0_TASK_NAME, COREMARK_TASK_STACK_SIZE, NULL, COREMARK_CORE0_TASK_PRIORITY, NULL, 0);
#if portNUM_PROCESSORS > 1
    xTaskCreatePinnedToCore(xCoreMarkCore1Function, COREMARK_CORE1_TASK_NAME, COREMARK_TASK_STACK_SIZE, NULL, COREMARK_CORE1_TASK_PRIORITY, NULL, 1);
#endif
}
