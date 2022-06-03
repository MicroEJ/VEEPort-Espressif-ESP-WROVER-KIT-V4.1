/*
 * C
 *
 * Copyright 2018 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_task_wdt.h"
#include "nvs_flash.h"
#include "Outputter.h"
#include "TextUIRunner.h"
#include "TextOutputter.h"

/******************************************************
 *                    Constants
 ******************************************************/

/* UnitTests task constants */
#define UNIT_TESTS_STACK_SIZE              (21 * 1024)
#define UNIT_TESTS_TASK_PRIORITY           ( CONFIG_TIMER_TASK_PRIORITY - 1 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define UNIT_TESTS_TASK_STACK_SIZE         (UNIT_TESTS_STACK_SIZE / 4)
#define UNIT_TESTS_TASK_NAME	           "UnitTests"

/* Test constants */
#define PERFORM_RAM_TEST

/******************************************************
 *               External Function Declarations
 ******************************************************/
extern TestRef ram_tests(void);

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void xUnitTestsTaskFunction(void * pvParameters)
{
	TextUIRunner_setOutputter(TextOutputter_outputter());

	TextUIRunner_start();

#ifdef PERFORM_RAM_TEST
	printf("\r\nPerform RAM tests.\r\n");
	TextUIRunner_runTest(ram_tests());
#endif

	TextUIRunner_end();

	vTaskDelete(xTaskGetCurrentTaskHandle());
}

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

    /* Start the UnitTests thread */
    xTaskCreate(xUnitTestsTaskFunction, UNIT_TESTS_TASK_NAME, UNIT_TESTS_TASK_STACK_SIZE, NULL, UNIT_TESTS_TASK_PRIORITY, NULL);
}
