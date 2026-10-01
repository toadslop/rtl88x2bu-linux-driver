ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SRC := $(CORE)/rtw_odm_phydm_init.c
SHIM := host_odm_phydm_shim.c
RUST_ODM := $(ROOT)/rust/rtw_odm.rs
RUSTC := rustc
VECTORS := odm_phydm_init_vectors.json
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(CORE) \
	-include $(HOST_INC)/host_autoconf.h -DHOST_ODM_PHYDM_INIT_TEST

.PHONY: all test test-c test-rust clean
all: test

test: test-c test-rust

test-c: test_odm_phydm_init
	./test_odm_phydm_init $(VECTORS)

test-rust: test_odm_phydm_init_rust
	./test_odm_phydm_init_rust $(VECTORS)

test_odm_phydm_init: test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)

librust_odm.a: $(RUST_ODM)
	$(RUSTC) --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_ODM) -o $@

test_odm_phydm_init_rust: test_odm_phydm_init.c librust_odm.a $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(SHIM) $(HOST_JSON) librust_odm.a

clean:
	rm -f test_odm_phydm_init test_odm_phydm_init_rust librust_odm.a \
		odm_phydm_init_c_ref.o
