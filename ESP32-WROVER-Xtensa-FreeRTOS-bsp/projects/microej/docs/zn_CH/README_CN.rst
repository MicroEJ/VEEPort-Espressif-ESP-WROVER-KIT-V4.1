..
	Copyright 2019-2022 MicroEJ Corp. All rights reserved.
	Use of this source code is governed by a BSD-style license that can be found with this software.

.. |BOARD_NAME| replace:: ESP-WROVER-KIT V4.1
.. |BOARD_REVISION| replace:: 4.1
.. |PLATFORM_VER| replace:: 2.0.0
.. |RCP| replace:: MICROEJ SDK
.. |PLATFORM| replace:: MicroEJ Platform
.. |PLATFORMS| replace:: MicroEJ Platforms
.. |SIM| replace:: MicroEJ Simulator
.. |ARCH| replace:: MicroEJ Architecture
.. |CIDE| replace:: MICROEJ SDK
.. |RTOS| replace:: FreeRTOS RTOS
.. |DEPLOYTOOL_NAME| replace:: Espressif Esptool
.. |MANUFACTURER| replace:: Espressif

.. _README: ./../../../README.rst
.. _RELEASE NOTES: ./../../../RELEASE_NOTES.rst
.. _CHANGELOG: ./../../../CHANGELOG.rst
.. _MicroEJ SystemView README: ./trace/systemview/README.rst 

================
|BOARD_NAME| BSP
================
这个工程包含了 |BOARD_NAME| 开发板的 |PLATFORM| BSP源代码。
BSP 工程基于 Espressif IoT Development Framework (ESP-IDF) 开发。

文档不包括如何创建 |PLATFORM|, 请参考 `README`_ 来创建它。

Scripts 脚本
============
- **set_project_env** 脚本用来设置如下变量：
  
  - *ESPPORT* COM端口的值必须由用户设置（比如：`=COM6` 或者`==/dev/ttyS0`）
  - *IDF_TOOLS_PATH* 该环境变量的值在安装Xtensa toolchain时设置。必须定义并指向 Xtensa toolchain 安装位置以确保编译和烧写正常工作。
  - *ESP_PROJECT_DIR* 该值由脚本自动设置。它表示项目工程的存放路径（`\projects\microej`）。
  - *ESP_BUILD_DIR* 该值由脚本自动设置。它表示编译结果存放路径（`\projects\microej\build`）。
  - *ESP_IDF_PATH* 该值由脚本自动设置。它表示 Espressif 软件模块 ESP-IDF 的存放路径（`\sdk\esp-idf`）。

- **RunAtFirstBSPCompile** 脚本用来应用MicroEJ system view补丁。
  ``projects/microej/trace/systemview/`` 路径下的补丁文件 *SYSVIEW-MicroEJ.patch* 将会被安装到
  Espressif 软件模块 ESP-IDF 的存放路径（`\sdk\esp-idf`）。这将给 Segger System View 日志系统添加 MicroEJ 自定义的实现。

  用户也可以手动执行上述操作，并且仅在第一次编译BSP之前需要。

- **build** 脚本用来编译、链接 BSP 和 MicroEJ 应用以生成可以烧写到设备的 MicroEJ 固件（ ``application.out`` ）。
  这个脚本使用 Mono-Sandbox 固件配置，使用 *sdkconfig_ota_systemview* 创建 *sdkconfig* 配置文件。
- **run** 脚本用来烧写MicroEJ固件(``application.out``)到设备。为保证脚本正常工作，必须正确设置 
  **set_project_env** 文件里的变量 *ESPPORT* 。
- **build_no_ota_no_systemview** 脚本用来编译和链接BSP与MicroEJ应用, 并生成可以烧写的
  MicroEJ固件(``application.out``) 。脚本使用 *sdkconfig_no_ota_no_systemview* 生成 *sdkconfig* 配置文件，
  并使用这个配置文件生成 Multi-Sandbox 固件。SystemView 跟踪实现被禁用，并且输出也被配置为单应用程序。

- **SysView** 脚本用于启动应用程序、跟踪和记录日志到文件（启动 Segger SystemView 跟踪）。更多关于这个特性的信息请
  参考 `MicroEJ SystemView README`_ 以及  `Espressif 官方文档 <https://docs.espressif.com/projects/esp-idf/en/v4.3.2/esp32/api-guides/app_trace.html?highlight=segger#app-trace-system-behaviour-analysis-with-segger-systemview>`_。

调试 |BOARD_NAME|
=============================

调试 |BOARD_NAME| 的所有流程参见 `这里 <https://docs.espressif.com/projects/esp-idf/en/v4.3.2/esp32/api-guides/jtag-debugging/using-debugger.html>`_. 

使用 Segger SystemView
=======================

全部流程参见 `MicroEJ SystemView README`_。
对于 Mono-Sandbox 配置，使用此功能是开箱即用的（使用 **build** 脚本构建）。

转换PEM私钥为DER格式
================================

MicroEJ需要DER格式的私钥。转换PEM私钥请按下列命令操作:

.. code-block:: console

	openssl.exe pkcs8 -inform PEM -in key1.pem -topk8 -outform DER -out key1.der -v1 PBE-SHA1-3DES -passout pass:<my_password>

不启动监视界面转换回溯信息
========================================

使用下面的命令:

.. code-block:: console

	xtensa-esp32-elf-addr2line.exe -e build/microej.elf Backtrace:0x40081afb:0x3ffba7f0 0x4008d335:0x3ffba810 0x40092cae:0x3ffba830 0x4008bb0f:0x3ffba8a0

应该可以按照最后一个参数给出的地址输出相应的指令。

文件系统
===========

SD 卡 （默认）
-----------------

文件系统默认设置于SD卡中。 在这种情况下，MicroEJ FS Foundation Library 的初始化函数可以在这里找到: ``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\fs\src\LLFS_ESP32_init_sdcard.c`` 。

SPI Flash
---------

除了使用 SD 卡，还可以将 SPI Flash 用于文件系统。在这种情况下，初始化函数在这里实现: ``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\fs\src\LLFS_ESP32_init_spiflash.c``
要在 run time 使用此功能，需要修改 FS 配置，通过注释掉如下行来实现: ``#define LLFS_ESP32_USE_SD_CARD``。该代码位于下面的文件中：
``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\fs\inc\fs_configuration.h``。
此外，还需要在分区表中定义Flash分区。 这可以通过添加以下行：
``storage,	data,	fat,	,	1M,``
到这个 CSV 文件: ``ESP32-WROVER-Xtensa-FreeRTOS-bsp\projects\microej\partitions_ota_systemview.csv``。
还需要移除或调整至少一个其他分区的大小，否则它将不适合 |BOARD_NAME| 的Flash。
