@echo off

REM BAT
REM Copyright 2020-2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM 'run.bat' implementation for Espressif IDF.

REM 'run.bat' is responsible for flashing the executable file on the target device 
REM then resetting target device

REM Reset ERRORLEVEL between multiple run in the same shell
SET ERRORLEVEL=0

REM Uncomment this line to set the COM port used by your board
SET ESPPORT=COM4

REM Check if the project environment variables are set
IF "%IDF_TOOLS_PATH%" == "" (
    ECHO Please set the environment variable 'IDF_TOOLS_PATH'
    exit /B -1
   )

IF "%IDF_PATH%" == "" (
    ECHO Please set the environment variable 'IDF_PATH'
    exit /B -1
)

IF [%ESPPORT%] == [] (
	ECHO Please set the environment variable 'ESPPORT' to the COM port used to flash your ESP32
	EXIT /B -1
)

CALL "%IDF_PATH%\export.bat"

REM Set build directory.
IF [%ESP_IDF_BUILD_DIR%] == [] (
	SET ESP_IDF_BUILD_DIR=%CD%\build
)

REM Rebuild and flash the programm.
python "%IDF_PATH%\tools\idf.py" -B "%ESP_IDF_BUILD_DIR%" -p "%ESPPORT%" flash && (
	SET ERRORLEVEL=0
	) || (
	SET ERRORLEVEL=1
)

IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

