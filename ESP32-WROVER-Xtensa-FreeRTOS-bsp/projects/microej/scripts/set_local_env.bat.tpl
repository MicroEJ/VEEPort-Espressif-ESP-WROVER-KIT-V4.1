@echo off

REM BAT
REM Copyright 2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM Set IDF tools path.
SET IDF_TOOLS_PATH=%USERPROFILE%\.espressif

REM Set serial port. Set a value (e.g. COM4).
SET ESPPORT=COM4
ECHO ESPPORT=%ESPPORT%

REM Set build directory.
SET ESP_BUILD_DIR="%CD%\projects\microej\build"
REM ECHO ESP_BUILD_DIR=%ESP_BUILD_DIR%