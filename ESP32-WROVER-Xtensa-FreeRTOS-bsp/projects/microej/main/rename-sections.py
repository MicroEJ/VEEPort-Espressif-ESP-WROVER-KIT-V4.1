#!/usr/bin/python
# Copyright 2021-2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

import sys
import subprocess

if len(sys.argv) != 3:
    print("This script expects two input arguments, the full path to the objcopy executable and the full path to microejapp object file")
    sys.exit(-1)

cmd = sys.argv[1] + \
" --rename-section _java_heap=.ext_ram.bss._java_heap \
--rename-section _java_immortals=.ext_ram.bss._java_immortals \
--rename-section ICETEA_HEAP=.ext_ram.bss.ICETEA_HEAP \
--rename-section .bss.vm.stacks.java=.ext_ram.bss.vm.stacks.java \
--rename-section .bss.microui.display.imagesHeap=.ext_ram.bss.microui.display.imagesHeap \
--rename-section .text.soar=.rodata.soar \
--rename-section .text.features.installed=.rodata.features.installed \
--rename-section .shieldedplug=.rodata.shieldedplug " + \
sys.argv[2]

print(cmd)

subprocess.call(cmd, shell=True)
