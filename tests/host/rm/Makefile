# Host L2 differential tests for core/rtw_rm_util_rest.c (W3-33).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RM_REST_SRC := $(CORE)/rtw_rm_util_rest.c
HOST_JSON := ../crypto/host_vector_json.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RM_TEST -DCONFIG_RTW_80211K

.PHONY: all test test-c test-rust clean

all: test

# W3-34 PR2: C + Rust oracles (token helpers ported in this PR).
test: test-c test-rust

test-c: test_rm
	./test_rm rm_vectors.json

test_rm: test_rm.c $(RM_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rm.c $(RM_REST_SRC) $(HOST_JSON)

RUST_RM := $(ROOT)/rust/rtw_rm_util.rs

librust_rm_util.a: $(RUST_RM)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_rm_test \
		--crate-type staticlib $(RUST_RM) -o $@

test-rust: test_rm_rust
	./test_rm_rust rm_vectors.json

test_rm_rust: test_rm.c librust_rm_util.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RM_ORACLE -o $@ test_rm.c $(HOST_JSON) librust_rm_util.a

clean:
	rm -f test_rm test_rm_rust librust_rm_util.a
