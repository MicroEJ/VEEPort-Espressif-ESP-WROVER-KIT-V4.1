/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef _LLUI_DISPLAY_CONFIGURATION
#define _LLUI_DISPLAY_CONFIGURATION

/* Defines -------------------------------------------------------------------*/

/**
 * Configure the LLUI_DISPLAY number of bits per pixel (BPP).
 *
 * There are 2 available BPP modes:
 * 	- 16:		16 BPP, in 5-6-5 mode
 * 	- 24:		24 BPP, in 8-8-8 mode
 *
 * Only one BPP can be used at any time. This define allows to select the wanted
 * implementation:
 * 	- '16':		use 16 BPP
 * 	- '24':		use 24 BPP
 *
 * According the chosen mode, there are some actions which must be done together:
 *
 * (1)	In MicroEJ, select the wanted display stack during the platform creation. In the display
 * 		configuration properties file of the configuration project
 * 		"[platform configuration project]/display/display.properties",
 * 		specify the expected BPP value and format (RGB565 or RGB888)
 *
 * (2)	Recompile the platform.
 *
 * (3)	Launch again the Java application against the new platform.
 *
 * (4)	Configure the define LLUI_DISPLAY_BPP (below) with the wanted mode.
 *
 * (5)	Recompile your BSP.
 *
 */
#define LLUI_DISPLAY_BPP 16


#endif
