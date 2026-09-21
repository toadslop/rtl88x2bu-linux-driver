ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := wnm_btm_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_wnm_btm
	./test_wnm_btm wnm_btm_vectors.json

test_wnm_btm: test_wnm_btm.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wnm_btm.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_wnm_btm
