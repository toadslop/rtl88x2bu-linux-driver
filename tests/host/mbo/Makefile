# Host L2 differential tests for core/rtw_mbo.c IE parse leaf (W3-107 PR1).

ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := mbo_ie_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include

.PHONY: all test clean

all: test

test: test_mbo_ie
	./test_mbo_ie mbo_ie_vectors.json

test_mbo_ie: test_mbo_ie.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mbo_ie.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_mbo_ie
