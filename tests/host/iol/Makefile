# Host L2 differential tests for core/rtw_iol_rest.c (W3-50) — PR2 scaffold.

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
IOL_REST_SRC := $(CORE)/rtw_iol_rest.c
HOST_JSON := ../crypto/host_vector_json.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_IOL_TEST

.PHONY: all test clean

all:
	@echo "iol L2 runner lands in PR3 (test_iol.c)"

test:
	@echo "iol L2 runner lands in PR3 (test_iol.c)"

clean:
	rm -f test_iol test_iol_rust librust_iol_rest.a iol_rest_c_ref.o
