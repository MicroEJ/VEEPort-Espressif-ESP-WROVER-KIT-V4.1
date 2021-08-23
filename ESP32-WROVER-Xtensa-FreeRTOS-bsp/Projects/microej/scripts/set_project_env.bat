@echo off

REM BAT
REM Copyright 2020-2021 MicroEJ Corp. All rights reserved.
REM This library is provided in source code for use, modification and test, subject to license terms.
REM Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

REM 'set_project_env.bat' implementation for Espressif IDF.

REM 'set_project_env' is responsible for
REM - checking the availability of required environment variables 
REM - setting project local variables for 'build.bat' and 'run.bat' 

REM Set Espressif toolchain installation directory default value recommanded by Espressif (see https://docs.espressif.com/projects/esp-idf/en/v3.3.4/get-started/windows-setup.html#toolchain-setup)
SET MSYS_PATH_DEFAULT=C:\msys32

REM Load local settings if the file exists.  See set_local_env.bat.tpl for an example.
SET LOCAL_ENV_SCRIPT="scripts\set_local_env.bat"
IF EXIST "%LOCAL_ENV_SCRIPT%" (
   ECHO "Load %LOCAL_ENV_SCRIPT%"
   CALL "%LOCAL_ENV_SCRIPT%"
   IF %ERRORLEVEL% NEQ 0 (
      exit /B %ERRORLEVEL%
   )
)

IF [%MSYS_PATH%] == [] (
	REM If user do not provide a specific Espressif toolchain msys path, set 'MSYS_PATH' to the default value
	ECHO "Environment variable 'MSYS_PATH' used to define Espressif msys toolchain installation directory is set to default value ('%MSYS_PATH_DEFAULT%')"
    SET MSYS_PATH=%MSYS_PATH_DEFAULT%
) ELSE ECHO Environment variable 'MSYS_PATH' used to define Espressif msys toolchain installation directory is set to "%MSYS_PATH%" value

IF NOT EXIST %MSYS_PATH% (
	ECHO "Please set the environment variable 'MSYS_PATH' to your Espressif toolchain msys installation directory or install Espressif toolchain at the recommanded location ('%MSYS_PATH_DEFAULT%', see https://docs.espressif.com/projects/esp-idf/en/v3.3.4/get-started/windows-setup.html#toolchain-setup)"
	exit /B -1
) 

SET Path=%MSYS_PATH%\usr\bin;%MSYS_PATH%\mingw32\bin;%MSYS_PATH%\opt\xtensa-esp32-elf\bin;%Path%

REM Set offsets for combined binary
SET BOOTLOADER_BIN_OFFSET=0x1000
SET PARTITIONS_BIN_OFFSET=0x8000
SET APPLICATION_BIN_OFFSET=0x10000
