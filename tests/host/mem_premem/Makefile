ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := mem_premem_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -include $(HOST_INC)/host_autoconf.h

RUST_MEM := $(ROOT)/rust/rtw_mem.rs
RUSTC := rustc

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust

test-c: test_mem_premem
	./test_mem_premem mem_premem_vectors.json

test-rust: test_mem_premem_rust
	./test_mem_premem_rust mem_premem_vectors.json

test_mem_premem: test_mem_premem.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mem_premem.c $(ORACLE) $(HOST_JSON)

librust_mem_premem.a: $(RUST_MEM)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mem_premem_test \
		--crate-type staticlib $(RUST_MEM) -o $@

test_mem_premem_rust: test_mem_premem.c librust_mem_premem.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mem_premem.c $(HOST_JSON) librust_mem_premem.a

clean:
	rm -f test_mem_premem test_mem_premem_rust librust_mem_premem.a
