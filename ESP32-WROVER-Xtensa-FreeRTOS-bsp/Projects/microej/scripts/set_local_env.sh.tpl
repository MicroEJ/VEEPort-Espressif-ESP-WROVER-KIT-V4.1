# BASH
# Copyright 2020 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

# Set the environment variable 'XTENSA_PATH' to your Espressif toolchain xtensa installation directory or install Espressif toolchain at the recommanded location ('$XTENSA_PATH_DEFAULT', see https://docs.espressif.com/projects/esp-idf/en/v3.3.4/get-started/linux-setup.html#toolchain-setup)
export XTENSA_PATH="$HOME/esp/xtensa-esp32-elf/"

# Set the COM port used by your board
ESPPORT="/dev/ttyS0"

# Set the default value for 'ENV_BASH_CMD' environment variable (default is to build a full features firmware)
export ENV_BASH_CMD="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make -j4 all"

# Set the default value for 'ENV_FLASH_CMD' environment variable (default is to flash full features firmware without erasing it)
# You can add erase_flash instead of flash to fully erase the flash at each run (e.g: "export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make -j4 erase_flash ESPPORT=%ESPPORT%")
export ENV_FLASH_CMD="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make -j4 flash ESPPORT=$ESPPORT"
