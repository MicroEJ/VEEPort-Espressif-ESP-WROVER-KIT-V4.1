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
cd $(dirname "$0")/..

# Set the default value for 'ENV_BASH_CMD' environment variable (default is to build a full features firmware)
export ENV_BASH_CMD_DEFAULT="export IDF_PATH=$(pwd)/../../sdk/esp-idf && echo IDF_PATH=${IDF_PATH} && make -j4 all"

. "scripts/set_project_env.sh"

if [ -z "$ENV_BASH_CMD" ]; then
	export ENV_BASH_CMD=$ENV_BASH_CMD_DEFAULT
    echo "Environment variable 'ENV_BASH_CMD' used to compile your IDF application is set to default value ($ENV_BASH_CMD_DEFAULT)"
else
	echo "Environment variable 'ENV_BASH_CMD' used to compile your IDF application is set to $ENV_BASH_CMD value"
fi

# Use sdkconfig_no_ota_no_systemview
ENV_SDKCONFIG="sdkconfig_no_ota_no_systemview"
# If sdkconfig exists
if [ -e "sdkconfig" ]; then
    cmp -s "sdkconfig" "sdkconfig.old" || (echo "Backup sdkconfig to sdkconfig.old" ; cp "sdkconfig" "sdkconfig.old")
    # Then use ENV_SDKCONFIG if they differ
    cmp -s "sdkconfig" "$ENV_SDKCONFIG" && (echo "$ENV_SDKCONFIG already installed as sdkconfig") || (echo "Copy $ENV_SDKCONFIG to sdkconfig" ; cp "$ENV_SDKCONFIG" "sdkconfig")
else
    echo "Copy $ENV_SDKCONFIG to sdkconfig"
    cp "$ENV_SDKCONFIG" "sdkconfig"
fi

eval $ENV_BASH_CMD
cp build/microej.elf $CURRENT_DIRECTORY/application.out

# Generate combined binary
python "scripts/combine_binaries.py" "$CURRENT_DIRECTORY/combined.bin" \
	$BOOTLOADER_BIN_OFFSET "build/bootloader/bootloader.bin" \
	$PARTITIONS_BIN_OFFSET "build/partitions_no_ota_no_systemview.bin" \
	$APPLICATION_BIN_OFFSET "build/microej.bin"

# Restore application directory
cd $CURRENT_DIRECTORY/
