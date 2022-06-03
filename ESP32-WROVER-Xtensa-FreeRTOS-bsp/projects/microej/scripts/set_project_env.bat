@echo off

REM BAT
REM Copyright 2020-2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM 'set_project_env.bat' implementation for Espressif IDF.

REM 'set_project_env' is responsible for
REM - checking the availability of required environment variables 
REM - setting project local variables for 'build.bat', 'run.bat' and 'SysView.bat'.

REM Get current directory
SET CURRENT_DIRECTORY=%CD%
REM ECHO CURRENT_DIRECTORY=%CURRENT_DIRECTORY%

REM Set MicroEJ top level folder.
CD "%~dp0..\..\.."
SET ESP_PROJECT_DIR="%CD%\projects\microej"

REM Set build directory.
SET ESP_BUILD_DIR="%CD%\projects\microej\build"

IF "%ESP_IDF_PATH%"=="" (
    REM Set "drivers" directory (esp-idf)
    SET ESP_IDF_PATH="%CD%\sdk\esp-idf"
)

REM Load local settings if the file exists.  See set_local_env.bat.tpl for an example.
SET LOCAL_ENV_SCRIPT="%ESP_PROJECT_DIR%\scripts\set_local_env.bat"
IF EXIST "%LOCAL_ENV_SCRIPT%" (
   ECHO Load %LOCAL_ENV_SCRIPT%
   CALL "%LOCAL_ENV_SCRIPT%"
   IF %ERRORLEVEL% NEQ 0 (
      exit /B %ERRORLEVEL%
   )
)

ECHO ESP_PROJECT_DIR=%ESP_PROJECT_DIR%
ECHO ESP_BUILD_DIR=%ESP_BUILD_DIR%
ECHO ESP_IDF_PATH=%ESP_IDF_PATH%

IF "%IDF_TOOLS_PATH%" == "" (
    ECHO Please set the environment variable 'IDF_TOOLS_PATH'
    exit /B -1
   )

IF "%ESPPORT%" == "" (
    ECHO Please set the environment variable 'ESPPORT'
	exit /B -1
)

CD %IDF_TOOLS_PATH%
CALL install.bat

REM Restore current directory
CD "%CURRENT_DIRECTORY%"

exit /B 0
