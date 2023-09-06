.. 
	Copyright 2019-2023 MicroEJ Corp. All rights reserved.
	Use of this source code is governed by a BSD-style license that can be found with this software.

.. |BOARD_NAME| replace:: ESP-WROVER-KIT V4.1
.. |BOARD_REVISION| replace:: 4.1
.. |PLATFORM_VER| replace:: 2.2.1
.. |RCP| replace:: MICROEJ SDK
.. |PLATFORM| replace:: MicroEJ Platform
.. |PLATFORMS| replace:: MicroEJ Platforms
.. |SIM| replace:: MicroEJ Simulator
.. |ARCH| replace:: MicroEJ Architecture
.. |CIDE| replace:: MICROEJ SDK
.. |RTOS| replace:: FreeRTOS RTOS
.. |MANUFACTURER| replace:: Espressif

.. _中文版: ./docs/zn_CH/README_CN.rst
.. _README: ./../../../README.rst
.. _RELEASE NOTES: ./../../../RELEASE_NOTES.rst
.. _CHANGELOG: ./../../../CHANGELOG.rst
.. _MicroEJ SystemView README: ./trace/systemview/README.rst 

================
|BOARD_NAME| BSP
================

`中文版`_

This project contains the BSP sources of the |PLATFORM| for the
|BOARD_NAME|.  The BSP project is based on Espressif IoT Development
Framework (ESP-IDF).

This document does not describe how to setup the |PLATFORM|.  Please
refer to the `README`_ for that.

Scripts
=======

In the folder ``projects/microej/scripts/`` are scripts that can be
used to update, build, flash and debug the BSP.  The ``.bat`` and ``.sh`` 
scripts are meant to run in a Windows and Linux environment respectively.

- The **set_project_env** script is responsible for setting the following variables:
  
  - *ESPPORT* The value of the COM port must be set by the user (e.g. `=COM6`
    or `==/dev/ttyS0`)
  - *IDF_TOOLS_PATH* The value of this environment variable is set by the Espressif 
    toolchain installation. It must be defined and pointing to the Xtensa toolchain 
    installation for the build/flash proccess to work.
  - *ESP_PROJECT_DIR* The value is set by the script. Represents the path to 
    the project directory (`\projects\microej`)
  - *ESP_BUILD_DIR* The value is set by the script. Represents the path to the 
    build directory (`\projects\microej\build`).
  - *ESP_IDF_PATH* The value is set by the script. Represents the path to the 
    Espressif software components directory (`\sdk\esp-idf`)

- The **RunAtFirstBSPCompile** script is used to apply MicroEJ system view implementation
  patch. 

  Basically, the patch file *SYSVIEW-MicroEJ.patch*, from the 
  ``projects/microej/trace/systemview/`` will be applied to the Espressif software
  components directory (`\sdk\esp-idf`). This will add the MicroEJ custom 
  implementation for the Segger System View tracing system.
  
  This steps can be, also,  manually done be the user, and are needed only before 
  first BSP compilation.

- The **build** script is used to compile and link the BSP with a MicroEJ 
  application to produce a MicroEJ Firmware (``application.out``) that can be
  flashed on a device.

  This script use a configuration for the Mono-Sandbox firmware, uses the 
  *sdkconfig_ota_systemview* to create the *sdkconfig* configuration file.

- The **run** script is used to flash a MicroEJ Firmware (``application.out``)
  on a device. The variable *ESPPORT* must be set in the **set_project_env**
  for the script to work properly.

- The **build_no_ota_no_systemview** script is used to compile and link the BSP 
  with a MicroEJ application to produce a MicroEJ Firmware (``application.out``)
  that can be flashed on a device.

  This script use a configuration for the Multi-Sandbox firmware, uses the 
  *sdkconfig_no_ota_no_systemview* to create the *sdkconfig* configuration file.
  The SystemView tracing implementation is disabled and also the output is
  configured as a single factory application.

- The **SysView** script is used to start the application, tracing and logging
  to files, (start the Segger SystemView tracing). More information about this feature
  can be found on `MicroEJ SystemView README`_ and also on the `Espressif official 
  documentation <https://docs.espressif.com/projects/esp-idf/en/v4.3.2/esp32/api-guides/app_trace.html?highlight=segger#app-trace-system-behaviour-analysis-with-segger-systemview>`_.

Debugging with a |BOARD_NAME|
=============================

The entire process of debugging a |BOARD_NAME| is described `here <https://docs.espressif.com/projects/esp-idf/en/v4.3.2/esp32/api-guides/jtag-debugging/using-debugger.html>`_. 

Using Segger SystemView
=======================

The entire process is described in `MicroEJ SystemView README`_
Using this feature is out of the box for the Mono-Sandbox configuration (created using the
**build** script). 

Convert a PEM private key to DER
================================

MicroEJ requires private keys in DER format. To convert your PEM
private keys use the following command:

.. code-block:: console

	openssl.exe pkcs8 -inform PEM -in key1.pem -topk8 -outform DER -out key1.der -v1 PBE-SHA1-3DES -passout pass:<my_password>

Convert a backtrace without make monitor
========================================

The following command:

.. code-block:: console

	xtensa-esp32-elf-addr2line.exe -e build/microej.elf Backtrace:0x40081afb:0x3ffba7f0 0x4008d335:0x3ffba810 0x40092cae:0x3ffba830 0x4008bb0f:0x3ffba8a0

Should output the corresponding instructions at the addresses given as
the last parameters.

File System
===========

SD Card (default)
-----------------

By default the file system is set to be in the SD card.
The initialization function for the MicroEJ FS Foundation Library in this case can be found here: ``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\fs\src\LLFS_ESP32_init_sdcard.c``.

SPI Flash
---------

Instead of using the SD card it is also possible to use the flash for the file system.
In this case the initialization function is implemented here: ``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\fs\src\LLFS_ESP32_init_spiflash.c``
To use this function at run time, enable via the FS configuration by commenting the line ``#define LLFS_ESP32_USE_SD_CARD`` in the following file:
``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\fs\inc\fs_configuration.h``.
A flash partition also need to be defined in the partition table. This can be done by adding this line:
``storage,	data,	fat,	,	1M,``.
To the CSV file: ``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\partitions_ota_systemview.csv``.
It is also required to remove or resize at least one other partition, otherwise it will not fit on the |BOARD_NAME| flash.


