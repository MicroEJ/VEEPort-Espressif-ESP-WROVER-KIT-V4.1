#!/bin/bash
#
# BASH
#
# Copyright 2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

# Set IDF tools path.
export IDF_TOOLS_PATH=$HOME/.espressif

# Set serial port. Set a value (e.g. COM4).
export ESPPORT=/dev/ttyUSB0
echo ESPPORT=$ESPPORT

# Set build directory.
export ESP_BUILD_DIR="$(pwd)/projects/microej/build"
#echo ESP_BUILD_DIR=$ESP_BUILD_DIR