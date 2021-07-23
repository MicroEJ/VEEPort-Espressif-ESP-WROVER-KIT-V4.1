#!/bin/bash
#
# BASH
#
# Copyright 2020 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

# 'run.sh' implementation for Espressif IDF.

# 'run.sh' is responsible for flashing the executable file on the target device 
# then resetting target device


# Set APPLICATION_FILE before changing directory
if [ -z "$1" ]; then
    APPLICATION_FILE="$(pwd)/application.out"
else
    APPLICATION_FILE="$(cd $(dirname $1) ; pwd)/$(basename $1)"
fi

# Save application current directory and jump one level above scripts
CURRENT_DIRECTORY=$(pwd)
cd $(dirname "$0")/..

. scripts/set_project_env.sh

# Uncomment this line to set the COM port used by your board
# export ESPPORT=/dev/tty0

# Set the default value for 'ENV_FLASH_CMD' environment variable (default is to flash full features firmware without erasing it)
# You can add erase_flash instead of flash to fully erase the flash at each run (e.g: "export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make -j4 erase_flash ESPPORT=%ESPPORT%")
export ENV_FLASH_CMD_DEFAULT="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make -j4 flash ESPPORT=$ESPPORT"

if [ -z "$ESPPORT" ]; then
	echo "Please set the environment variable 'ESPPORT' to the COM port used to flash your ESP32"
    exit -1
fi


if [ -z "$ENV_FLASH_CMD" ]; then
	export ENV_FLASH_CMD=$ENV_FLASH_CMD_DEFAULT
    echo "Environment variable 'ENV_FLASH_CMD' used to flash your IDF application is set to default value ($ENV_FLASH_CMD_DEFAULT)"
else
	echo "Environment variable 'ENV_FLASH_CMD' used to flash your IDF application is set to $ENV_FLASH_CMD value"
fi

cp -v "${APPLICATION_FILE}" build/microej.elf

eval $ENV_FLASH_CMD

# Restore application directory
cd $CURRENT_DIRECTORY/
