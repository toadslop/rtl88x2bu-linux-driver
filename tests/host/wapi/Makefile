# Host L2 differential tests for core/rtw_wapi.c PN/IE/CAM leaf (W3-109).

ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := wapi_pn_cam_c_oracle.c
RUST_WAPI := $(ROOT)/rust/rtw_wapi.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_wapi_pn_cam
	./test_wapi_pn_cam wapi_pn_cam_vectors.json

test-rust: test_wapi_pn_cam_rust
	./test_wapi_pn_cam_rust wapi_pn_cam_vectors.json

test_wapi_pn_cam: test_wapi_pn_cam.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wapi_pn_cam.c $(ORACLE) $(HOST_JSON)

librust_wapi.a: $(RUST_WAPI)
	$(RUSTC) --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_wapi_pn_cam_test --crate-type staticlib $(RUST_WAPI) -o $@

test_wapi_pn_cam_rust: test_wapi_pn_cam.c librust_wapi.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wapi_pn_cam.c $(HOST_JSON) librust_wapi.a

clean:
	rm -f test_wapi_pn_cam test_wapi_pn_cam_rust librust_wapi.a
