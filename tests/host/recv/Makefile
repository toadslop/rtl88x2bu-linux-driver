# Host L2 differential tests for core/rtw_recv_rest.c (W3-39).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RECV_REST_SRC := $(CORE)/rtw_recv_rest.c
WFD_ORACLE := recv_wfd_c_oracle.c
HOST_JSON := ../crypto/host_vector_json.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RECV_TEST -DHOST_RECV_WFD_TEST

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_recv
	./test_recv recv_vectors.json

test_recv: test_recv.c $(RECV_REST_SRC) $(WFD_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_recv.c $(RECV_REST_SRC) $(WFD_ORACLE) $(HOST_JSON)

clean:
	rm -f test_recv
