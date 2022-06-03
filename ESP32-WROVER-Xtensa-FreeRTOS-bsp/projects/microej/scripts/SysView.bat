@echo off
SETLOCAL ENABLEEXTENSIONS

REM BAT
REM Copyright 2020-2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM 'SysView.bat' implementation for Espressif IDF.

REM 'SysView.bat' is responsible for generating the Segger System View files.
REM It automatically starts the OpenOCD session, connects a gdb debbugger client
REM and starts logging the sysview events on two separate files.

REM Reset ERRORLEVEL between multiple run in the same shell
SET ERRORLEVEL=0

REM Get current directory
SET CURRENT_DIR=%CD%
ECHO CURRENT_DIR=%CURRENT_DIR%
CD "%~dp0"

REM Check if the project environment variables are set
CALL set_project_env.bat
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

CALL %ESP_IDF_PATH%\export.bat

REM Save the log files to the scripts directory
CD "%ESP_PROJECT_DIR%\scripts"
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

REM Start the OpenOCD server
start /min cmd.exe /k "openocd -f board/esp32-wrover-kit-3.3v.cfg"
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

REM start the gdb client
cmd.exe /k xtensa-esp32-elf-gdb -x gdbinit %ESP_BUILD_DIR%\microej.elf

REM Restore current directory
CD %CURRENT_DIR%

IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

