.. 
	Copyright 2019-2023 MicroEJ Corp. All rights reserved.
	Use of this source code is governed by a BSD-style license that can be found with this software.

.. |BOARD_NAME| replace:: ESP-WROVER-KIT V4.1
.. |BOARD_REVISION| replace:: 4.1
.. |PLATFORM_VER| replace:: 2.2.0
.. |RCP| replace:: MICROEJ SDK
.. |PLATFORM| replace:: MicroEJ Platform
.. |PLATFORMS| replace:: MicroEJ Platforms
.. |SIM| replace:: MicroEJ Simulator
.. |ARCH| replace:: MicroEJ Architecture
.. |CIDE| replace:: MICROEJ SDK
.. |RTOS| replace:: FreeRTOS RTOS
.. |MANUFACTURER| replace:: Espressif

.. _中文版: ./../../docs/zn_CH/README_CN.rst
.. _README: ./../../../../../README.rst
.. _RELEASE NOTES: ./../../../../../RELEASE_NOTES.rst
.. _CHANGELOG: ./../../../../../CHANGELOG.rst
.. _README MicroEJ BSP: ./../../README.rst

================
|BOARD_NAME| BSP
================

`中文版`_

This module contains the git patch file needet to be applyied after the ``sdk/esp-idf`` submodules update to enable the Segger SystemView trace capability with custom MicroEJ implementation.

In this document you will find the steps needed to build an application with the SystemView feature enabled, trace and view output logs (traces).

The steps presented in the document assumes that the installation steps for `here <https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/get-started/index.html#installation-step-by-step>`__ are already done.

Applying patch with MicroEJ implementation of Segger SystemView
----------------------------------------------------------------

After all the submodules (components from the ``sdk/esp-idf`` folder) are updated (``git submodule update --init --recursive``), the patch ``SYSVIEW-MicroEJ.patch`` should be applyied to this folder via the ``git am SYSVIEW-MicroEJ.patch``. 

The same operation is achieved running the script ``RunAtFirstBSBCompile`` from the folder ``Project/microej/scripts/``.

This operation is neccessary to be executed only once, at the platform set-up, before first application build.

Another file relevant for the user, is the ``gdbinit`` file (located in the folder ``Project/microej/scripts/``). This file contains commands for starting the systemview tracing.

Configure the |BOARD_NAME|
--------------------------

To be able to run and save the logs, you will need to setup the board for the JTAG debugging. The setup operation for the hardware and also the USB drivers configuration is described `here <https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-guides/jtag-debugging/configure-ft2232h-jtag.html>`__. 

In case of problems, please check if the OpenOCD is installed properly as is described `here <https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-guides/jtag-debugging/index.html#setup-of-openocd>`__ (normally it should be correctly installed when the official Espressif toolchain is installed).

Installing the Segger SystemView application
--------------------------------------------

A PC software application is needet to properly view the saved traces from the |BOARD_NAME|. Follow this `link <https://www.segger.com/products/development-tools/systemview/>`__ to install the SystemView application.

A few words about logging trace Data
------------------------------------

In the BSB software, the activation of the systemview tracing is done by setting some configuration parameters in the ``sdkconfig`` project file.
These parameters are:

.. code-block::

      CONFIG_APPTRACE_DEST_TRAX=y
      CONFIG_SYSVIEW_ENABLE=y

An already build ``sdkconfig`` file with systemview activated is used when the ``build`` script is used.

The start of the logging is attached to a temporary breakpoint. To function properly, the logging must be started after the microJvm tast is created. Because of this, a label is created in the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/main/microej.c`` file, nammed ``start_systemview_logging``. 

In the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/scripts`` there is a ``dgbinit`` file. From this file the user can change the logging files and also the maximum file (tracing) lenght. The line that is responsible with this parameters looks like this:

.. code-block::

    mon esp sysview start file://../trace/systemview/uEJ_c1.svdat file://../trace/systemview/uEJ_c2.svdat 1 100000 

As we can see, the above configuration will create 2 files named ``uEJ_c1.svdat`` and ``uEJ_c2.svdat`` (one for every core), and will sample until the summed size of the files is 100000 bytes. User can change this parameters to suit his needs.

Using the systemview tracing feature
------------------------------------

To use this feature, simply build the application using ``build`` script, transfer the application to the |BOARD_NAME| using ``run`` script, then start the ``SysView`` script. All the above scripts are located in the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/scripts`` folder.

Two terminal windows will be opened. One is the OpenOCD server the other is with the gdb (debugging) session attached to that server that also log the systemview events.

After the completion of the logging phase, you will find two files located in the ``ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/trace/systemview`` folder. Open these files with the Segger SystemView PC application (``File -> Load data``). Consult the help file of the Segger SystemView for more information about how to interpret the data and to use the application.

Related resources to the SystemView implementation
--------------------------------------------------

This is a short review of the resources needed for the MicroEJ implementation of the Segger SystemView tracing feature:

- **SYSVIEW-MicroEJ.patch** git patch that must be applied on the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/sdk/esp-idf/``, after the submodules update. This file can be found in the ``ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/trace/systemview`` folder.
- **RunAtFirstBSBCompile** script file that update git submodules and apply the above patch (optional to use, user can make the manual operations). This script can be found in the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/scripts`` folder.
- **build** and **run** scripts. The build script will create an application with MicroEJ SystemView configuration active. These scripts can be found in the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/scripts`` folder.
- **gdbinit** is a file with commands that start the systemview logging.
- **SystemView** is a script file that starts the tracing session and fill the logging files. It use the **gdbinit** file as a receptacle of commands. This script it is found in the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/scripts`` folder.
- **uEJ_c1.svdat** and **uEJ_c2.svdat** - are the recorded log files. The names and path can be changed by the user (by altering the **gdbinit** file), and by default can be found in the ``/ESP32-WROVER-Xtensa-FreeRTOS-bsp/projects/microej/scripts`` folder. To view them, load them in the Segger SystemView PC software application.



