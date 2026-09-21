ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := host_br_ext_nat25_leaf.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test clean
all: test

test: test_nat25_leaf
	./test_nat25_leaf nat25_leaf_vectors.json

test_nat25_leaf: test_nat25_leaf.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_leaf.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_nat25_leaf
