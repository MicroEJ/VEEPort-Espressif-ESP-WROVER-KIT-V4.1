#!/bin/bash
#
# BASH
#
# Copyright 2020-2021 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

# 'set_project_env.sh' implementation for Espressif IDF.

# 'set_project_env' is responsible for
# - checking the availability of required environment variables 
# - setting project local variables for 'build.sh' and 'run.sh' 

# Set Espressif toolchain installation directory default value recommanded by Espressif (see https://docs.espressif.com/projects/esp-idf/en/v3.3.4/get-started/windows-setup.html#toolchain-setup)
export XTENSA_PATH_DEFAULT="$HOME/esp/xtensa-esp32-elf/"

LOCAL_ENV_SCRIPT="scripts/set_local_env.sh"
if [[ -e "$LOCAL_ENV_SCRIPT" ]]; then
    echo "Load $LOCAL_ENV_SCRIPT"
    . "$LOCAL_ENV_SCRIPT"
fi

if [ -z "$XTENSA_PATH" ]; then
	# If user do not provide a specific Espressif toolchain xtensa path, set 'XTENSA_PATH' to the default value
	echo "Environment variable 'XTENSA_PATH' used to define Espressif xtensa toolchain installation directory is set to default value ('$XTENSA_PATH_DEFAULT')"
    export XTENSA_PATH=$XTENSA_PATH_DEFAULT
else
	echo "Environment variable 'XTENSA_PATH' used to define Espressif xtensa toolchain installation directory is set to '$XTENSA_PATH' value"
fi

if [ ! -d "$XTENSA_PATH" ]; then
	echo "Please set the environment variable 'XTENSA_PATH' to your Espressif toolchain xtensa installation directory or install Espressif toolchain at the recommanded location ('$XTENSA_PATH_DEFAULT', see https://docs.espressif.com/projects/esp-idf/en/v3.3.4/get-started/linux-setup.html#toolchain-setup)"
    exit
fi

# Add xtensa tool chain to path
export PATH=$XTENSA_PATH/bin:${PATH}

# Set offsets for combined binary
BOOTLOADER_BIN_OFFSET=0x1000
PARTITIONS_BIN_OFFSET=0x8000
APPLICATION_BIN_OFFSET=0x10000
