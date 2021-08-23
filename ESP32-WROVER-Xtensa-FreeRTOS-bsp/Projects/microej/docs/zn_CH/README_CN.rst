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
.. |DEPLOYTOOL_NAME| replace:: Espressif Esptool
.. |MANUFACTURER| replace:: Espressif

.. _README: ./../../../README.rst
.. _RELEASE NOTES: ./../../../RELEASE_NOTES.rst
.. _CHANGELOG: ./../../../CHANGELOG.rst

================
|BOARD_NAME| BSP
================

这个工程包含了 |BOARD_NAME| 开发板 |PLATFORM| BSP源代码.  
BSP 工程基于 Espressif IoT Development Framework (ESP-IDF) 开发.

文档不包括如何创建|PLATFORM|, 请参考 `README`_.

Build & Flash Scripts 编译和烧写脚本
------------------------------------

``Project/microej/scripts/`` 文件夹保存了编译和烧写BSP的所有脚本.  
``.bat`` 和 ``.sh`` 脚本分别适合在Windows和Linux环境下运行.

- ``build*`` 脚本用来编译和链接BSP与MicroEJ应用, 并生成可以烧写的
MicroEJ固件(``application.out``) .

  ``build*`` 脚本按原样工作 , 
  假设工具链都安装在默认路径.

- ``run*`` 脚本用来烧写MicroEJ固件到 (``application.out``) 硬件设备.

  运行``run*``脚本, 需要提前配置好COM端口.

下面的环境变量是可配置的:  

- ``MSYS_PATH``: msys32安装位置 (Windows).
- ``XTENSA_PATH``: Xtensa工具链安装位置(Linux).
- ``ENV_BASH_CMD``: 编译BSP的命令行.
- ``ENV_FLASH_CMD``: 编译和烧写固件的命令行.
- ``ESPPORT``: 连接到硬件设备的串口.

脚本中的注释给出了每个变量的详细说明.

按照Espressif文档描述, 编译脚本期望工具链安装在 ``C:\msys32`` (Windows) 
或者``~/esp/xtensa-esp32-elf/``(Linux).  如果工具链安装在其他位置, 请更
新相关的环境变量..

其他可能用到的变量, 请参考文档 `section of Espressif documentation
<https://docs.espressif.com/projects/esp-idf/en/v3.3.4/get-started/index.html#environment-variables>`__.

环境变量可以定义在全局范围或者在 ``set_local_env*`` 脚本里. 
在执行``.bat`` (``.sh``) 脚本时, ``set_local_env.bat`` (``set_local_env.sh``)
如果存在也会被执行.  创建上述脚本可以定制环境变量. 已经提供了模板文件
``set_local_env.bat.tpl`` and ``set_local_env.sh.tpl``供参考.

Customize SDKCONFIG 定制 SDKCONFIG
----------------------------------

根据平台功能, 请按照下面的示例配置 ESP-IDF BSP 的 SDKCONFIG:

- ``sdkconfig.default`` 用于 Mono-Sandbox 固件
- ``sdkconfig_no_ota_no_systemview`` 用于 Multi-Sandbox 固件

改变SDKCONFIG，可以采取下列方式之一:

- 使用文本编辑器编辑
- 或者使用 make menuconfig

  Windows下Mono-Sandbox固件命令行例子:

  - ``SET ENV_BASH_CMD="export IDF_PATH=$(pwd)/../../Drivers/esp-idf && make menuconfig"``
  - ``build.bat``

把生成的 ``sdkconfig`` 改为 ``sdkconfig.default`` Mono-Sandbox 固件 (``sdkconfig_no_ota_no_systemview`` Multi-Sandbox 固件).

使用 Espressif 工具烧写开发板
---------------------------------------------

也可以使用``run*`` 脚本来烧写MicroEJ固件到设备.

- 启动 ESP32 Flash 下载工具 ( `下载 <https://www.espressif.com/en/support/download/other-tools>`__)
- 点击 ESP32 DownloadTool 按钮
- 选在 SPI下载 标签
- 按下面说明填写第一个空白行:
  - 勾选左侧的复选框
  - 点击 ... 并且提供 application.bin 文件路径
  - 在地址栏@附后后面输入0
- 设置已经挂载的COM口
- 波特率设置为 921600
- 点击 START

绿色进度条指示烧写进程, 请等待直到看到 FINISH 消息.

调试 WROVER dev kit
-------------------------------

**配置并启动 OpenOCD GDB 服务器**:

1. 进入 Espressif esp-idf "Setup of OpenOCD" 网页 ( `访问 <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/index.html#jtag-debugging-setup-openocd>`_)
2. 按照说明书安装 OpenOCD 软件
3. 按照说明书配置 ESP32-WROVER Kit v4.1 开发板 ( `访问 <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/configure-wrover.html#configure-usb-drivers>`_)
4. 按照说明书运行 OpenOCD ( `访问 <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/index.html#run-openocd>`_).  在Windows系统上, OpenOCD 可以在 MingGW (``mingw32.exe``) 里执行或者 使用Windows命令行 ( ``.\bin\openocd ...``).

现在你的OpenOCD GDB服务器已经准备好启动GDB调试客户端了.

使用 OpenOCD调试时, SD卡接口不能使用.  请参考 `RELEASE NOTES`_ 限制项章节获取更多信息

**启动GDB调试会话**:

1. 确保OpenOCD GDB服务器正在运行. 否则请按照前面的说明启动OpenOCD GDB服务器.
2. 在 MicroEJ SDK里, ``Run > Debug Configurations...``. 点击 ``Filter launch configuration...`` (左上角图标) and 取消 ``Filter Configuration Types``. 接下来关闭 ``Debug Configurations`` 窗口. 这个操作将会允许你看到先前过滤的Eclipse Configuration Types(e.g: ``GDB Hardware Debugging``).
3. 进入 Espressif esp-idf "Launch Debugger" 页面 (`访问re <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/index.html#launching-debugger>`_)
4. 按照说明书创建 Eclipse ``GDB Hardware Debugging`` 启动器 and 并且运行(`访问 <https://docs.espressif.com/projects/esp-idf/en/v3.3.4/api-guides/jtag-debugging/using-debugger.html#eclipse>`_). 在创建 Eclipse ``GDB Hardware Debugging`` 启动器点击``Project > Browse...``时, ``ESP32-WROVER-Xtensa-FreeRTOS-bsp`` 工程不会显示在工程列表里. 这是由于 ``ESP32-WROVER-Xtensa-FreeRTOS-bsp`` 不是一个Eclipse CDT工程.  因此, 请在``Project:``文本区域输入 ``ESP32-WROVER-Xtensa-FreeRTOS-bsp``并且在``C/C++ Application:``区域提供ELF可执行文件的绝对地址(e.g: ``[WORKSPACE_LOCATION]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Projects/microej/build/microej.elf``).

**编译包含调试信息的应用**:

编辑 ``sdkconfig.default`` (``sdkconfig_no_ota_no_systemview`` 如果使用 Multi-Sandbox 固件):

- 设置 ``CONFIG_OPTIMIZATION_LEVEL_DEBUG=y``
- 设置 ``CONFIG_OPTIMIZATION_LEVEL_RELEASE=``
- 设置 ``CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER=y`` (如果没有请添加).

转换PEM私钥为DER格式
------------------------

MicroEJ需要DER格式的私钥. 转换PEM私钥请按下列命令操作:

.. code-block:: console

	openssl.exe pkcs8 -inform PEM -in key1.pem -topk8 -outform DER -out key1.der -v1 PBE-SHA1-3DES -passout pass:<my_password>

不启动监视界面转换回溯信息
--------------------------

使用下面的命令:

.. code-block:: console

	xtensa-esp32-elf-addr2line.exe -pfiaC -e build/microej.elf 0x400d316d

应该可以按照最后一个参数给出的地址输出相应的代码.

如果需要多行代码, 可以把回溯信息粘贴到文本文件并执行下面的命令:

.. code-block:: console

	cat backtrace.txt | sed 's/\ 0x/\n0x/' | sed 's/\:.*//' | grep '\ 0x' backtrace | xargs xtensa-esp32-elf-addr2line.exe -pfiaC -e build/microej.elf

依赖项
------------

下面的依赖项已经包含在BSP里:

- MicroEJ 通用包:

  * hal-stub (stubs for Hardware Abstraction Layer): 1.1.1
  * wadapps (Multi Application Framework): 1.0.1
  * wadapps.impl (extra implementation for Multi Application Framework): 1.0.2
  * microej-util: 1.3.0

- 第三方工具

  * esptool (Flashing tool): 1.2.0

