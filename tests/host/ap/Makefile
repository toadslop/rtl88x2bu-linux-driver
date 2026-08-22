# Host L2 differential tests for core/rtw_ap_rest.c (W3-55).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
AP_REST_SRC := $(CORE)/rtw_ap_rest.c
HOST_JSON := ../crypto/host_vector_json.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_AP_REST_TEST -DCONFIG_FW_HANDLE_TXBCN -DCONFIG_LIMITED_AP_NUM=4

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_ap_rest
	./test_ap_rest ap_rest_vectors.json

test_ap_rest: test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)

clean:
	rm -f test_ap_rest
