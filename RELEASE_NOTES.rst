..
    Copyright 2019-2021 MicroEJ Corp. All rights reserved.
	This library is provided in source code for use, modification and test, subject to license terms.
	Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

.. |BOARD_NAME| replace:: ESP-WROVER-KIT V4.1
.. |BOARD_REVISION| replace:: 4.1
.. |PLATFORM_NAME| replace:: ESP32 WROVER Platform
.. |PLATFORM_VER| replace:: 1.7.1
.. |RCP| replace:: MICROEJ SDK
.. |PLATFORM| replace:: MicroEJ Platform
.. |PLATFORMS| replace:: MicroEJ Platforms
.. |SIM| replace:: MicroEJ Simulator
.. |ARCH| replace:: MicroEJ Architecture
.. |CIDE| replace:: MICROEJ SDK
.. |RTOS| replace:: FreeRTOS RTOS
.. |MANUFACTURER| replace:: Espressif

.. _README MicroEJ BSP: ./ESP32-WROVER-Xtensa-FreeRTOS-bsp/Projects/microej/README.rst
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
   * - Architecture (simikou2)
     - 7.14.0
   * - UI Pack (simikou2UI)
     - 12.1.5
   * - NET Pack
     - 9.2.3
   * - NET-ADDONS Pack
     - 2.3.0
   * - HAL Pack
     - 2.0.2
   * - BLUETOOTH Pack
     - 2.1.0
   * - FS Pack
     - 4.0.3
   * - DEVICE Pack
     - 1.1.1

Please refer to the |PLATFORM| `module description file <./ESP32-WROVER-Xtensa-FreeRTOS-configuration/module.ivy>`_ 
for more details.

Board Support Package
---------------------

- BSP provider: |MANUFACTURER| (``esp-idf``)
- BSP version: v3.3.4

Please refer to the |MANUFACTURER| ``esp-idf`` GitHub git repository
available `here
<https://github.com/espressif/esp-idf/releases/tag/v3.3.4>`__.

Third Party Software
--------------------

Third party softwares used in BSP can be found `here
<https://github.com/espressif/esp-idf/tree/v3.3.4/components>`__. Here
is a list of the most important ones:

.. list-table::
   :widths: 3 3 3

   * - RTOS 
     - FreeRTOS
     - 8.2.0
   * - TCP/IP stack 
     - esp_lwip
     - 2.0.3
   * - Cryptographic stack 
     - Mbed TLS
     - 2.16.5
   * - File System stack 
     - FatFS
     - R0.13a
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

Leds
~~~~

The board provides an RGB matrix with 3 colored LEDs (red, green ,
blue).  However, only the red LED is available for the user.  The other
two LEDs are using GPIOs multiplexed for other uses.

Network
-------

|PLATFORM| features a network interface with Wi-Fi as an
underlying hardware media.  A limited number of 16 sockets could be
used for TCP connections, 16 for TCP listening (server) connections
and 16 for UDP connections. A DHCP client can be activated to retrieve
a dynamic IP address. All DNS requests can be handled by a MicroEJ
software implementation or a native one.

SSL
-------

|PLATFORM| features a network secure interface. Available
secured protocols are SSL 3.0, TLS 1.0, TLS 1.1, TLS 1.2. Supported
keys and certificates formats are PKCS#5 and PKCS#12, PEM or DER
encoded.

File System
-------

|PLATFORM| features a file system interface. An SD card is
used for the storage (previously formated to a FAT32 file system). Up
to 2 files can be opened simultaneously.


Known issues/limitations
========================

- NET LL API cannot change the socket
  send and receive buffer sizes (``LLNET_CHANNEL_IMPL_setOption`` function),
- FS API does not support file
  write/read with offset from/to immortal arrays,
- FS API does not support file backward
  skip,
- IPV6 is not supported,
- |BOARD_NAME| JTAG interface & SD Card interface usage are mutually exclusive. 
  As a consequence, SystemView (which uses the JTAG interface) is enabled only on the Mono-Sandbox Platform. 
  The Multi-Sandbox Platform which requires the FS API which initializes the SDCard interface.
- OTA is enabled only on the Mono-Sandbox Platform. It has been disabled on the Multi-Sandbox Platform in order to fit into the FLASH memory.
- As described in espressif documentation, LCD and microSD cannot be used at
  the same time without unsoldering the resistor R167 (`https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-wrover-kit.html#allocation-of-esp32-pins`).
- On Windows, the toolchain doesn't support long path.  The build
  directory (named ``target~/``) can be moved closer to the root
  filesystem with the ``target`` property.  For example, set
  ``target=C:/tmp/`` in
  ``ESP32-WROVER-Xtensa-FreeRTOS-configuration/module.properties``.

Platform Memory Layout
======================

Memory Sections
---------------

Each memory section is discribed in the GCC linker file available
`here
<https://github.com/espressif/esp-idf/blob/v3.3.4/components/esp32/ld/esp32.ld>`__

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
<https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-reference/system/heap_debug.html#heap-information>`__

Information on MicroEJ memory sections can be found `here
<./ESP32-WROVER-Xtensa-FreeRTOS-bsp/Projects/microej/components/microej_gen/Makefile.projbuild>`__.

Please also refer to the MicroEJ docs website page available `here
<https://docs.microej.com/en/latest/PlatformDeveloperGuide/coreEngine.html#link>`__
for more details.

