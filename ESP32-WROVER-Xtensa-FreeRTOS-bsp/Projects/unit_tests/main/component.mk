#
# Makefile
#
# Copyright 2018 IS2T. All rights reserved.
# IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#

COMPONENT_ADD_INCLUDEDIRS += ../embunit \
                             ../embunit/textui \
                             ../ram \
                             ../ram/inc

COMPONENT_SRCDIRS += ../embunit/embUnit \
                     ../embunit/textui \
                     ../ram \
                     ../ram/src

CFLAGS += -fno-strict-aliasing     # embunit module uses type-casting which break the anti-aliasing rules
