ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SRC := $(CORE)/rtw_odm_phydm_init.c
SHIM := host_odm_phydm_shim.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(CORE) \
	-include $(HOST_INC)/host_autoconf.h -DHOST_ODM_PHYDM_INIT_TEST

RUSTC := rustc
RUST_ODM := $(ROOT)/rust/rtw_odm.rs

.PHONY: all test test-c test-rust clean
all: test

test: test-c test-rust

test-c: test_odm_phydm_init
	./test_odm_phydm_init odm_phydm_init_vectors.json

test_odm_phydm_init: test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)

librust_odm_phydm.a: $(RUST_ODM)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_odm_phydm_init_test --crate-type staticlib $(RUST_ODM) -o $@

test-rust: test_odm_phydm_init_rust
	./test_odm_phydm_init_rust odm_phydm_init_vectors.json

test_odm_phydm_init_rust: test_odm_phydm_init.c librust_odm_phydm.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(HOST_JSON) librust_odm_phydm.a

clean:
	rm -f test_odm_phydm_init test_odm_phydm_init_rust librust_odm_phydm.a
