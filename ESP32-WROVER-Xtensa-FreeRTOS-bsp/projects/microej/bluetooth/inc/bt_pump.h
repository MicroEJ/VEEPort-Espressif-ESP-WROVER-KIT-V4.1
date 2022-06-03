/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef BLUETOOTH_PUMP_H_
#define BLUETOOTH_PUMP_H_

#include <stdint.h>

#include "LLBLUETOOTH_defines.h"

/*
 * Starts the Bluetooth pump task.
 */
bool BT_PUMP_start(void);

/*
 * Stops the Bluetooth pump task.
 */
void BT_PUMP_stop(void);

/*
 * Gets the next Bluetooth event from the event queue. If there is no event in the queue, the
 * current Java thread is paused.
 */
int32_t BT_PUMP_wait_event(LLBLUETOOTH_event_t *event, uint32_t max_event_size);

/*
 * Sets the given scan filter. Scan result which do not fit this filter will not be returned in
 * BT_PUMP_wait_event().
 */
void BT_PUMP_set_scan_filter(uint8_t filter_action, uint8_t filter_type, const uint8_t *filter_data,
		uint32_t filter_data_size);

#endif
