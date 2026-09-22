ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
	-I$(HOST_INC) -DHOST_RM_FSM_TEST -DCONFIG_RTW_80211K
RUST_RM := $(ROOT)/rust/rtw_rm_fsm.rs
RUSTC   := rustc

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust
test-c: test_rm_fsm
	./test_rm_fsm rm_fsm_vectors.json
test-rust: test_rm_fsm_rust
	./test_rm_fsm_rust rm_fsm_vectors.json
test_rm_fsm: test_rm_fsm.c rm_fsm_stubs.c rm_fsm_c_oracle.c rm_fsm_queue_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rm_fsm.c rm_fsm_stubs.c rm_fsm_c_oracle.c rm_fsm_queue_oracle.c $(HOST_JSON)
librust_rm_fsm.a: $(RUST_RM)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rm_fsm_test \
		--crate-type staticlib $(RUST_RM) -o $@
test_rm_fsm_rust: test_rm_fsm.c librust_rm_fsm.a rm_fsm_stubs.c $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RM_FSM_ORACLE -o $@ test_rm_fsm.c rm_fsm_stubs.c $(HOST_JSON) librust_rm_fsm.a
clean:
	rm -f test_rm_fsm test_rm_fsm_rust librust_rm_fsm.a
