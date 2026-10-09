# Host L2 differential tests for rtw_debug (test hooks + sec_cam formatters).

ROOT      := ../../..
HOST_INC  := ../include
HOST_JSON := ../crypto/host_vector_json.c

# W3-138 test hooks (proc/debug)
RUST_HOOKS := $(ROOT)/rust/rtw_debug_test_hooks.rs
HOOKS_ORACLE := rtw_debug_test_hooks_c_leaf.c
HOOKS_SHIM := host_rtw_debug_test_hooks_shim.c
HOOKS_L2 := test_debug_test_hooks_l2.c

# W3-133 sec_cam dump formatters
SEC_ORACLE := debug_sec_cam_ent_c_oracle.c
SEC_SHIM   := host_rtw_debug_sec_cam_security_shim.c
RUST_SEC   := $(ROOT)/rust/rtw_debug_sec_cam.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(ROOT)/include

.PHONY: all test test-all test-c test-rust clean \
	test-hooks-c test-hooks-rust test-sec-cam-c test-sec-cam-rust

all: test

test: test-hooks-c test-hooks-rust test-sec-cam-c test-sec-cam-rust

test-all: test

test-c: test-hooks-c test-sec-cam-c

test-rust: test-hooks-rust test-sec-cam-rust

test-hooks-c: test_debug_test_hooks_c
	./test_debug_test_hooks_c

test_debug_test_hooks_c: $(HOOKS_L2) $(HOOKS_ORACLE)
	$(CC) $(CFLAGS) -o $@ $(HOOKS_L2) $(HOOKS_ORACLE)

librust_debug_test_hooks.a: $(RUST_HOOKS)
	rustc -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib --cfg host_rtw_debug_test_hooks_test \
		--cfg config_ap_mode \
		$(RUST_HOOKS) -o $@

test-hooks-rust: test_debug_test_hooks_rust
	./test_debug_test_hooks_rust

test_debug_test_hooks_rust: $(HOOKS_L2) librust_debug_test_hooks.a $(HOOKS_SHIM)
	$(CC) $(CFLAGS) -DTEST_RUST_HOOKS -o $@ $(HOOKS_L2) $(HOOKS_SHIM) librust_debug_test_hooks.a

test-sec-cam-c: test_dump_sec_cam_ent
	./test_dump_sec_cam_ent debug_sec_cam_vectors.json

test_dump_sec_cam_ent: test_dump_sec_cam_ent.c $(SEC_ORACLE) $(SEC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_sec_cam_ent.c $(SEC_ORACLE) $(SEC_SHIM) $(HOST_JSON)

librust_rtw_debug_sec_cam.a: $(RUST_SEC)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_sec_cam_test --cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(RUST_SEC) -o $@

test-sec-cam-rust: test_dump_sec_cam_ent_rust
	./test_dump_sec_cam_ent_rust debug_sec_cam_vectors.json

test_dump_sec_cam_ent_rust: test_dump_sec_cam_ent.c librust_rtw_debug_sec_cam.a $(SEC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_SEC_CAM_ORACLE -o $@ \
		test_dump_sec_cam_ent.c $(SEC_SHIM) $(HOST_JSON) librust_rtw_debug_sec_cam.a

clean:
	rm -f test_debug_test_hooks_c test_debug_test_hooks_rust librust_debug_test_hooks.a \
		test_dump_sec_cam_ent test_dump_sec_cam_ent_rust librust_rtw_debug_sec_cam.a
