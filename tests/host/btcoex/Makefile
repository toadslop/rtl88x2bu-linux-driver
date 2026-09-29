ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_BTCOEX := $(ROOT)/rust/rtw_btcoex.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)
L2_SRC := btcoex_init_notify_l2.c
HP_SRC := btcoex_handler_policy_l2.c

.PHONY: all test test-c test-rust test-handler test-handler-c clean
all: test

test: test-c test-rust test-handler

test-c: test_btcoex_init_notify
	./test_btcoex_init_notify btcoex_init_notify_vectors.json

test-rust: test_btcoex_init_notify_rust
	./test_btcoex_init_notify_rust btcoex_init_notify_vectors.json

test-handler: test-handler-c

test-handler-c: test_btcoex_handler_policy
	./test_btcoex_handler_policy btcoex_handler_policy_vectors.json

test_btcoex_init_notify: $(L2_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(L2_SRC) $(HOST_JSON)

test_btcoex_init_notify_rust: $(L2_SRC) librust_btcoex_init_notify.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_BTCOEX_INIT_NOTIFY_RUST -o $@ $(L2_SRC) $(HOST_JSON) \
		librust_btcoex_init_notify.a

librust_btcoex_init_notify.a: $(RUST_BTCOEX)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_btcoex_init_notify_test \
		--crate-type staticlib $(RUST_BTCOEX) -o $@

test_btcoex_handler_policy: $(HP_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(HP_SRC) $(HOST_JSON)

clean:
	rm -f test_btcoex_init_notify test_btcoex_init_notify_rust librust_btcoex_init_notify.a
	rm -f test_btcoex_handler_policy test_btcoex_handler_policy_rust librust_btcoex_handler_policy.a
