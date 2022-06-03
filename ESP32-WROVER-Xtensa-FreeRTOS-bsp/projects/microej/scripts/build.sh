#!/bin/bash
#
# BASH
#
# Copyright 2020-2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

# 'build.sh' implementation for Espressif IDF.

# 'build.sh' is responsible for producing the executable file 
# then copying this executable file to the current directory where it has been executed to a file named 'application.out'

# Save application current directory and jump one level above scripts
CURRENT_DIRECTORY=$(pwd)
cd $(dirname "$0")

. "set_project_env.sh"

# Apply patch if variable $APPLY_FIRST_RUN_PATCHS is defined
if [ -n "$APPLY_FIRST_BSP_COMPILE_HOOKS" ]; then
  echo "Running first build bsp hooks"
  bash RunAtFirstBSPCompile.sh
fi

cd $ESP_PROJECT_DIR

# Use sdkconfig.default
ENV_SDKCONFIG="sdkconfig_ota_systemview"
# If sdkconfig exists
if [ -e "sdkconfig" ]; then
    cmp -s "sdkconfig" "sdkconfig.backup" || (echo "Backup sdkconfig to sdkconfig.backup" ; cp "sdkconfig" "sdkconfig.backup")
    # Then use ENV_SDKCONFIG if they differ
    cmp -s "sdkconfig" "$ENV_SDKCONFIG" && (echo "$ENV_SDKCONFIG already installed as sdkconfig") || (echo "Copy $ENV_SDKCONFIG to sdkconfig" ; cp "$ENV_SDKCONFIG" "sdkconfig")
else
    echo "Copy $ENV_SDKCONFIG to sdkconfig"
    cp "$ENV_SDKCONFIG" "sdkconfig"
fi

. $IDF_PATH/export.sh
idf.py -B $ESP_BUILD_DIR all && cp $ESP_BUILD_DIR/microej.elf $CURRENT_DIRECTORY/application.out
if [ $? -ne 0 ]; then
  exit 1
fi

# Generate combined binary
python "$ESP_PROJECT_DIR/scripts/combine_binaries.py" "$CURRENT_DIRECTORY/combined.bin" \
	$BOOTLOADER_BIN_OFFSET "$ESP_BUILD_DIR/bootloader/bootloader.bin" \
	$PARTITIONS_BIN_OFFSET "$ESP_BUILD_DIR/partition_table/partition-table.bin" \
	$APPLICATION_BIN_OFFSET "$ESP_BUILD_DIR/microej.bin"
if [ $? -ne 0 ]; then
  exit 1
fi

# Restore application directory
cd $CURRENT_DIRECTORY/
