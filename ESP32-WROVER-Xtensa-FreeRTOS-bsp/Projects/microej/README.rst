.. 
	Copyright 2019-2021 MicroEJ Corp. All rights reserved.
	This library is provided in source code for use, modification and test, subject to license terms.
	Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

.. |BOARD_NAME| replace:: ESP-WROVER-KIT V4.1
.. |BOARD_REVISION| replace:: 4.1
.. |PLATFORM_VER| replace:: 1.8.2
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

================
|BOARD_NAME| BSP
================

`中文版`_

This project contains the BSP sources of the |PLATFORM| for the
|BOARD_NAME|.  The BSP project is based on Espressif IoT Development
Framework (ESP-IDF).

This document does not describe how to setup the |PLATFORM|.  Please
refer to the `README`_ for that.

Build & Flash Scripts
---------------------

In the folder ``Project/microej/scripts/`` are scripts that can be
used to build and flash the BSP.  The ``.bat`` and ``.sh`` scripts are
meant to run in a Windows and Linux environment respectively.

- The ``build*`` scripts are used to compile and link the BSP with a
  MicroEJ Application to produce a MicroEJ Firmware
  (``application.out``) that can be flashed on a device.

  The ``build*`` scripts work out of the box, assuming the toolchain is
  installed in the default path.

- The ``run*`` scripts are used to flash a MicroEJ Firmware
  (``application.out``) on a device.

  The COM port of the device should be configured for the ``run*``
  scripts to work.

The following environment variables are customizable:  

- ``MSYS_PATH``: Path to the msys32 installation (Windows).
- ``XTENSA_PATH``: Path to the Xtensa toolchain installation (Linux).
- ``ENV_BASH_CMD``: Command line to compile the BSP.
- ``ENV_FLASH_CMD``: Command line to flash and execute the firmware
  produced by the build script.
- ``ESPPORT``: Serial port connected to the device.

See the comments for each variables in the scripts for a detailed
explanation.

As documented by Espressif, the build scripts expects the toolchain to
be installed in ``C:\msys32`` (Windows) or ``~/esp/xtensa-esp32-elf/``
(Linux).  If the toolchain is installed elsewhere, update the
environment variables accordingly.

See this `section of Espressif documentation
<https://docs.espressif.com/projects/esp-idf/en/v3.3.4/get-started/index.html#environment-variables>`__
for other possible variables.

The environment variables can be defined globally by the user or in
the ``set_local_env*`` scripts.  When the ``.bat`` (``.sh``) scripts
are executed, the ``set_local_env.bat`` (``set_local_env.sh``) script
is executed if it exists.  Create and configure these files to
customize the environment locally.  Template files are provided as
example, see ``set_local_env.bat.tpl`` and ``set_local_env.sh.tpl``.

Customize SDKCONFIG
-------------------

To change the ESP-IDF BSP configuration, update the SDKCONFIG
depending on the firmware build by the platform:

- ``sdkconfig.default`` for a Mono-Sandbox Firmware
- ``sdkconfig_no_ota_no_systemview`` for a Multi-Sandbox Firmware

To change the SDKCONFIG, either:

- edit the file with your favorite editor
- use menuconfig by updating the ``ENV_BASH_CMD`` variable to invoke ``menuconfig`` and then run the build script.

  For example on Windows for a Mono-Sandbox Firmware:

  - ``SET ENV_BASH_CMD="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make menuconfig"``
  - ``build.bat``

Then copy the file ``sdkconfig`` back to ``sdkconfig.default`` for a
Mono-Sandbox Firmware (``sdkconfig_no_ota_no_systemview`` for a
Multi-Sandbox Firmware).

Flash the board with the Espressif flash tool
---------------------------------------------

The ``run*`` scripts can also be used to flash the device with the
MicroEJ Firmware.

- Start ESP32 Flash Download Tool (available `here <https://www.espressif.com/en/support/download/other-tools>`__)
- Click on ESP32 DownloadTool button
- Select the tab SPI Download
- Fill the first blank line as following:
  - Select the check box on the left
  - Click on ... and browse the application.bin file
  - Type the value 0 into the address text field on the right (after the @ symbol)
- Set COM to the previously mounted COM port
- Set Baud to 921600
- Click on START

The flashing process starts with a green progress bar. Wait until the message FINISH is shown.

Debugging with a WROVER dev kit
-------------------------------

**Setup and run OpenOCD GDB server**:

1. Go to Espressif esp-idf "Setup of OpenOCD" page (available `here <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/index.html#jtag-debugging-setup-openocd>`_)
2. Follow Espressif esp-idf instructions to install OpenOCD software on your machine
3. Follow Espressif esp-idf instructions to configure ESP32-WROVER Kit v4.1 board (available `here <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/configure-wrover.html#configure-usb-drivers>`_)
4. Follow Espressif esp-idf instructions to run OpenOCD (available `here <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/index.html#run-openocd>`_).  On Windows, the OpenOCD can be executed in MingGW (``mingw32.exe``) or in Windows Command Prompt (use ``.\bin\openocd ...``). The configuration file is ``esp32-wrover-kit-1.8v.cfg``.

Your OpenOCD GDB server is now ready to launch a GDB client debug session.

Debugging with OpenOCD works only if the SD Card interface is not
used.  Please refer to the `RELEASE NOTES`_ limitations section for more
details

**Launch a GDB debug session**:

1. Ensure that OpenOCD GDB server is running. If not, please refers to the previous paragraph to run OpenOCD GDB server.
2. In MicroEJ SDK, go to ``Run > Debug Configurations...``. Click on the ``Filter launch configuration...`` (upper left corner icon) and uncheck ``Filter Configuration Types``. Then close the ``Debug Configurations`` window. This action will allow you to see previously filtered Eclipse Configuration Types (e.g: ``GDB Hardware Debugging``).
3. Go to Espressif esp-idf "Launch Debugger" page (available `here <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/index.html#launching-debugger>`_)
4. Follow Espressif esp-idf instructions to create an Eclipse ``GDB Hardware Debugging`` launcher and run it (available `here <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/using-debugger.html#eclipse>`_). Take into account that, when you will create the Eclipse ``GDB Hardware Debugging`` launcher, ``ESP32-WROVER-Xtensa-FreeRTOS-bsp`` project will not be listed in the projects list available when clicking on ``Project > Browse...`` button. This behavior is due to the fact that ``ESP32-WROVER-Xtensa-FreeRTOS-bsp`` is not an Eclipse CDT project. Consequently, please enter ``ESP32-WROVER-Xtensa-FreeRTOS-bsp`` in ``Project:`` text field and put the absolute path of the Elf executable to debug in ``C/C++ Application:`` text field (e.g: ``[WORKSPACE_LOCATION]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Projects/microej/build/microej.elf``).

**Compile the application with debug symbols**:

Edit the ``sdkconfig.default`` (``sdkconfig_no_ota_no_systemview`` for Multi-Sandbox Firmware):

- Set ``CONFIG_OPTIMIZATION_LEVEL_DEBUG=y``
- Set ``CONFIG_OPTIMIZATION_LEVEL_RELEASE=``
- Set ``CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER=y`` (add it if it doesn't exist).

Convert a PEM private key to DER
--------------------------------

MicroEJ requires private keys in DER format. To convert your PEM
private keys use the following command:

.. code-block:: console

	openssl.exe pkcs8 -inform PEM -in key1.pem -topk8 -outform DER -out key1.der -v1 PBE-SHA1-3DES -passout pass:<my_password>

Convert a backtrace without make monitor
----------------------------------------

The following command:

.. code-block:: console

	xtensa-esp32-elf-addr2line.exe -pfiaC -e build/microej.elf 0x400d316d

Should output the corresponding instruction at the address given in
the last parameter.

If you want to decode multiple instructions, copy/paste the backtrace
in a text file and run this command:

.. code-block:: console

	cat backtrace.txt | sed 's/\ 0x/\n0x/' | sed 's/\:.*//' | grep '\ 0x' backtrace | xargs xtensa-esp32-elf-addr2line.exe -pfiaC -e build/microej.elf

Dependencies
------------

The following dependencies are included as part of the BSP:

- MicroEJ generic packs:

  * hal-stub (stubs for Hardware Abstraction Layer): 1.1.1
  * wadapps (Multi Application Framework): 1.0.1
  * wadapps.impl (extra implementation for Multi Application Framework): 1.0.2
  * microej-util: 1.3.0

