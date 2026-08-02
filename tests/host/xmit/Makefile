# Host L2 differential tests for core/rtw_xmit_rest.c (W3-40).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
XMIT_REST_SRC := $(CORE)/rtw_xmit_rest.c
HOST_JSON := ../crypto/host_vector_json.c
SHIM     := host_xmit_shim.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_XMIT_TEST

.PHONY: all test clean

all: test

test: test_xmit
	./test_xmit xmit_vectors.json

test_xmit: test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)

clean:
	rm -f test_xmit
