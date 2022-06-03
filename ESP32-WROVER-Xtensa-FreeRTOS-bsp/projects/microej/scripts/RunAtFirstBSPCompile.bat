@echo off
SETLOCAL ENABLEEXTENSIONS

REM BAT
REM Copyright 2020-2022 MicroEJ Corp. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be found with this software.

REM Init. the echosystem afther the repository cloning of the working branch.
REM This will update submodules (esp-idf) to the version 4.3 and will apply patch to 
REM to include Segger System View, MicroEJ implementation.

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

REM Set MicroEJ top level folder.
CD "..\..\.."

REM Copy the patch file to the submodule directory (esp-idf)
COPY /Y /Z /B "projects\microej\trace\systemview\SYSVIEW-MicroEJ.patch" "%ESP_IDF_PATH%\SYSVIEW-MicroEJ.patch"
IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%
)

REM Apply patch
CD %ESP_IDF_PATH%

CMD /C git apply SYSVIEW-MicroEJ.patch

REM Delete patch file.
DEL  /F /Q SYSVIEW-MicroEJ.patch

IF %ERRORLEVEL% NEQ 0 (
	EXIT /B %ERRORLEVEL%

REM Restore current directory
CD %CURRENT_DIR%

)

exit /B 0
