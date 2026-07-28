# Host L2 differential tests for core/rtw_io_rest.c (W3-18 PR1).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
IO_SRC   := $(CORE)/rtw_io_rest.c
HOST_JSON := ../crypto/host_vector_json.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_IO_TEST -DDBG_IO -DDBG_IO_SNIFF_EXAMPLE

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_io
	./test_io io_vectors.json

test_io: test_io.c $(IO_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_io.c $(IO_SRC) $(HOST_JSON)

clean:
	rm -f test_io
