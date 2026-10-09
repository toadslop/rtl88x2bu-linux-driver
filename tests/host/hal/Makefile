# Host L2 differential tests for hal_com Rust slices (W4-01 hw rate, W4-02 rsvd page).

ROOT     := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE   := hal_hw_rate_c_oracle.c
RSVD_ORACLE := hal_rsvd_page_c_oracle.c
RSVD_SHIM := hal_rsvd_page_shim.c
RUST_HAL := $(ROOT)/rust/hal_com.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC)

.PHONY: all test test-c test-rust test-rsvd test-rsvd-rust clean

all: test

test: test-c test-rust test-rsvd

test-c: test_hw_rate_map
	./test_hw_rate_map hal_hw_rate_vectors.json

test_hw_rate_map: test_hw_rate_map.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hw_rate_map.c $(ORACLE) $(HOST_JSON)

librust_hal_com.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_hw_rate_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-rust: test_hw_rate_map_rust
	./test_hw_rate_map_rust hal_hw_rate_vectors.json

test_hw_rate_map_rust: test_hw_rate_map.c librust_hal_com.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_ORACLE -o $@ test_hw_rate_map.c $(HOST_JSON) librust_hal_com.a

test-rsvd: test_rsvd_page_cache
	./test_rsvd_page_cache

test_rsvd_page_cache: test_rsvd_page_cache.c $(RSVD_ORACLE) $(RSVD_SHIM)
	$(CC) $(CFLAGS) -o $@ test_rsvd_page_cache.c $(RSVD_ORACLE) $(RSVD_SHIM)

librust_hal_com_rsvd.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_rsvd_page_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-rsvd-rust: test_rsvd_page_cache_rust
	./test_rsvd_page_cache_rust

test_rsvd_page_cache_rust: test_rsvd_page_cache.c librust_hal_com_rsvd.a $(RSVD_SHIM)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_RSVD_ORACLE -o $@ test_rsvd_page_cache.c $(RSVD_SHIM) librust_hal_com_rsvd.a

clean:
	rm -f test_hw_rate_map test_hw_rate_map_rust librust_hal_com.a \
		test_rsvd_page_cache test_rsvd_page_cache_rust librust_hal_com_rsvd.a
