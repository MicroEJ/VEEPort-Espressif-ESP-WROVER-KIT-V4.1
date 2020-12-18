# Build MicroEJ Platform Reference Implementation for ESP32-WROVER

## Introduction

This document describes how to build the MicroEJ Platform Reference Implementation for ESP32-WROVER and how to launch a MicroEJ application on this platform.

## Prerequisites

 * Java7 or higher
 * MicroEJ SDK 4.0 or later
 * Eclipse CDT plugin installed
 * GCC xtensa-esp32 5.2.0

## Projects Description

This platform source files are split into multiple projects:

`ESP32-WROVER-Xtensa-FreeRTOS-configuration` :
> Contains the platform reference implementation configuration description. Some modules are described in a specific sub-folder /<module> with some optional configuration files (<module>.properties and / or <module>.xml).

`ESP32-WROVER-Xtensa-FreeRTOS-bsp` :
> Contains a ready-to-use BSP software project for the ESP32-WROVER board, including a GCC xtensa-esp32 project, an implementation of MicroEJ core engine (and extensions) port on FreeRTOS OS and the ESP32-WROVER board support package.

`ESP32WROVER-Full-GNUv52_xtensa-esp32-[version]` :
> This project is created once the platform is built. It contains the platform for ESP32-WROVER board, ready to be linked into the GCC xtensa-esp32 project.

## Platform Reference Implementation Build

First step is to build a platform reference implementation for the ESP32-WROVER board from platform architecture ESP32. 

Build may be more or less complex, depending on the set	of foundation libraries that will be provided by the platform. Typical build steps are:
 * reference implementation import,
 * platform instantiation,
 * modules selection (native stacks, MicroEJ foundation libraries etc.),
 * Platform build
 * GCC xtensa-esp32 project generation.

### Reference implementation import
Procedure:
 * Unzip `ESP32WROVER-[UID]-fullPackaging-dev-fullpackaging-[version].zip` to a temporary dir.
 * In MicroEJ SDK:
   1. Import the architectures
     * Click on `File > Import`
     * Select `Platform, Virtual Devices and Architecture` and click on `Next >`
     * Select `Select directory`
     * Click on `Browse...` and browse to `platformArchitecture` directory
     * Accept the Term and conditions
     * Click on `Finish`
   2. Import the platform source
     * Click on `File > Import`
     * Select `General > Existing Projects into Workspace` and click on `Next >`
     * Select `Select file`
     * Click on `Browse...` and browse to `platformSource\ESP32WROVER-[UID]-platformSource.zip`
     * Select all Eclipse projects.
     * Click on `Finish`

### Modules selection
Procedure:
 * Open the file `ESP32-WROVER-Xtensa-FreeRTOS-configuration/ESP32WROVER.platform`.
 * Go to `content` tab.
 * select your modules

### Platform build
 * Go to `Overview` tab
 * Launch the platform build clicking on the link `Build Platform` in the `Overview > Build` section.
 * Once the platform is built, the `ESP32WROVER-Full-GNUv52_xtensa-esp32-[version]` project is created in the workspace.

## MicroEJ Application Build

Second step is to write and generate a MicroEJ application on top of the newly created platform. This platform has built-in MicroEJ examples. Some additional MicroEJ examples are available on <http://developer.microej.com>

Procedure:
 * Click on `File > New > MicroEJ Standalone Example Project`
 * Select `ESP32WROVER-Full-GNUv52_xtensa-esp32-[version]` platform
 * Select one of the available example
 * Click on `Next`
 * Click on `Finish`
 * In the Package Explorer, right-click on `[example]` project
 * Select `Run As > MicroEJ Application` to run the application on the MicroEJ simulator
 * Click on menu `Run > Run Configurations...`
 * The launcher is automatically selected on left menu, if not, select it
 * Open `Execution` tab
 * Select `Execute on Device`
 * Click on `Run`
 * Once MicroEJ launch is executed, the generated object file is available in `[workspace]/.microej/GNUv52_xtensa-esp32/application/microejapp.o`.

## Final Application build

Last step is to open the ESP32-WROVER board support project for GCC xtensa-esp32, compile it and link it with the MicroEJ application, and program the final application binary on the board.

### Board support project import

Procedure:
 * Click on `File > Import... > General > Existing Project into Workspace` and click on `Next >`
 * Select `Select root directory:`, click on `Browse...` and select the following folder: `[workspace]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Projects/microej`
 * Check the `microej` project and uncheck the `Copy project into workspace` checkbox
 * Click on `Finish`. Then, a new project named `microej` appear in your workspace

### Board support project setup

#### Tools installation

Procedure:
* Download `https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20181001.zip` and unzip its content to `[workspace]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Tools`
* Download `https://dl.espressif.com/dl/openocd-esp32-win32-a859564.zip` and unzip its content to `[workspace]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Tools`
* Download `https://github.com/pbatard/libwdi/releases/download/v1.3.0/zadig-2.3.exe` and copy it to `[workspace]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Tools/zadig`
* Refresh the `[workspace]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Tools` folder in Eclipse. Then, ESP32 tools is now installed.

Note: for more details, please refers to the following documentation: https://docs.espressif.com/projects/esp-idf/en/v3.3.1/get-started/index.html#setup-toolchain.

### Launch build

Procedure:
 * Select the `C/C++` Eclipse perspective by clicking on `Window > Perspective > Open Perspective > Others... > C/C++` and click on `Open`.
 * Click on the `[workspace]/microej` Eclipse project.
 * Click on `Project > Build Project`. Then the ESP32 board support project build is launched.
 * Sometime issue can occurred during the very beginning of the BSP build, See `How to fix issue with kconfig during build`

 ### How to program the ESP32 board
ESP32 board support project use `OpenOCD` tool to launch program and debug session. This section list the different step to do these actions.

Procedure:
 * In MicroEJ SDK, click on `Run > External Tools > [ATTACH] flash binary`.
 * The program of the board is now launched.

 #### SEGGER J-Link probe not recognized by OpenOCD
 `OpenOCD` need specific USB driver to recognize the SEGGER J-Link probe. You can use a tool named `Zadig` (https://github.com/pbatard/libwdi/releases/download/v1.3.0/zadig-2.3.exe) to install it.

Procedure:
 * Download `Zadig` here: https://github.com/pbatard/libwdi/releases/download/v1.3.0/zadig-2.3.exe
 * Launch the program by double-clicking on `zadig-2.3.exe`
 * Click on `Options > List All Devices`.
 * Select `J-Link` in the drop down list.
 * Select `WinUSB` driver in the next drop down list and click on `Replace Driver`, then the SEGGER J-Link probe USB driver is updated. You can now launch `OpenOCD` operations.

 Notice:
 Take care, `Zadig` tool will update your SEGGER J-Link driver. After that, SEGGER J-Link tools can not be used anymore (SEGGER J-Link probe will not been recognized). Nevertheless, you can reinstall the previous driver by going in the `Device Manager`. Choose the J-Link USB device in the devices list. Right click on it. Click on `Uninstall driver` and then reinstall the right one.

### How to fix issue with kconfig during build

Sometime issue can occurred during the very beginning of the BSP build. If your issue looks like:
```
WARNING: Toolchain version is not supported: 1.22.0-80-g6c4433a
Expected to see version: 1.22.0-75-gbaf03c2
Please check ESP-IDF setup instructions and update the toolchain, or proceed at your own risk.
make[1]: Entering directory '/c/Users/xxx/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig'
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o mconf.o mconf.c
flex -L -P zconf -o zconf.lex.c zconf.l
zconf.l:256: warning, -s option given but default rule can be matched
bison -t -l -p zconf -o zconf.tab.c zconf.y
/usr/bin/m4:/usr/share/bison/yacc.c:616: ERROR: end of file in string
zconf.y: fatal error: invalid @ in skeleton: @
make[1]: *** [Makefile:311: zconf.tab.c] Error 1
make[1]: Leaving directory '/c/Users/xxx/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig'
make: *** No rule to make target '/c/Users/xxx/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig/conf', needed by '/c/Users/xxx/Projects/microej/build/include/config/auto.conf'.  Stop
```

or

```
make[1]: Entering directory '/c/Users/xxx/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig'
sed -E "s/\\x0D$//" zconf.gperf | gperf -t --output-file zconf.hash.c -a -C -E -g -k '1,3,$' -p -t
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o zconf.tab.o zconf.tab.c
lxdialog/check-lxdialog.sh -check cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD -lncursesw -lintl
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o lxdialog/checklist.o lxdialog/checklist.c
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o lxdialog/util.o lxdialog/util.c
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o lxdialog/inputbox.o lxdialog/inputbox.c
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o lxdialog/textbox.o lxdialog/textbox.c
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o lxdialog/yesno.o lxdialog/yesno.c
cc  -I/usr/include/ncursesw -DCURSES_LOC="<curses.h>"  -DNCURSES_WIDECHAR=1 -DLOCALE -MD   -c -o lxdialog/menubox.o lxdialog/menubox.c
cc -o mconf mconf.o zconf.tab.o lxdialog/checklist.o lxdialog/util.o lxdialog/inputbox.o lxdialog/textbox.o lxdialog/yesno.o lxdialog/menubox.o -lncursesw -lintl
mconf.o:mconf.c:(.text+0x10e): undefined reference to `rootmenu'
mconf.o:mconf.c:(.text+0x1ff): undefined reference to `xcalloc'
mconf.o:mconf.c:(.text+0x224): undefined reference to `xcalloc'
mconf.o:mconf.c:(.text+0x3ee): undefined reference to `str_new'
mconf.o:mconf.c:(.text+0x432): undefined reference to `str_printf'
mconf.o:mconf.c:(.text+0x442): undefined reference to `str_get'
mconf.o:mconf.c:(.text+0x4ac): undefined reference to `str_free'
mconf.o:mconf.c:(.text+0x503): undefined reference to `str_new'
mconf.o:mconf.c:(.text+0x53c): undefined reference to `str_printf'
mconf.o:mconf.c:(.text+0x547): undefined reference to `str_get'
mconf.o:mconf.c:(.text+0x568): undefined reference to `sym_re_search'
mconf.o:mconf.c:(.text+0x5ab): undefined reference to `get_relations_str'
mconf.o:mconf.c:(.text+0x5d9): undefined reference to `str_get'
mconf.o:mconf.c:(.text+0x695): undefined reference to `str_free'
mconf.o:mconf.c:(.text+0x6fa): undefined reference to `str_free'
mconf.o:mconf.c:(.text+0x712): undefined reference to `str_free'
mconf.o:mconf.c:(.text+0x730): undefined reference to `menu_is_visible'
mconf.o:mconf.c:(.text+0x747): undefined reference to `menu_has_prompt'
mconf.o:mconf.c:(.text+0x7a9): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0x836): undefined reference to `menu_is_empty'
mconf.o:mconf.c:(.text+0x986): undefined reference to `sym_get_type'
mconf.o:mconf.c:(.text+0x9d4): undefined reference to `menu_is_visible'
mconf.o:mconf.c:(.text+0xa10): undefined reference to `sym_is_changable'
mconf.o:mconf.c:(.text+0xacf): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0xb1c): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0xb8d): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0xc4c): undefined reference to `sym_is_changable'
mconf.o:mconf.c:(.text+0xcdb): undefined reference to `sym_is_changable'
mconf.o:mconf.c:(.text+0xd51): undefined reference to `sym_get_string_value'
mconf.o:mconf.c:(.text+0xd6a): undefined reference to `sym_get_string_value'
mconf.o:mconf.c:(.text+0xdaf): undefined reference to `sym_is_changable'
mconf.o:mconf.c:(.text+0xdd3): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0xe34): undefined reference to `sym_is_changable'
mconf.o:mconf.c:(.text+0xe58): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0xea3): undefined reference to `menu_is_empty'
mconf.o:mconf.c:(.text+0xf2a): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0xf3b): undefined reference to `rootmenu'
mconf.o:mconf.c:(.text+0xf48): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0x11ae): undefined reference to `sym_set_tristate_value'
mconf.o:mconf.c:(.text+0x11c9): undefined reference to `sym_set_tristate_value'
mconf.o:mconf.c:(.text+0x1221): undefined reference to `sym_set_tristate_value'
mconf.o:mconf.c:(.text+0x124d): undefined reference to `sym_set_tristate_value'
mconf.o:mconf.c:(.text+0x126a): undefined reference to `sym_toggle_tristate_value'
mconf.o:mconf.c:(.text+0x1455): undefined reference to `str_new'
mconf.o:mconf.c:(.text+0x148a): undefined reference to `menu_get_ext_help'
mconf.o:mconf.c:(.text+0x1495): undefined reference to `str_get'
mconf.o:mconf.c:(.text+0x14a2): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0x14c1): undefined reference to `str_free'
mconf.o:mconf.c:(.text+0x14d9): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0x151b): undefined reference to `menu_is_visible'
mconf.o:mconf.c:(.text+0x153b): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0x1560): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0x168e): undefined reference to `sym_set_tristate_value'
mconf.o:mconf.c:(.text+0x16e6): undefined reference to `menu_get_prompt'
mconf.o:mconf.c:(.text+0x16f7): undefined reference to `sym_get_type'
mconf.o:mconf.c:(.text+0x175b): undefined reference to `sym_get_string_value'
mconf.o:mconf.c:(.text+0x17c9): undefined reference to `sym_set_string_value'
mconf.o:mconf.c:(.text+0x1880): undefined reference to `conf_read'
mconf.o:mconf.c:(.text+0x189c): undefined reference to `sym_set_change_count'
mconf.o:mconf.c:(.text+0x1956): undefined reference to `conf_write'
mconf.o:mconf.c:(.text+0x19de): undefined reference to `conf_get_changed'
mconf.o:mconf.c:(.text+0x1a49): undefined reference to `conf_write'
mconf.o:mconf.c:(.text+0x1b7e): undefined reference to `conf_set_message_callback'
mconf.o:mconf.c:(.text+0x1b92): undefined reference to `conf_parse'
mconf.o:mconf.c:(.text+0x1b9e): undefined reference to `conf_read'
mconf.o:mconf.c:(.text+0x1c43): undefined reference to `conf_get_configname'
mconf.o:mconf.c:(.text+0x1c57): undefined reference to `conf_set_message_callback'
mconf.o:mconf.c:(.text+0x1c66): undefined reference to `rootmenu'
collect2: error: ld returned 1 exit status
make[1]: *** [Makefile:296: mconf] Error 1
make[1]: Leaving directory '/c/Users/xxx/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig'
make: *** No rule to make target '/c/Users/xxx/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig/conf', needed by '/c/Users/xxx/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Projects/microej/build/include/config/auto.conf'.  Stop.
```

Procedure:
 * Right click on the `[workspace]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig`, click on `Properties` and copy the `location` path of the folder.
 * Open the `kconfig.zip` archive and extract it at the location previously copied
 * Refresh the `[workspace]/ESP32-WROVER-Xtensa-FreeRTOS-bsp/Drivers/esp-idf/tools/kconfig` folder in Eclipse. Then, pre-compiled kconfig tools is now installed.
 * Clean the `microej` project by clicking on the project and click on `Project > Clean...` and select only the `microej` project.
 * Launch now a build of the `microej` project with the procedure previously explained

---

_Copyright 2018-2020 MicroEJ Corp. All rights reserved._  
_ This library is provided in source code for use, modification and test, subject to license terms._  
_ Any modification of the source code will break MicroEJ Corp. warranties on the whole library._  
