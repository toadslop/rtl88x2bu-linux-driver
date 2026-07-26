# Host L2 differential tests for core/rtw_chplan.c lookup helpers (W2-17a).
# C oracle only — Rust oracle added in W2-17b.

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include

CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
           -I$(HOST_INC) -I$(CORE) \
           -include $(HOST_INC)/host_autoconf.h \
           -DHOST_CHPLAN_TEST

CHPLAN_SRCS := $(CORE)/rtw_chplan.c ../shim/host_chplan_rf.c \
               ../crypto/host_vector_json.c

.PHONY: all test clean

all: test

test: test_chplan
	./test_chplan chplan_vectors.json

test_chplan: test_chplan.c $(CHPLAN_SRCS)
	$(CC) $(CFLAGS) -o $@ test_chplan.c $(CHPLAN_SRCS)

clean:
	rm -f test_chplan
