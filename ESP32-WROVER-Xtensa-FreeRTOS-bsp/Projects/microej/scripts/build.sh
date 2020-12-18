#!/bin/bash
#
# BASH
#
# Copyright 2020 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

# 'build.sh' implementation for Espressif IDF.

# 'build.sh' is responsible for producing the executable file 
# then copying this executable file to the current directory where it has been executed to a file named 'application.out'

# Save application current directory and jump one level above scripts
CURRENT_DIRECTORY=$(pwd)
cd $(dirname "$0")/..

# Set the default value for 'ENV_BASH_CMD' environment variable (default is to build a full features firmware)
export ENV_BASH_CMD_DEFAULT="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && echo IDF_PATH=${IDF_PATH} && make -j4 all"

. "scripts/set_project_env.sh"

if [ -z "$ENV_BASH_CMD" ]; then
    export ENV_BASH_CMD=$ENV_BASH_CMD_DEFAULT
    echo "Environment variable 'ENV_BASH_CMD' used to compile your IDF application is set to default value ($ENV_BASH_CMD_DEFAULT)"
else
	echo "Environment variable 'ENV_BASH_CMD' used to compile your IDF application is set to $ENV_BASH_CMD value"
fi

# Use sdkconfig.default
ENV_SDKCONFIG="sdkconfig.default"
# If sdkconfig exists
if [ -e "sdkconfig" ]; then
    cmp -s "sdkconfig" "sdkconfig.old" || (echo "Backup sdkconfig to sdkconfig.old" ; cp "sdkconfig" "sdkconfig.old")
    # Then use ENV_SDKCONFIG if they differ
    cmp -s "sdkconfig" "$ENV_SDKCONFIG" && (echo "File are the same, don't update sdkconfig") || (echo "Copy $ENV_SDKCONFIG to sdkconfig" ; cp "$ENV_SDKCONFIG" "sdkconfig")
else
    echo "Copy $ENV_SDKCONFIG to sdkconfig"
    cp "$ENV_SDKCONFIG" "sdkconfig"
fi

eval $ENV_BASH_CMD
cp build/microej.elf $CURRENT_DIRECTORY/application.out

# Restore application directory
cd $CURRENT_DIRECTORY/
