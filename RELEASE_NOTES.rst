..
    Copyright 2019-2023 MicroEJ Corp. All rights reserved.
    Use of this source code is governed by a BSD-style license that can be found with this software.

.. |BOARD_NAME| replace:: ESP-WROVER-KIT V4.1
.. |BOARD_REVISION| replace:: 4.1
.. |PLATFORM_NAME| replace:: ESP32 WROVER Platform
.. |PLATFORM_VER| replace:: 2.2.0
.. |RCP| replace:: MICROEJ SDK
.. |PLATFORM| replace:: MicroEJ Platform
.. |PLATFORMS| replace:: MicroEJ Platforms
.. |SIM| replace:: MicroEJ Simulator
.. |ARCH| replace:: MicroEJ Architecture
.. |CIDE| replace:: MICROEJ SDK
.. |RTOS| replace:: FreeRTOS RTOS
.. |MANUFACTURER| replace:: Espressif

.. _README MicroEJ BSP: ./ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/README.rst
.. _RELEASE NOTES: ./RELEASE_NOTES.rst
.. _CHANGELOG: ./CHANGELOG.rst

.. _release-notes:

========================================================
|PLATFORM| Release Notes for |MANUFACTURER| |BOARD_NAME|
========================================================

Description
===========

This is the release notes of the |PLATFORM| for |BOARD_NAME|.

Versions
========

Platform
--------

|PLATFORM_VER|

Dependencies
------------

This |PLATFORM| contains the following dependencies:

.. list-table::
   :header-rows: 1
   
   * - Dependency Name
     - Version
   * - Architecture (simikou6)
     - 7.20.1
   * - UI Pack (simikou6UI)
     - 13.2.0
   * - NET Pack
     - 10.1.0
   * - NET-ADDONS Pack
     - 2.5.3
   * - HAL Pack
     - 2.0.2
   * - BLUETOOTH Pack
     - 2.2.1
   * - FS Pack
     - 6.0.1
   * - DEVICE Pack
     - 1.1.1
   * - WATCHDOG-TIMER Pack
     - 2.0.1

Please refer to the |PLATFORM| `module description file <./ESP32-WROVER-Xtensa-FreeRTOS-configuration/module.ivy>`_ 
for more details.

Board Support Package
---------------------

- BSP provider: |MANUFACTURER| (``esp-idf``)
- BSP version: v4.3.2

Please refer to the |MANUFACTURER| ``esp-idf`` GitHub git repository
available `here
<https://github.com/espressif/esp-idf/releases/tag/v4.3.2>`__.

Third Party Software
--------------------

Third party softwares used in BSP can be found `here
<https://github.com/espressif/esp-idf/tree/v4.3.2/components>`__. Here
is a list of the most important ones:

.. list-table::
   :widths: 3 3 3

   * - RTOS 
     - FreeRTOS
     - 10.2.1
   * - TCP/IP stack 
     - esp_lwip
     - 2.1.2
   * - Cryptographic stack 
     - Mbed TLS
     - 2.16.11
   * - File System stack 
     - FatFS
     - R0.13c
   * - Bluetooth stack 
     - BLUEDROID
     - N/A

Features
========

Graphical User Interface
------------------------

|PLATFORM| features a graphical user interface.  It includes
a display, one user LED and a runtime PNG decoder.

Display
~~~~~~

The display module drives a 320 x 240 LCD display.  The pixel format
is 16 bits-per-pixel.  The display device is clocked at 60Hz and it is
connected to the MCU via a SPI link, clocked at 80MHz for ST7789V LCP
display and at 33MHz for ILI9341V LCD display.

MicroUI requires a RAM buffer to store the dynamic images data.  A
dynamic image is an image decoded at runtime (PNG image) or an image created
by the MicroEJ application using the ``Image.create(width, height)`` API.
This buffer is located in external RAM.

LEDs
~~~~

The board provides an RGB matrix with 3 colored LEDs (red, green ,
blue).  However, only the red LED is available for the user.  The other
two LEDs are using GPIOs multiplexed for other uses.

ECOM-COMM
---------

|PLATFORM| features a ecom-comm interface. Two serial connections are provided for (ECOM COMM) 

-  UART1: uses GPIO_PIN_4 for Tx and GPIO_PIN_5 for Rx 

-  UART2: uses GPIO_PIN_18 for Tx and GPIO_PIN_19 for Rx 

.. note::

   This implementation uses a receive queue mechanism and relies on the MicroEJ ``LLCOMM_BUFFERED_CONNECTION`` API. This API is FIFO oriented. It requires two
   distincts software buffers for reception and transmission: reception buffer uses 2048 bytes and transmission buffer uses 2048 bytes. These buffers are statically allocated in internal RAM.


Network
-------

|PLATFORM| features a network interface with Wi-Fi as an
underlying hardware media.  A limited number of 16 sockets could be
used for TCP connections, 16 for TCP listening (server) connections
and 16 for UDP connections. A DHCP client can be activated to retrieve
a dynamic IP address. All DNS requests are handled by LwIP.

SSL
-------

|PLATFORM| features a network secure interface. Available
secured protocols are SSL 3.0, TLS 1.0, TLS 1.1, TLS 1.2. Supported
keys and certificates formats are PKCS#5 and PKCS#12, PEM or DER
encoded.

File System
-------

|PLATFORM| features a file system interface. An SD card is
used for the storage (previously formated to a FAT32 file system). It's 
also possible to use SPI flash for storage. By default the file system 
is set to be in the SD card (see ``File System`` section in `README MicroEJ BSP`_ 
for more details). Up to 2 files can be opened simultaneously.


Known issues/limitations
========================

- NET LL API cannot change the socket
  send and receive buffer sizes (``LLNET_CHANNEL_IMPL_setOption`` function),
- FS API does not support file
  write/read with offset from/to immortal arrays,
- FS API does not support file backward
  skip,
- TestFileProperties from the FS testsuite fails with current fs#6.0.1 pack,
- IPV6 is not supported,
- |BOARD_NAME| JTAG interface & SD Card interface usage are mutually exclusive. 
  As a consequence, SystemView (which uses the JTAG interface) is enabled only on the Mono-Sandbox Platform. 
  The Multi-Sandbox Platform which requires the FS API which initializes the SDCard interface.
- OTA is enabled only on the Mono-Sandbox Platform. It has been disabled on the Multi-Sandbox Platform in order to fit into the FLASH memory.
- As described in espressif documentation, LCD and microSD cannot be used at
  the same time without unsoldering the resistor R167 (`https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-wrover-kit.html#allocation-of-esp32-pins`).
- On Windows, the toolchain doesn't support long paths. 
  The build directory, set with the variable ``ESP_BUILD_DIR``, can be moved closer to the root of the filesystem. 
  To override the default value create a copy of this file: ``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\scripts\set_local_env.bat.tpl``. 
  Remove the ``.tpl`` at the end of the file name and set the variable ``ESP_BUILD_DIR``, for example with ``C:\tmp``, to prevent toolchain errors.

Platform Memory Layout
======================

Memory Sections
---------------

Each memory section is discribed in the GCC linker file available
`here
<https://github.com/espressif/esp-idf/blob/v4.3.2/components/esp32/ld/esp32.ld>`__

Memory Layout
-------------

.. list-table::
   :header-rows: 1
   
   * - Section Content
     - Section Source
     - Section Destination
     - Memory Type
   * - MicroEJ Application static
     - ``.bss.soar``
     - ``.bss``
     - internal RAM
   * - MicroEJ Application threads stack blocks 
     - ``.bss.vm.stacks.java``
     - ``.ext_ram.bss``
     - external PSRAM
   * - MicroEJ Core Engine internal heap 
     - ``ICETEA_HEAP``
     - ``.ext_ram.bss``
     - external PSRAM
   * - MicroEJ Application heap 
     - ``_java_heap``
     - ``.ext_ram.bss``
     - external PSRAM
   * - MicroEJ Application Immortal Heap 
     - ``_java_immortals``
     - ``.ext_ram.bss``
     - external PSRAM
   * - MicroEJ Application resources 
     - ``.rodata.resources``
     - ``.rodata``
     - external QSPI
   * - MicroEJ System Applications code and resources 
     - ``.rodata.soar.features``
     - ``.rodata``
     - external QSPI
   * - MicroEJ System Application statics 
     - ``.bss.features.installed``
     - ``.ext_ram.bss``
     - external PSRAM
   * - MicroEJ Shielded Plug data 
     - ``.shieldedplug``
     - ``.rodata``
     - external QSPI
   * - MicroEJ Application and Library code 
     - ``.text.soar``
     - ``.rodata``
     - external QSPI
   * - MicroUI frame buffer
     - ``-``
     - ``.ext_ram.bss``
     - external PSRAM

For the C heap, please refer to the |MANUFACTURER| documentation
available `here
<https://docs.espressif.com/projects/esp-idf/en/v4.3.2/api-reference/system/heap_debug.html#heap-information>`__

Information on MicroEJ memory sections can be found `here
<./ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/main/rename-sections.py>`__.

Please also refer to the MicroEJ docs website page available `here
<https://docs.microej.com/en/latest/PlatformDeveloperGuide/coreEngine.html#link>`__
for more details.

