#!/bin/sh
#
# bash
#
# Copyright 2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

# Init. the echosystem afther the repository cloning of the working branch.
# This will update submodules (esp-idf) to the version 4.3 and will apply patch to 
# to include Segger System View, MicroEJ implementation.

# Get current directory
CURRENT_DIR=$(pwd)
echo CURRENT_DIR=$CURRENT_DIR
cd $(dirname $(realpath $0))

. "./set_project_env.sh"

# Set MicroEJ top level folder.
cd ../../..

# Copy the patch file to the submodule directory (esp-idf)
cp projects/microej/trace/systemview/SYSVIEW-MicroEJ.patch $IDF_PATH/SYSVIEW-MicroEJ.patch

# Apply patch
cd $IDF_PATH

git apply SYSVIEW-MicroEJ.patch

# Delete patch file.
rm SYSVIEW-MicroEJ.patch

# Restore current directory
cd $CURRENT_DIR
