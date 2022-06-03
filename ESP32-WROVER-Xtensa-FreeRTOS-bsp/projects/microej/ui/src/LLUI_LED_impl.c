/*
 * C
 *
 * Copyright 2016-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "LLUI_LED_impl.h"
#include "driver/ledc.h"

/* Uncomment to enable all LEDs supported by the HW. Careful, the GPIOs used by some LEDs may be used by other modules */
//#define ALL_LEDS_AVAILABLE

/* User LEDs available */
#if ALL_LEDS_AVAILABLE
#define MAX_NUMBER_OF_LEDS 3
#else
#define MAX_NUMBER_OF_LEDS 1
#endif

#define MIN_LEDC_INTENSITY    0.0 // 10bit resolution, so maximum duty cycle is         0
#define MAX_LEDC_INTENSITY 1023.0 // 10bit resolution, so maximum duty cycle is (2^10 - 1)

/* LEDC timer configuration */
static ledc_timer_config_t ledc_timer = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.duty_resolution = LEDC_TIMER_10_BIT,
		.timer_num = LEDC_TIMER_0,
		.freq_hz = 5000
};

/* LED configuration as LEDC channels */
static ledc_channel_config_t ledc_channel[MAX_NUMBER_OF_LEDS] = {
#if ALL_LEDS_AVAILABLE
		/* LED0 is the Blue LED of the RGB matrix (shared with the custom UART logger) */
		{
				.gpio_num = GPIO_NUM_4,
				.speed_mode = LEDC_LOW_SPEED_MODE,
				.channel = LEDC_CHANNEL_0,
				.intr_type = LEDC_INTR_DISABLE,
				.timer_sel = LEDC_TIMER_0,
				.duty = 0
		},
#endif
		/* LED1 is the Red LED of the RGB matrix */
		{
				.gpio_num = GPIO_NUM_0,
				.speed_mode = LEDC_LOW_SPEED_MODE,
				.channel = LEDC_CHANNEL_1,
				.intr_type = LEDC_INTR_DISABLE,
				.timer_sel = LEDC_TIMER_0,
				.duty = 0
		},
#if ALL_LEDS_AVAILABLE
		/* LED2 is the Green LED of the RGB matrix (shared with the MicroSD) */
		{
				.gpio_num = GPIO_NUM_2,
				.speed_mode = LEDC_LOW_SPEED_MODE,
				.channel = LEDC_CHANNEL_2,
				.intr_type = LEDC_INTR_DISABLE,
				.timer_sel = LEDC_TIMER_0,
				.duty = 0
		},
#endif
};

// --------------------------------------------------------------------------------
// -                      Functions                                               -
// --------------------------------------------------------------------------------
/**
 * Initializes the LEDs.
 * @return the number of LEDs in the group.
 */
int32_t LLUI_LED_IMPL_initialize(void)
{
	// Set configuration of the LEDC timer
	ledc_timer_config(&ledc_timer);

	// Set LED Controller with previously prepared configuration
	for (int32_t i = 0; i < MAX_NUMBER_OF_LEDS; i++) {
		ledc_channel_config(&ledc_channel[i]);
	}

    return MAX_NUMBER_OF_LEDS;
}

/**
 * Get the intensity of the LED.
 * @return the intensity of the LED
 */
int32_t LLUI_LED_IMPL_getIntensity(int32_t ledID)
{
	if ((ledID >= 0) && (ledID <= MAX_NUMBER_OF_LEDS)) {
		double duty = LLUI_LED_MIN_INTENSITY +
				((LLUI_LED_MAX_INTENSITY - LLUI_LED_MIN_INTENSITY) / (MAX_LEDC_INTENSITY - MIN_LEDC_INTENSITY))
				* (ledc_get_duty(ledc_channel[ledID].speed_mode, ledc_channel[ledID].channel) - MIN_LEDC_INTENSITY);
    	return (int32_t)duty;
	} else {
		return LLUI_LED_MIN_INTENSITY;
	}
}

/**
 * Set the intensity of the LED.
 * A 0 intensity turns off the LED, intensity of 255 turns on the LED to max power.
 * @param intensity the intensity of the LED
 */
void LLUI_LED_IMPL_setIntensity(int32_t ledID, int32_t intensity)
{
	if ((ledID >= 0) && (ledID <= MAX_NUMBER_OF_LEDS)) {
		/* Saturate minimum intensity */
		if (intensity < LLUI_LED_MIN_INTENSITY) {
			intensity = LLUI_LED_MIN_INTENSITY;
		}

		/* Saturate maximum intensity */
		if (intensity > LLUI_LED_MAX_INTENSITY) {
			intensity = LLUI_LED_MAX_INTENSITY;
		}

		/* Compute duty cycle and update it in the LEDC */
		double duty = ceil((MIN_LEDC_INTENSITY +
				((MAX_LEDC_INTENSITY - MIN_LEDC_INTENSITY) / (LLUI_LED_MAX_INTENSITY - LLUI_LED_MIN_INTENSITY))
				* (intensity - LLUI_LED_MIN_INTENSITY)));
		ledc_set_duty(ledc_channel[ledID].speed_mode, ledc_channel[ledID].channel, (uint32_t)duty);
		ledc_update_duty(ledc_channel[ledID].speed_mode, ledc_channel[ledID].channel);
	}
}
