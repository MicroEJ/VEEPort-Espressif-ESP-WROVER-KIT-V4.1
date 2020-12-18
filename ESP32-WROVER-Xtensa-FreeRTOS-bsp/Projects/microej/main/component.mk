#
# Makefile
#
# Copyright 2018-2020 MicroEJ Corp. All rights reserved.
# This library is provided in source code for use, modification and test, subject to license terms.
# Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
#

COMPONENT_ADD_INCLUDEDIRS += \
                             ../config/inc \
                             ../core/inc \
                             ../platform/inc \
                             ../ecom-comm/inc \
                             ../ecom-network/inc \
                             ../ecom-wifi/inc \
                             ../microej-util/inc \
                             ../net/inc \
                             ../ssl/inc \
                             ../util/inc \
                             ../bluetooth/inc \
                             ../kf/inc \
                             ../hal/inc \
                             ../fs/inc \
                             ../ui/inc \
                             ../espressif/inc \
                             ../uart/inc \
                             ../security/inc

ifdef CONFIG_SYSVIEW_ENABLE
COMPONENT_ADD_INCLUDEDIRS += ../trace/inc
endif

COMPONENT_SRCDIRS += \
                     ../core/src \
                     ../ecom-comm/src \
                     ../ecom-network/src \
                     ../ecom-wifi/src \
                     ../microej-util/src \
                     ../net/src \
                     ../ssl/src \
                     ../util/src \
                     ../bluetooth/src \
                     ../kf/src \
                     ../hal/src \
                     ../fs/src \
                     ../ui/src \
                     ../espressif/src \
                     ../uart/src \
                     ../security/src

ifdef CONFIG_SYSVIEW_ENABLE
COMPONENT_SRCDIRS += ../trace/src
endif

CFLAGS += -Wno-address             # lots of LWIP header files evaluate macros that check address of stack variables
CFLAGS += -fno-strict-aliasing     # NETIF structures are cast to LwIP IP4 types which break the anti-aliasing rules

COMPONENT_DEPENDS += microej_platform

# Add MicroEJ Libraries
COMPONENT_ADD_LDFLAGS +=  platform/lib/microejapp.o \
						platform/lib/microejruntime.a
                      