ROOT := ../../..
HOST_INC := ../include
RUST_HOOKS := $(ROOT)/rust/rtw_debug_test_hooks.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include
ORACLE := rtw_debug_test_hooks_c_leaf.c
SHIM := host_rtw_debug_test_hooks_shim.c
L2 := test_debug_test_hooks_l2.c

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_debug_test_hooks_c
	./test_debug_test_hooks_c

test-rust: test_debug_test_hooks_rust
	./test_debug_test_hooks_rust

test_debug_test_hooks_c: $(L2) $(ORACLE)
	$(CC) $(CFLAGS) -o $@ $(L2) $(ORACLE)

librust_debug_test_hooks.a: $(RUST_HOOKS)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib --cfg host_rtw_debug_test_hooks_test \
		$(RUST_HOOKS) -o $@

test_debug_test_hooks_rust: $(L2) librust_debug_test_hooks.a $(SHIM)
	$(CC) $(CFLAGS) -DTEST_RUST_HOOKS -o $@ $(L2) $(SHIM) librust_debug_test_hooks.a

clean:
	rm -f test_debug_test_hooks_c test_debug_test_hooks_rust librust_debug_test_hooks.a
