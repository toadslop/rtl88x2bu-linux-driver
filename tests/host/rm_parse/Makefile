ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
RM_PARSE_SRC := $(CORE)/rtw_rm_parse_rest.c
STUBS := rm_parse_stubs.c
RUST_RM := $(ROOT)/rust/rtw_rm.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
	-I$(HOST_INC) -I$(CORE) -DHOST_RM_PARSE_TEST -DCONFIG_RTW_80211K

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust
test-c: test_rm_parse
	./test_rm_parse
test_rm_parse: test_rm_parse.c $(RM_PARSE_SRC) $(STUBS)
	$(CC) $(CFLAGS) -o $@ test_rm_parse.c $(RM_PARSE_SRC) $(STUBS)
librust_rm_parse.a: $(RUST_RM)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rm_parse_test \
		--crate-type staticlib $(RUST_RM) -o $@
test-rust: test_rm_parse_rust_part1
	./test_rm_parse_rust_part1
test_rm_parse_rust_part1: test_rm_parse_rust_part1.c librust_rm_parse.a $(STUBS)
	$(CC) $(CFLAGS) -o $@ test_rm_parse_rust_part1.c $(STUBS) librust_rm_parse.a
clean:
	rm -f test_rm_parse test_rm_parse_rust_part1 librust_rm_parse.a
