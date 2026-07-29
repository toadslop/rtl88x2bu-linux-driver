# Host L2 differential tests for core/rtw_rf_rest.c (W3-19).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RF_SRC   := $(CORE)/rtw_rf_rest.c
HOST_JSON := ../crypto/host_vector_json.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RF_TEST

.PHONY: all test clean

all: test

# W3-24 PR1: C oracle only; test-rust is restored in PR2 (stacked Rust port).
test: test_rf
	./test_rf rf_vectors.json

test_rf: test_rf.c $(RF_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rf.c $(RF_SRC) $(HOST_JSON)

clean:
	rm -f test_rf
