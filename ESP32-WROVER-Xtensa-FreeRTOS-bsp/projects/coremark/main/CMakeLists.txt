# Copyright 2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

idf_component_register(SRCS "coremark.c"
    "../coremark/src/core_list_join.c"
    "../coremark/src/core_main.c"
    "../coremark/src/core_matrix.c"
    "../coremark/src/core_portme.c"
    "../coremark/src/core_state.c"
    "../coremark/src/core_util.c"
    INCLUDE_DIRS "../coremark/inc"
)

set(compile_flags 
    "-fno-strict-aliasing"
    "-Wno-address")

# Definition for the number of iterations
add_compile_definitions(ITERATIONS=4000)

target_compile_options(${COMPONENT_LIB} PRIVATE ${compile_flags})

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--start-group" CACHE INTERNAL "-Wl,--end-group" FORCE)

