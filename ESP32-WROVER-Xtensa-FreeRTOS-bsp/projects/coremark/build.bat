@echo off

SETLOCAL ENABLEEXTENSIONS

REM BAT
REM Copyright 2020-2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM 'build.bat' implementation for Espressif IDF - coremark test. It is responsible for producing the executable file.

REM Reset ERRORLEVEL between multiple run in the same shell
SET ERRORLEVEL=0

REM Check if the project environment variables are set
IF "%IDF_TOOLS_PATH%" == "" (
    ECHO Please set the environment variable 'IDF_TOOLS_PATH'
    exit /B -1
   )

IF "%IDF_PATH%" == "" (
    ECHO Please set the environment variable 'IDF_PATH'
    exit /B -1
)

CALL "%IDF_PATH%\export.bat"

REM Set build directory.
IF [%ESP_IDF_BUILD_DIR%] == [] (
	SET ESP_IDF_BUILD_DIR=%CD%\build
)

REM Build the project.
python "%IDF_PATH%\tools\idf.py" -B "%ESP_IDF_BUILD_DIR%" all 


