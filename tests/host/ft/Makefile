ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := ft_ie_shim.c
STUBS := ft_ie_stubs.c
ORACLE := ft_ie_c_oracle.c
RUST_FT := $(ROOT)/rust/rtw_ft.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include \
	-DCONFIG_RTW_BTM_ROAM

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_ft_ie
	./test_ft_ie ft_ie_vectors.json

test-rust: test_ft_ie_rust
	./test_ft_ie_rust ft_ie_vectors.json

test_ft_ie: test_ft_ie.c $(SHIM) $(STUBS) $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ft_ie.c $(SHIM) $(STUBS) $(ORACLE) $(HOST_JSON)

librust_ft.a: $(RUST_FT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_ft_test \
		--cfg CONFIG_RTW_BTM_ROAM --crate-type staticlib $(RUST_FT) -o $@

test_ft_ie_rust: test_ft_ie.c $(SHIM) $(STUBS) librust_ft.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ft_ie.c $(SHIM) $(STUBS) $(HOST_JSON) librust_ft.a

clean:
	rm -f test_ft_ie test_ft_ie_rust librust_ft.a
