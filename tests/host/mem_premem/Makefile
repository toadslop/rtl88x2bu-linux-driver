ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := mem_premem_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -include $(HOST_INC)/host_autoconf.h

.PHONY: all test test-c clean
all: test
test: test-c

test-c: test_mem_premem
	./test_mem_premem mem_premem_vectors.json

test_mem_premem: test_mem_premem.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mem_premem.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_mem_premem
