#
# Makefile
#
# Copyright 2018 IS2T. All rights reserved.
# IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
#

COMPONENT_ADD_INCLUDEDIRS += ../coremark/inc

COMPONENT_SRCDIRS += ../coremark/src

CFLAGS += -DITERATIONS=4000
