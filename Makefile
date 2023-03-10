# Copyright (c) 2023 Leandro José Britto de Oliveira.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

export CPP_PROJECT_BUILDER ?= $(abspath libs/libserial/test/libs/libcomm/make)
include $(CPP_PROJECT_BUILDER)/common.mk

PROJ_NAME    := jserial-jni
PROJ_TYPE    := lib
LIB_TYPE     := shared
PROJ_VERSION := 0.1.0

INCLUDE_DIRS += $(O)/libs/dist/include
LDFLAGS      += -L$(O)/libs/dist/lib

# libserial ====================================================================
LIBSERIAL_DIR  := libs/libserial
PRE_BUILD_DEPS += $(O)/libs/libserial.marker
LDFLAGS        += -lserial0

--libserial:
	$(O_VERBOSE)$(MAKE) -C $(LIBSERIAL_DIR) O=$(call FN_REL_DIR,$(LIBSERIAL_DIR),$(O)/libs) BUILD_SUBDIR=libserial DIST_MARKER=libserial.marker LIB_TYPE=static  EXTRA_CFLAGS=-fPIC

$(O)/libs/libserial.marker: --libserial ;
# ==============================================================================

CFLAGS += -std=gnu99 -fvisibility=hidden -DLIB_VERSION=\"$(PROJ_VERSION)\"

# ------------------------------------------------------------------------------
JNI_NATIVE_HEADERS := libs/jni-native-headers
include $(JNI_NATIVE_HEADERS)/jni.mk
# ------------------------------------------------------------------------------

include $(CPP_PROJECT_BUILDER)/builder.mk
