ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_tdls_lifecycle_shim.c host_tdls_adapter_shim.c
HT_CAP_SHIM := host_tdls_ht_cap_shim.c
VHT_SHIM := host_tdls_vht_shim.c host_tdls_vht_helper_shim.c
VHT_MCS_ORACLE := ../vht/vht_mcs_rate_c_oracle.c
RUST_TDLS := $(ROOT)/rust/rtw_tdls.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) \
          -DCONFIG_TDLS -DCONFIG_TDLS_CH_SW
HT_CAP_CFLAGS := $(CFLAGS) -DCONFIG_80211N_HT
VHT_CAP_CFLAGS := $(CFLAGS) -DCONFIG_80211AC_VHT

.PHONY: all test test-c test-rust test-ht-cap test-vht-cap clean
all: test
test: test-c test-rust test-ht-cap test-vht-cap

test-c: test_tdls_lifecycle
	./test_tdls_lifecycle tdls_lifecycle_vectors.json

test-rust: test_tdls_lifecycle_rust
	./test_tdls_lifecycle_rust tdls_lifecycle_vectors.json

test-ht-cap: test_tdls_ht_cap
	./test_tdls_ht_cap tdls_ht_cap_vectors.json

test-vht-cap: test_tdls_vht_cap
	./test_tdls_vht_cap tdls_vht_cap_vectors.json

test_tdls_vht_cap: test_tdls_vht_cap.c $(VHT_SHIM) $(VHT_MCS_ORACLE) $(HOST_JSON)
	$(CC) $(VHT_CAP_CFLAGS) -o $@ test_tdls_vht_cap.c $(VHT_SHIM) $(VHT_MCS_ORACLE) $(HOST_JSON)

test_tdls_ht_cap: test_tdls_ht_cap.c $(HT_CAP_SHIM) $(HOST_JSON)
	$(CC) $(HT_CAP_CFLAGS) -o $@ test_tdls_ht_cap.c $(HT_CAP_SHIM) $(HOST_JSON)

test_tdls_lifecycle: test_tdls_lifecycle.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_tdls_lifecycle.c $(SHIM) $(HOST_JSON)

librust_tdls.a: $(RUST_TDLS)
	$(RUSTC) -C opt-level=2 --cfg host_tdls_test --crate-type staticlib $(RUST_TDLS) -o $@

test_tdls_lifecycle_rust: test_tdls_lifecycle.c librust_tdls.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_tdls_lifecycle.c $(HOST_JSON) librust_tdls.a

clean:
	rm -f test_tdls_lifecycle test_tdls_lifecycle_rust test_tdls_ht_cap test_tdls_vht_cap librust_tdls.a
