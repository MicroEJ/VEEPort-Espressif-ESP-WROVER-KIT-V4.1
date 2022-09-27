..
    Copyright 2019-2022 MicroEJ Corp. All rights reserved.
    Use of this source code is governed by a BSD-style license that can be found with this software.

===========
 Changelog
===========

---------------------
 [2.1.0] -
---------------------

Added
=====

- Add support for WebP image format
  - javafx 1.2.0
  - imageio-webp 1.0.1
  - libwebp 1.0.1

Changed
=======

- Update modules:
  - Pack UI 13.2.0.

---------------------
 [2.0.0] - 2022-06-02
---------------------

Added
=====

- MicroEJ C Components:
  - microui 1.1.0.
  - mjvm-freertos 1.1.0.
  - ned-bsd 1.3.1.
  - ecom-wifi-generic 2.1.0.
  - ecom-network-generic 2.2.1.
  - microej-pool 0.1.0.
  - bsp-microej-main 2.0.0.
  - security-mbedtls 0.10.0.
  - fs-generic 2.0.0.
  - fs-fatfs 2.1.0.
  - microej-time-freertos 0.1.0.
  - net-ssl-mbedtls 2.1.5.
  - net-bsd 1.3.1.
  - watchdog-timer-generic 3.1.0
  - watchdog-timer-checkpoint-freertos 2.0.1
- MicroEJ Pack:
  - watchdog-timer-pack 2.0.1

Changed
=======

- Change license terms to MicroEJ Corp. BSD-style license.
- Update frontpanel to framework 1.1.0.
- Update frontpanel to widget 2.0.0.
- Update modules:
  - Pack UI 13.1.0.
  - Pack FS 6.0.1.
  - Pack Bluetooth 2.2.1.
  - Pack net 10.1.0.
  - Pack net-addons 2.5.3.
- MicroEJ C Components:
  - trace-systemview 2.1.2.
  - osal-headers 1.0.0.
  - osal-FreeRTOS 1.0.2.
  - bsp-microej-async-worker 0.3.0.
  - async_select 2.3.0.
- Build scripts updated for use with ESP-IDF v4.
- Update to toolchain GNUv84_extensa-esp32, simikou 6, rev. 7.16.1.
- ESP-IDF Git submodule aligned with v4.3.2. 

Removed
=======

- ECOM-COMM over SPI.
- ECOM-COMM .jar and .h files in dropins.
- Frontpanel framework 1.1.0.

Fixed
=====

- Fix a DNS setting that caused an exception when trying to use a static IP configuration.
- Fix build.sh if BSP fail build do not fail.

---------------------
[1.8.4] - 2022-04-07
---------------------

Fixed
=====

- Comment C component dependencies in the `-configuration/module.ivy` file.

---------------------
[1.8.3] - 2021-09-13
---------------------

Fixed
=====

- Fix firmware linker error when output binary path is same as input binary path.

---------------------
[1.8.2] - 2021-08-17
---------------------

Added
=====

- Generate combined binary after link.

Fixed
=====

- PEM loader no longer loads multiple certificates.
- Timeout on socket no longer raise an unknown error.

---------------------
 [1.8.1] - 2021-07-22
---------------------

Added
=====

- Update PCA to 1.3.0.  See `./ESP32-WROVER-Xtensa-FreeRTOS-configuration/build/CHANGELOG.md`_ for more details.

---------------------
 [1.8.0] - 2021-04-16
---------------------

Added
=====

- Update PCA to 1.2.0.  See `./ESP32-WROVER-Xtensa-FreeRTOS-configuration/build/CHANGELOG.md`_ for more details.

---------------------
 [1.7.1] - 2021-03-16
---------------------

Fixed
=====

- Fix Platform project directory version.  Ensure the Platform project imported after a clone is the one being populated during the Platform build.
- Removed old mention of esptool.  esptool is not distributed by the Platform but by ESP-IDF.

---------------------
 [1.7.0] - 2021-03-09
---------------------

Added
=====

- Added Chinese documentation

Changed
=======

- Clarify message when ``ENV_SDKCONFIG`` is already installed as ``sdkconfig``.
- Do not fetch the Platform dependency MicroEJ Pack UI ``.xpfp`` explicitly, behavior is unchanged.
- Remove legacy ESP-IDF migration documentation.  Not needed anymore because the ESP-IDF is fetched as a submodule since 1.6.1.

Fixed
=====

- Fix Platform directory name in ``README.rst``, ``README_UPDATE_ESP-IDF.rst`` and ``README_CN.rst``.
- Fix typo and list display in ``CHANGELOG.rst``

----------------------
 [1.6.2] - 2020-12-18
----------------------

Fixed
=====

- Remove ``CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER`` from the provided sdkconfig files.  This prevents the build scripts to recompile the BSP every time.

----------------------
 [1.6.1] - 2020-12-17
----------------------

Fixed
=====

- Documentation for initial public revision.

----------------------
 [1.6.0] - 2020-12-17
----------------------

Added
=====

- Scripts to build and flash.
- Platform documentation.
- Initial ``Hello World`` application files to compile the BSP without any platform.
- An empty LLAPI security implementation.
- New platform builder.
- New platform build type.

Changed
=======

- Use Espressif ESP-IDF GitHub Git repository as a submodule.
- Replace the async_netconn with async_select MicroEJ C Component (CCO).
- New platform builder.
- New platform build type.
- Increase the number of characteristics which can be registered for notifications to 31.
- Update to Espressif ESP-IDF v3.3.4  .
- Update modules:.

  - MicroEJ Architecture simikou2 7.14.0.
  - Pack UI 12.1.5.
  - pack net 9.2.3.
  - pack net-addons 2.3.0.
  - pack hal 2.0.2.
  - pack bluetooth 2.0.1.
  - device pack 1.1.1.
  - fs 4.0.2.
  - API esp_idf 1.0.1.
  - esp_idf-impl 1.1.1.
  - esp_idf-mock 1.1.2.

- Update CCOs.

  - trace-systemview 2.0.1.
  - osal-headers 0.2.1.
  - osal-FreeRTOS 0.2.1.
  - async_select 2.0.2 MicroEJ C Component (CCO).
  - bsp-microej-async-worker 0.2.1 MicroEJ C Component (CCO).
  - esp_idf 1.1.1 MicroEJ C Component (CCO).

- Move Net low level buffer in PSRAM or RAM if no PSRAM.
- Update sdkconfig to have a running coremark.
- Rename frontpanel and configuration project to follow MicroEJ Naming Convention.
- Update frontpanel to framework 1.0.0.

Fixed
=====

- Rename MicroEJ sections so the linker put them at the correct position.

----------------------
 [1.5.1] - 2020-05-19
----------------------

Fixed
=====
- Net : update net pack to version 9.2.1. 

----------------------
 [1.5.0] - 2020-05-13
----------------------

Added
=====

- UI: Support for LCD mode in portrait (via compile switch).
- Core: Reset reason printed at startup.
- Device : Device pack version 1.1.0.

Changed
=======

- Update esp-idf version to 3.3.1.
- Optimize display driver my moving the functions in IRAM.
- Optimize the JVM RTOS task scheduling.
- Architecture : update architecture to version 7.12.1.
- Net : update net pack to version 9.2.0 and addons pack to version 2.1.6.
- Bluetooth: update bluetooth pack to version 2.0.0.
- Bluetooth: update LLBLUETOOTH implementation.

Fixed
=====

- Issue with some file names case on Linux 
- Wifi Driver : allow to enable softAP in OPEN auth mode.
- Wifi Driver : Fix SSID length too long (null terminated string).
- Ecom-network: Fix isEnabled and isStarted natives.
- Filesystem: Fix get free space and get total space computation.
- Filesystem: Fix the verification of writing data to a file.
- Net: Fix memory leak by freeing the netconn when closing a socket.
- SSL: Reset the ssl session after closing the ssl connection.
- Documentation: Link to Getting-Started page.
- Documentation: Board image.
- Documentation: Instructions to get the traces.

----------------------
 [1.4.5] - 2019-11-25
----------------------

Fixed
=====

- Documentation is now compliant with MicroEJ distribution 19.05.
- Getting Started URL in the README.rst.
- SDK URL in the RELEASE_NOTES.rst.

----------------------
 [1.4.4] - 2019-11-13
----------------------

Fixed
=====

- An issue that prevents building the platform from sources in MicroEJ SDK.

----------------------
 [1.4.2] - 2019-09-12
----------------------

Fixed
=====

- An issue that prevents linking application on linux hosts.

----------------------
 [1.4.1] - 2019-09-05
----------------------

Added
=====

- Bluetooth stub mock-up

Fixed
=====

- An issue that sometime prevents loading application in the simulator.
- Improve the WiFi mock-up user interface.

----------------------
 [1.4.0] - 2019-07-26
----------------------

This version differentiate configuration (sdkconfig) between singleapp (HDAHT)
and multiapp (9C5H4) platforms.

Added
=====

- Bluetooth support.
- New Mock Wi-Fi.
- SEGGER SytemView support for singleapp only.

Changed
=======

- Update OpenOCD version to win32-0.10.0-esp32-20190313,
- Improve documentation clarity.
- Disable OTA for multiapp only.

Fixed
=====

- LwIP socket leak with esp-idf ESP_THREAD_SAFE option enabled
- Fix URLs to espressif.doc.

----------------------
 [1.3.0] - 2018-10-08
----------------------

Added
=====

- Java ``System.out.println`` trace output on a new UART (USB COM port no
  longer used).
- Java ESP32 <code>esp-idf</code> foundation library.
- SNI 1.3 non immortal access feature

Fixed
=====

- LwIP issue that leads to lockup
- Net multi-thread access issues
- UI low level port do not support all LCD modules that can be included in
  ESP32-WROVER-KIT V3
- C stack overflow during complex TLS handshake

----------------------
 [1.2.0] - 2018-08-02
----------------------

Added
=====

- Wi-Fi throughput enhancement.
- UI MicroEJ pack and a device port.
- FS MicroEJ pack and a device port on SD card.

Fixed
=====

- Failure when trying to launch a Wi-Fi scan after mount and dismount.

----------------------
 [1.1.0] - 2018-05-30
----------------------

Added
=====

- HAL MicroEJ pack and a stubbed implementation.
- JPF MicroEJ platform.

Changed
=======

- Update esptool MicroEJTool error messages.

Fixed
=====

- Failures when trying to attach a GDB debug session.

----------------------
 [1.0.0] - 2018-05-04
----------------------

Initial release of the platform.
