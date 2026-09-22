ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := sdio_cmd_c_oracle.c
RUST_SDIO := $(ROOT)/rust/rtw_sdio.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -include $(HOST_INC)/host_autoconf.h

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust

test-c: test_sdio_cmd
	./test_sdio_cmd sdio_cmd_vectors.json

test-rust: test_sdio_cmd_rust
	./test_sdio_cmd_rust sdio_cmd_vectors.json

test_sdio_cmd: test_sdio_cmd.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sdio_cmd.c $(ORACLE) $(HOST_JSON)

librust_sdio_cmd.a: $(RUST_SDIO)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_sdio_cmd_test \
		--crate-type staticlib $(RUST_SDIO) -o $@

test_sdio_cmd_rust: test_sdio_cmd.c librust_sdio_cmd.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sdio_cmd.c $(HOST_JSON) librust_sdio_cmd.a -lm -ldl -pthread

clean:
	rm -f test_sdio_cmd test_sdio_cmd_rust librust_sdio_cmd.a
