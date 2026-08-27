ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_CMD_REST := $(ROOT)/rust/rtw_cmd_rest.rs
CMD_QUEUE_SRC := $(CORE)/rtw_cmd_queue.c

CC := gcc
RUSTC := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(CORE) -include $(HOST_INC)/host_autoconf.h \
          -DHOST_CMD_PRIV_TEST
CFLAGS_CMD_QUEUE := $(CFLAGS) -DHOST_CMD_QUEUE_TEST

.PHONY: all test test-c test-rust test-cmd-queue clean

all: test

test: test-c test-rust test-cmd-queue

test-c: test_cmd_priv
	./test_cmd_priv cmd_priv_vectors.json

test_cmd_priv: test_cmd_priv.c host_cmd_priv_shim.c $(CORE)/rtw_cmd_priv.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_cmd_priv.c host_cmd_priv_shim.c $(CORE)/rtw_cmd_priv.c $(HOST_JSON)

test-cmd-queue: test_cmd_queue
	./test_cmd_queue cmd_queue_vectors.json

test_cmd_queue: test_cmd_queue.c $(CMD_QUEUE_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS_CMD_QUEUE) -o $@ test_cmd_queue.c $(CMD_QUEUE_SRC) $(HOST_JSON)

librust_cmd_rest.a: $(RUST_CMD_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_cmd_priv_test --cfg event_thread_mode --cfg c2h_wk \
		--crate-type staticlib $(RUST_CMD_REST) -o $@

test-rust: test_cmd_priv_rust
	./test_cmd_priv_rust cmd_priv_vectors.json

test_cmd_priv_rust: test_cmd_priv.c host_cmd_priv_shim.c librust_cmd_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_cmd_priv.c host_cmd_priv_shim.c $(HOST_JSON) librust_cmd_rest.a

clean:
	rm -f test_cmd_priv test_cmd_priv_rust test_cmd_queue librust_cmd_rest.a
