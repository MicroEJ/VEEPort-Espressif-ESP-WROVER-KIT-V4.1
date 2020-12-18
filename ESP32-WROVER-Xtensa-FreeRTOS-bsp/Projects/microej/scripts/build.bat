@echo off
SETLOCAL ENABLEEXTENSIONS

REM BAT
REM Copyright 2020 MicroEJ Corp. All rights reserved.
REM This library is provided in source code for use, modification and test, subject to license terms.
REM Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

REM 'build.bat' implementation for Espressif IDF.

REM 'build.bat' is responsible for producing the executable file 
REM then copying this executable file to the current directory where it has been executed to a file named 'application.out'

REM Reset ERRORLEVEL between multiple run in the same shell
SET ERRORLEVEL=0

REM Save application current directory and jump one level above scripts
SET CURRENT_DIRECTORY=%CD%
CD "%~dp0\..\"

REM Set the default value for 'ENV_BASH_CMD' environment variable (default is to build a full features firmware) 
SET ENV_BASH_CMD_DEFAULT="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && echo IDF_PATH=${IDF_PATH} && make -j4 all"

CALL "scripts\set_project_env.bat"
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

IF [%ENV_BASH_CMD%] == [] (
    SET ENV_BASH_CMD=%ENV_BASH_CMD_DEFAULT%
    ECHO "Environment variable 'ENV_BASH_CMD' used to compile your IDF application is set to default value " %ENV_BASH_CMD_DEFAULT%
) ELSE ECHO Environment variable 'ENV_BASH_CMD' used to compile your IDF application is set to %ENV_BASH_CMD% value

REM Use sdkconfig.default
SET ENV_SDKCONFIG="sdkconfig.default"
REM If sdkconfig exists
IF EXIST "sdkconfig" (
   FC "sdkconfig" "sdkconfig.old" >NUL || (ECHO Backup sdkconfig to sdkconfig.old && COPY "sdkconfig" "sdkconfig.old")
   REM Then use ENV_SDKCONFIG if they differ
   FC "sdkconfig" %ENV_SDKCONFIG% >NUL && (ECHO File are the same, don't update sdkconfig) || (ECHO Copy %ENV_SDKCONFIG% to sdkconfig && COPY /Y %ENV_SDKCONFIG% "sdkconfig" && COPY /B "sdkconfig" +,,)
) ELSE (
   REM sdkconfig doesn't exist, use ENV_SDKCONFIG
   ECHO Copy %ENV_SDKCONFIG% to sdkconfig
   COPY %ENV_SDKCONFIG% "sdkconfig"
)

bash.exe -c %ENV_BASH_CMD% && COPY "build\microej.elf" "%CURRENT_DIRECTORY%\application.out" && (
	SET ERRORLEVEL=0
	) || (
	SET ERRORLEVEL=1
)

REM Restore application directory
CD "%CURRENT_DIRECTORY%"

IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

