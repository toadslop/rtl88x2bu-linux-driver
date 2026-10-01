ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
	-I$(HOST_INC) -DHOST_MP_PMAC_TEST

RUST_MP := $(ROOT)/rust/rtw_mp.rs
RUSTC   := rustc

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust
test-c: test_mp_pmac
	./test_mp_pmac mp_pmac_vectors.json
test-rust: test_mp_pmac_rust
	./test_mp_pmac_rust mp_pmac_vectors.json

test_mp_pmac: test_mp_pmac.c mp_pmac_stubs.c mp_pmac_bit_crc_oracle.c mp_pmac_cck_oracle.c mp_pmac_param_oracle.c mp_pmac_sig_oracle.c mp_pmac_ht_sig_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mp_pmac.c mp_pmac_stubs.c mp_pmac_bit_crc_oracle.c mp_pmac_cck_oracle.c mp_pmac_param_oracle.c mp_pmac_sig_oracle.c mp_pmac_ht_sig_oracle.c $(HOST_JSON) -lm

librust_rtw_mp.a: $(RUST_MP)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rtw_mp_test \
		--crate-type staticlib $(RUST_MP) -o $@

test_mp_pmac_rust: test_mp_pmac.c librust_rtw_mp.a mp_pmac_stubs.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mp_pmac.c mp_pmac_stubs.c $(HOST_JSON) librust_rtw_mp.a -lm

clean:
	rm -f test_mp_pmac test_mp_pmac_rust librust_rtw_mp.a
