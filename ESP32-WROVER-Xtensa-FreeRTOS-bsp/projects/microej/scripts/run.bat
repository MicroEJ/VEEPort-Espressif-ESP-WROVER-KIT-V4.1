@echo off
SETLOCAL ENABLEEXTENSIONS

REM BAT
REM Copyright 2020-2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM 'run.bat' implementation for Espressif IDF.

REM 'run.bat' is responsible for flashing the executable file on the target device 
REM then resetting target device. A valid serial port in the form of (COMx) must be
REM set previously in the 'set_local_env.bat' file.

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
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

CD "%ESP_PROJECT_DIR%"
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

python "%ESP_IDF_PATH%\tools\idf.py" -B "%ESP_BUILD_DIR%" -p "%ESPPORT%" flash && (
	SET ERRORLEVEL=0
	) || (
	SET ERRORLEVEL=1
)

REM Restore current directory
CD "%CURRENT_DIR%"

IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

REM Clean exit.
