# Host L2 differential tests for core/rtw_sta_mgt_rest.c (W3-37).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
STA_REST_SRC := $(CORE)/rtw_sta_mgt_rest.c
HOST_JSON := ../crypto/host_vector_json.c
HOST_SHIM := host_sta_mgt_shim.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_STA_MGT_TEST -DCONFIG_RTW_MACADDR_ACL

.PHONY: all test test-c sta_mgt-compile clean

all: test

# PR2: compile-only gate; vector harness in PR3.
sta_mgt-compile: $(STA_REST_SRC) $(HOST_SHIM)
	$(CC) $(CFLAGS) -c $(STA_REST_SRC) -o sta_mgt_rest_host.o
	$(CC) $(CFLAGS) -c $(HOST_SHIM) -o sta_mgt_shim_host.o

test: test-c

test-c: test_sta_mgt
	./test_sta_mgt sta_mgt_vectors.json

test_sta_mgt: test_sta_mgt.c $(STA_REST_SRC) $(HOST_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sta_mgt.c $(HOST_SHIM) $(STA_REST_SRC) $(HOST_JSON)

clean:
	rm -f test_sta_mgt sta_mgt_rest_host.o sta_mgt_shim_host.o
