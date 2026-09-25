ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test clean
all: test

test: test_mi_ch_union
	./test_mi_ch_union mi_ch_union_vectors.json

test_mi_ch_union: mi_ch_union_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ mi_ch_union_l2.c $(HOST_JSON)

clean:
	rm -f test_mi_ch_union
