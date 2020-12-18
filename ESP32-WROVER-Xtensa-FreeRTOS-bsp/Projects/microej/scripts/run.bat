@echo off

REM BAT
REM Copyright 2020 MicroEJ Corp. All rights reserved.
REM This library is provided in source code for use, modification and test, subject to license terms.
REM Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

REM 'run.bat' implementation for Espressif IDF.

REM 'run.bat' is responsible for flashing the executable file on the target device 
REM then resetting target device

REM Reset ERRORLEVEL between multiple run in the same shell
SET ERRORLEVEL=0

REM Save application current directory and jump one level above scripts
SET CURRENT_DIRECTORY=%CD%
CD "%~dp0\..\"

REM Uncomment this line to set the COM port used by your board
REM SET ESPPORT=COM0

REM Set the default value for 'ENV_FLASH_CMD' environment variable (default is to flash full features firmware without erasing it)
REM You can add erase_flash instead of flash to fully erase the flash at each run (e.g: "IDF_PATH=$(pwd)/../../Drivers/esp-idf && make -j4 erase_flash ESPPORT=%ESPPORT%")
SET ENV_FLASH_CMD_DEFAULT="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make -j4 flash ESPPORT=$ESPPORT"

CALL "scripts\set_project_env.bat"
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

IF [%ESPPORT%] == [] (
	ECHO Please set the environment variable 'ESPPORT' to the COM port used to flash your ESP32
	EXIT /B -1
)

IF [%ENV_FLASH_CMD%] == [] (
    SET ENV_FLASH_CMD=%ENV_FLASH_CMD_DEFAULT%
    ECHO "Environment variable 'ENV_FLASH_CMD' used to flash your IDF application is set to default value " %ENV_FLASH_CMD_DEFAULT%
) ELSE ECHO Environment variable 'ENV_FLASH_CMD' used to flash your IDF application is set to %ENV_FLASH_CMD% value

bash.exe -c %ENV_FLASH_CMD%

REM Restore application directory
CD "%CURRENT_DIRECTORY%"

IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

