# Host L2 differential tests for core/rtw_rm_util_rest.c (W3-33).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RM_REST_SRC := $(CORE)/rtw_rm_util_rest.c
HOST_JSON := ../crypto/host_vector_json.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RM_TEST -DCONFIG_RTW_80211K

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_rm
	./test_rm rm_vectors.json

test_rm: test_rm.c $(RM_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rm.c $(RM_REST_SRC) $(HOST_JSON)

clean:
	rm -f test_rm
