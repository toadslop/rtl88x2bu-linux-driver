# Host L2 differential tests for core/rtw_vht_rest.c (W3-35, W3-36).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
VHT_REST_SRC := $(CORE)/rtw_vht_rest.c
VHT_REST_SHIM := host_vht_rest_shim.c
HOST_JSON := ../crypto/host_vector_json.c
VHT_MCS_RATE_ORACLE := vht_mcs_rate_c_oracle.c
VHT_VHT20_ORACLE := vht_vht20_c_oracle.c
VHT_MCS_SHIM := host_vht_mcs_shim.c
IE_DIR := ../ie
WLAN_UTIL := ../wlan_util
IE_REST_SRC := $(CORE)/rtw_ieee80211_rest.c
RF_REST_SRC := $(CORE)/rtw_rf_rest.c
VHT20_DEPS := $(IE_DIR)/ie_c_oracle.c $(IE_DIR)/ie_cipher_c_oracle.c \
	$(IE_DIR)/ie_wpa_rsn_parse_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
	$(IE_DIR)/ie_mac_str_host_shim.c $(IE_REST_SRC) $(RF_REST_SRC)
RUST_VHT := $(ROOT)/rust/rtw_vht.rs

CC     := gcc
BASE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DROKU_PRIVATE
CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_TEST
MCS_RATE_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_MCS_RATE_TEST
VHT20_CFLAGS := $(BASE_CFLAGS) -I$(WLAN_UTIL) -Wno-format-truncation \
	-DHOST_IEEE80211_REST_TEST -DHOST_IEEE80211_REST_CHBW_TEST \
	-DHOST_VHT_MCS_RATE_TEST -DHOST_RF_TEST
RESTRUCT_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_RESTRUCTURE_TEST -Wno-pointer-sign

.PHONY: all test test-c test-rust test-mcs-rate test-mcs-rate-rust test-vht20 \
	vht-restructure-compile test-restructure test-restructure-rust clean

all: test

test: test-c test-rust test-mcs-rate test-vht20 test-restructure test-restructure-rust

test-c: test_vht
	./test_vht vht_vectors.json

test_vht: test_vht.c $(VHT_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_vht.c $(VHT_REST_SRC) $(HOST_JSON)

librust_vht.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-rust: test_vht_rust
	./test_vht_rust vht_vectors.json

test_vht_rust: test_vht.c librust_vht.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_VHT_ORACLE -o $@ test_vht.c $(HOST_JSON) librust_vht.a

test-mcs-rate: test_vht_mcs_rate
	./test_vht_mcs_rate vht_mcs_rate_vectors.json

test_vht_mcs_rate: test_vht_mcs_rate.c $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(MCS_RATE_CFLAGS) -o $@ test_vht_mcs_rate.c $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)

test-vht20: test_vht_vht20
	./test_vht_vht20 vht_vht20_vectors.json

test_vht_vht20: test_vht_vht20.c $(VHT_VHT20_ORACLE) $(VHT_MCS_SHIM) $(VHT20_DEPS) $(HOST_JSON)
	$(CC) $(VHT20_CFLAGS) -o $@ test_vht_vht20.c $(VHT_VHT20_ORACLE) $(VHT_MCS_SHIM) \
		$(VHT20_DEPS) $(HOST_JSON)

vht-restructure-compile: $(VHT_REST_SRC) $(VHT_REST_SHIM)
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SRC) -o vht_rest_host.o
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SHIM) -o vht_rest_shim_host.o

librust_vht_restructure.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_restructure_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-restructure-rust: test_vht_restructure_rust
	./test_vht_restructure_rust vht_restructure_vectors.json

test_vht_restructure_rust: test_vht_restructure.c librust_vht_restructure.a $(VHT_REST_SHIM) $(HOST_JSON)
	$(CC) $(RESTRUCT_CFLAGS) -DRUST_VHT_ORACLE -o $@ test_vht_restructure.c $(VHT_REST_SHIM) $(HOST_JSON) librust_vht_restructure.a

test-restructure: test_vht_restructure
	./test_vht_restructure vht_restructure_vectors.json

test_vht_restructure: test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)
	$(CC) $(RESTRUCT_CFLAGS) -o $@ test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)

clean:
	rm -f test_vht test_vht_rust test_vht_mcs_rate test_vht_vht20 test_vht_restructure \
		test_vht_restructure_rust librust_vht.a librust_vht_restructure.a \
		vht_rest_host.o vht_rest_shim_host.o
