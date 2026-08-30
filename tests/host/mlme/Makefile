# Host L2 differential tests for core/rtw_mlme_rest.c (W3-53).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
WLAN_UTIL := ../wlan_util
MLME_REST_SRC := $(CORE)/rtw_mlme_rest.c
MLME_SHIM := host_mlme_shim.c
WMM_RSN_IE_ORACLE := ../ie/ie_cipher_c_oracle.c ../ie/ie_wpa_rsn_parse_c_oracle.c \
                     ../ie/ie_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                     ../ie/ie_mac_str_host_shim.c
WMM_RSN_MEM_SHIM := host_mlme_wmm_rsn_mem_shim.c
HOST_JSON := ../crypto/host_vector_json.c
IE_REST_SRC := $(CORE)/rtw_ieee80211_rest.c
RUST_MLME_REST := $(ROOT)/rust/rtw_mlme_rest.rs
RUST_MLME_UNASSOC := $(ROOT)/rust/rtw_mlme_unassoc.rs
RUST_MLME_WMM_RSN := $(ROOT)/rust/rtw_mlme_wmm_rsn.rs
RUST_MLME_ROAMING := $(ROOT)/rust/rtw_mlme_roaming.rs
RUST_MLME_NETWORK_UPDATE := $(ROOT)/rust/rtw_mlme_network_update.rs

CC     := gcc
RUSTC  := rustc
CFLAGS_COMMON := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h
CFLAGS := $(CFLAGS_COMMON) -DHOST_MLME_TEST -DCONFIG_P2P

.PHONY: all test test-c test-rust test-mlme-unassoc test-mlme-unassoc-rust test-mlme-unassoc-rust-full test-mlme-wmm-rsn test-mlme-wmm-rsn-rust test-mlme-roaming test-mlme-roaming-rust test-mlme-network-update test-mlme-network-update-rust clean

all: test

test: test-c test-rust test-mlme-unassoc test-mlme-unassoc-rust test-mlme-unassoc-rust-full test-mlme-wmm-rsn test-mlme-wmm-rsn-rust test-mlme-network-update test-mlme-network-update-rust
ifneq ($(wildcard test_mlme_roaming.c),)
test: test-mlme-roaming test-mlme-roaming-rust
endif

test-c: test_mlme
	./test_mlme mlme_vectors.json

test_mlme: test_mlme.c $(MLME_REST_SRC) $(MLME_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mlme.c $(MLME_REST_SRC) $(MLME_SHIM) $(HOST_JSON)

librust_mlme_rest.a: $(RUST_MLME_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mlme_test --cfg config_p2p \
		--crate-type staticlib $(RUST_MLME_REST) -o $@

test-rust: test_mlme_rust
	./test_mlme_rust mlme_vectors.json

test_mlme_rust: test_mlme.c librust_mlme_rest.a $(MLME_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_MLME_ORACLE -o $@ test_mlme.c $(MLME_SHIM) $(HOST_JSON) librust_mlme_rest.a

CFLAGS_UNASSOC := $(CFLAGS_COMMON) -DCONFIG_RTW_MULTI_AP -DHOST_MLME_UNASSOC_TEST

test-mlme-unassoc: test_mlme_unassoc
	./test_mlme_unassoc mlme_unassoc_vectors.json

test_mlme_unassoc: test_mlme_unassoc.c $(MLME_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS_UNASSOC) -o $@ test_mlme_unassoc.c $(MLME_REST_SRC) $(HOST_JSON)

librust_mlme_unassoc.a: $(RUST_MLME_UNASSOC)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mlme_unassoc_test \
		--crate-type staticlib $(RUST_MLME_UNASSOC) -o $@

test-mlme-unassoc-rust: test_mlme_unassoc_rust
	./test_mlme_unassoc_rust mlme_unassoc_del_vectors.json

test-mlme-unassoc-rust-full: test_mlme_unassoc_rust_full
	./test_mlme_unassoc_rust_full mlme_unassoc_vectors.json

test_mlme_unassoc_rust: test_mlme_unassoc.c librust_mlme_unassoc.a $(HOST_JSON)
	$(CC) $(CFLAGS_UNASSOC) -DRUST_MLME_UNASSOC_ORACLE -DRUST_MLME_UNASSOC_DEL_ONLY -o $@ test_mlme_unassoc.c $(HOST_JSON) librust_mlme_unassoc.a

test_mlme_unassoc_rust_full: test_mlme_unassoc.c librust_mlme_unassoc.a $(HOST_JSON)
	$(CC) $(CFLAGS_UNASSOC) -DRUST_MLME_UNASSOC_ORACLE -o test_mlme_unassoc_rust_full test_mlme_unassoc.c $(HOST_JSON) librust_mlme_unassoc.a

CFLAGS_WMM_RSN := $(CFLAGS_COMMON) -DHOST_MLME_WMM_RSN_TEST -DCONFIG_WMMPS_STA \
                  -DHOST_IEEE80211_REST_TEST -I$(WLAN_UTIL) -I$(ROOT)/include

test-mlme-wmm-rsn: test_mlme_wmm_rsn
	./test_mlme_wmm_rsn mlme_wmm_rsn_vectors.json

test_mlme_wmm_rsn: test_mlme_wmm_rsn.c $(MLME_REST_SRC) $(IE_REST_SRC) $(WMM_RSN_IE_ORACLE) \
		$(HOST_JSON)
	$(CC) $(CFLAGS_WMM_RSN) -o $@ test_mlme_wmm_rsn.c $(MLME_REST_SRC) $(IE_REST_SRC) \
		$(WMM_RSN_IE_ORACLE) $(HOST_JSON)

librust_mlme_wmm_rsn.a: $(RUST_MLME_WMM_RSN)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mlme_wmm_rsn_test \
		--cfg config_wmmps_sta --crate-type staticlib $(RUST_MLME_WMM_RSN) -o $@

test-mlme-wmm-rsn-rust: test_mlme_wmm_rsn_rust
	./test_mlme_wmm_rsn_rust mlme_wmm_rsn_vectors.json

test_mlme_wmm_rsn_rust: test_mlme_wmm_rsn.c librust_mlme_wmm_rsn.a $(IE_REST_SRC) \
		$(WMM_RSN_IE_ORACLE) $(WMM_RSN_MEM_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS_WMM_RSN) -DRUST_MLME_WMM_RSN_ORACLE -o $@ test_mlme_wmm_rsn.c \
		$(IE_REST_SRC) $(WMM_RSN_IE_ORACLE) $(WMM_RSN_MEM_SHIM) $(HOST_JSON) librust_mlme_wmm_rsn.a

CFLAGS_ROAMING := $(CFLAGS_COMMON) -DCONFIG_LAYER2_ROAMING -DHOST_MLME_ROAMING_TEST

test-mlme-roaming: test_mlme_roaming
	./test_mlme_roaming mlme_roaming_vectors.json

test_mlme_roaming: test_mlme_roaming.c $(MLME_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS_ROAMING) -o $@ test_mlme_roaming.c $(MLME_REST_SRC) $(HOST_JSON)

librust_mlme_roaming.a: $(RUST_MLME_ROAMING)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mlme_roaming_test \
		--crate-type staticlib $(RUST_MLME_ROAMING) -o $@

test-mlme-roaming-rust: test_mlme_roaming_rust
	./test_mlme_roaming_rust mlme_roaming_vectors.json

test_mlme_roaming_rust: test_mlme_roaming.c librust_mlme_roaming.a $(HOST_JSON)
	$(CC) $(CFLAGS_ROAMING) -DRUST_MLME_ROAMING_ORACLE -o $@ test_mlme_roaming.c $(HOST_JSON) librust_mlme_roaming.a

CFLAGS_NETWORK_UPDATE := $(CFLAGS_COMMON)

test-mlme-network-update: test_mlme_network_update
	./test_mlme_network_update

test_mlme_network_update: test_mlme_network_update.c
	$(CC) $(CFLAGS_NETWORK_UPDATE) -o $@ $<

librust_mlme_network_update.a: $(RUST_MLME_NETWORK_UPDATE)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mlme_network_update_test \
		--crate-type staticlib $(RUST_MLME_NETWORK_UPDATE) -o $@

test-mlme-network-update-rust: test_mlme_network_update_rust
	./test_mlme_network_update_rust

test_mlme_network_update_rust: test_mlme_network_update.c librust_mlme_network_update.a
	$(CC) $(CFLAGS_NETWORK_UPDATE) -DRUST_MLME_NETWORK_UPDATE_ORACLE -o $@ test_mlme_network_update.c librust_mlme_network_update.a

clean:
	rm -f test_mlme test_mlme_rust test_mlme_unassoc test_mlme_unassoc_rust test_mlme_unassoc_rust_full test_mlme_wmm_rsn test_mlme_wmm_rsn_rust test_mlme_roaming test_mlme_roaming_rust test_mlme_network_update test_mlme_network_update_rust librust_mlme_rest.a librust_mlme_unassoc.a librust_mlme_wmm_rsn.a librust_mlme_roaming.a librust_mlme_network_update.a
