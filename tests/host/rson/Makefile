# Host L2 differential tests for core/rtw_rson.c leaf helpers (W3-106).

ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DCONFIG_RTW_REPEATER_SON

CORE_ORACLE := rson_core_c_oracle.c host_rson_util.c
IE_ORACLE := rson_score_c_oracle.c rson_core_c_oracle.c rson_ie_c_oracle.c host_rson_util.c

RUST_RSON := $(ROOT)/rust/rtw_rson.rs
RUSTC := rustc

.PHONY: all test test-score test-core test-ie test-choose test-rust clean

all: test

test: test-score test-core test-ie test-choose

test-score: test_rson_score
	./test_rson_score rson_score_vectors.json

test-core: test_rson_core
	./test_rson_core rson_core_vectors.json

test_rson_score: test_rson_score.c rson_score_c_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_score.c rson_score_c_oracle.c $(HOST_JSON)

test_rson_core: test_rson_core.c $(CORE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_core.c $(CORE_ORACLE) $(HOST_JSON)

test-ie: test_rson_ie
	./test_rson_ie rson_ie_vectors.json

test_rson_ie: test_rson_ie.c $(IE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_ie.c $(IE_ORACLE) $(HOST_JSON)

test-choose: test_rson_choose
	./test_rson_choose rson_choose_vectors.json

test_rson_choose: test_rson_choose.c $(IE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_choose.c $(IE_ORACLE) $(HOST_JSON)

librust_rson.a: $(RUST_RSON)
	$(RUSTC) -C opt-level=2 --cfg host_rson_test --crate-type staticlib $(RUST_RSON) -o $@

test-rust: test_rson_score_rust test_rson_core_rust test_rson_ie_rust test_rson_choose_rust

test_rson_score_rust: test_rson_score.c librust_rson.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_score.c $(HOST_JSON) librust_rson.a

test_rson_core_rust: test_rson_core.c librust_rson.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_core.c $(HOST_JSON) librust_rson.a

test_rson_ie_rust: test_rson_ie.c librust_rson.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_ie.c $(HOST_JSON) librust_rson.a

test_rson_choose_rust: test_rson_choose.c librust_rson.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_choose.c $(HOST_JSON) librust_rson.a

test-rust-run: test-rust
	./test_rson_score_rust rson_score_vectors.json
	./test_rson_core_rust rson_core_vectors.json
	./test_rson_ie_rust rson_ie_vectors.json
	./test_rson_choose_rust rson_choose_vectors.json

clean:
	rm -f test_rson_score test_rson_core test_rson_ie test_rson_choose \
		test_rson_score_rust test_rson_core_rust test_rson_ie_rust test_rson_choose_rust \
		librust_rson.a
