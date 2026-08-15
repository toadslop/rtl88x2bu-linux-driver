# Host L2 differential tests for core/rtw_ieee80211.c IE helpers (W3-03)
# and core/rtw_ieee80211_rest.c rate helpers (W3-26), cipher getters (W3-27),
# WPA/RSN IE parse (W3-28), and WAPI/WPS/sec-IE getters (W3-29), and
# string/MAC address helpers (W3-30), and chbw grouping/sync (W3-31), and
# frame header / HT MCS helpers (W3-32), and P2P IE merge/delete (W3-43), and
# WFD/multi-AP IE helpers (W3-44).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
WLAN_UTIL := ../wlan_util
RUST_IE  := $(ROOT)/rust/rtw_ieee80211.rs
RUST_IE_REST := $(ROOT)/rust/rtw_ieee80211_rest.rs
IE_REST_SRC := $(CORE)/rtw_ieee80211_rest.c
RF_REST_SRC := $(CORE)/rtw_rf_rest.c

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I$(CORE) -I$(WLAN_UTIL) \
           -DHOST_IE_TEST
REST_CFLAGS := $(CFLAGS) -DHOST_IEEE80211_REST_TEST
CHBW_CFLAGS := $(REST_CFLAGS) -I$(ROOT)/include \
           -include $(HOST_INC)/host_autoconf.h -DHOST_RF_TEST \
           -DHOST_IEEE80211_REST_CHBW_TEST -Wno-format-truncation
FRAME_HT_CFLAGS := $(REST_CFLAGS) -DHOST_IEEE80211_REST_FRAME_HT_TEST
RATE_SECTION_CFLAGS := $(REST_CFLAGS) -DHOST_IEEE80211_REST_RATE_SECTION_TEST
HT_MCS_AMSDU_CFLAGS := $(REST_CFLAGS) -DHOST_IEEE80211_REST_HT_MCS_AMSDU_TEST
P2P_IE_CFLAGS := $(REST_CFLAGS) -DCONFIG_P2P -DHOST_IEEE80211_REST_P2P_IE_TEST
WFD_MULTIAP_CFLAGS := $(REST_CFLAGS) -DCONFIG_WFD -DCONFIG_RTW_MULTI_AP \
	-DHOST_IEEE80211_REST_WFD_MULTIAP_TEST
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on

IE_SRCS := ie_c_oracle.c ../crypto/host_vector_json.c
IE_REST_SRCS := $(IE_REST_SRC) ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
                ie_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                ../crypto/host_vector_json.c ie_mac_str_host_shim.c

IE_REST_CIPHER_SRCS := $(IE_REST_SRCS)

IE_REST_WPA_RSN_PARSE_SRCS := $(IE_REST_SRCS)

IE_REST_WAPI_WPS_SRCS := $(IE_REST_SRCS)

IE_REST_MAC_STR_SRCS := $(IE_REST_SRCS)

IE_REST_CHBW_SRCS := $(IE_REST_SRC) ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
                ie_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                ../crypto/host_vector_json.c ie_mac_str_host_shim.c \
                $(RF_REST_SRC)

IE_REST_FRAME_HT_SRCS := $(IE_REST_SRCS)

IE_REST_RATE_SECTION_SRCS := $(IE_REST_SRC) ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
                ie_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                ../crypto/host_vector_json.c ie_mac_str_host_shim.c

IE_REST_HT_MCS_AMSDU_SRCS := $(IE_REST_SRC) ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
                ie_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                ../crypto/host_vector_json.c ie_mac_str_host_shim.c

IE_REST_P2P_IE_SRCS := $(IE_REST_SRC) ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
                ie_c_oracle.c ie_p2p_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                ../crypto/host_vector_json.c ie_mac_str_host_shim.c

IE_REST_WFD_MULTIAP_SRCS := $(IE_REST_SRC) ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
                ie_c_oracle.c ie_wfd_multiap_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                ../crypto/host_vector_json.c ie_mac_str_host_shim.c

IE_REST_RUST_SHIM := ie_mac_str_host_shim.c ie_p2p_c_oracle.c ie_wfd_multiap_c_oracle.c

.PHONY: all test test-c test-rust test-rest test-rest-rust test-rest-cipher \
	test-rest-wpa-rsn-parse test-rest-wpa-rsn-parse-rust test-rest-wapi-wps \
	test-rest-wapi-wps-rust test-rest-mac-str test-rest-mac-str-rust \
	test-rest-chbw test-rest-chbw-rust test-rest-frame-ht test-rest-frame-ht-rust \
	test-rest-rate-section test-rest-rate-section-rust \
	test-rest-ht-mcs-amsdu test-rest-ht-mcs-amsdu-rust \
	test-rest-p2p-ie test-rest-p2p-ie-rust \
	test-rest-wfd-multiap test-rest-wfd-multiap-rust clean

all: test

test: test-c test-rust test-rest test-rest-rust test-rest-cipher \
	test-rest-cipher-rust test-rest-wpa-rsn-parse test-rest-wpa-rsn-parse-rust \
	test-rest-wapi-wps test-rest-wapi-wps-rust test-rest-mac-str \
	test-rest-mac-str-rust 	test-rest-chbw test-rest-frame-ht test-rest-frame-ht-rust \
	test-rest-rate-section test-rest-rate-section-rust \
	test-rest-ht-mcs-amsdu test-rest-ht-mcs-amsdu-rust \
	test-rest-p2p-ie test-rest-p2p-ie-rust \
	test-rest-wfd-multiap test-rest-wfd-multiap-rust

test-c: test_ie
	./test_ie ie_vectors.json

test_ie: test_ie.c $(IE_SRCS)
	$(CC) $(CFLAGS) -o $@ test_ie.c $(IE_SRCS)

librust_ie.a: $(RUST_IE)
	$(RUSTC) $(RUSTFLAGS) --cfg host_ie_test --crate-type staticlib \
		$(RUST_IE) -o $@

test-rust: test_ie_rust
	./test_ie_rust ie_vectors.json

test_ie_rust: test_ie.c librust_ie.a ../crypto/host_vector_json.c
	$(CC) $(CFLAGS) -DRUST_IE_ORACLE -o $@ test_ie.c ../crypto/host_vector_json.c \
		librust_ie.a

test-rest: test_ie_rest
	./test_ie_rest ie_rest_vectors.json

test_ie_rest: test_ie_rest.c $(IE_REST_SRCS)
	$(CC) $(REST_CFLAGS) -o $@ test_ie_rest.c $(IE_REST_SRCS)

librust_ie_rest.a: $(RUST_IE_REST)
	$(RUSTC) $(RUSTFLAGS) --cfg host_ieee80211_rest_test --crate-type staticlib \
		$(RUST_IE_REST) -o $@

test-rest-rust: test_ie_rest_rust
	./test_ie_rest_rust ie_rest_vectors.json

test-rest-cipher: test_ie_rest_cipher
	./test_ie_rest_cipher ie_rest_cipher_vectors.json

test-rest-cipher-rust: test_ie_rest_cipher_rust
	./test_ie_rest_cipher_rust ie_rest_cipher_vectors.json

test_ie_rest_cipher: test_ie_rest_cipher.c $(IE_REST_CIPHER_SRCS)
	$(CC) $(REST_CFLAGS) -o $@ test_ie_rest_cipher.c $(IE_REST_CIPHER_SRCS)

test_ie_rest_cipher_rust: test_ie_rest_cipher.c librust_ie_rest.a ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(REST_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ test_ie_rest_cipher.c \
		ie_c_oracle.c ../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-wpa-rsn-parse: test_ie_rest_wpa_rsn_parse
	./test_ie_rest_wpa_rsn_parse ie_rest_wpa_rsn_parse_vectors.json

test_ie_rest_wpa_rsn_parse: test_ie_rest_wpa_rsn_parse.c $(IE_REST_WPA_RSN_PARSE_SRCS)
	$(CC) $(REST_CFLAGS) -o $@ test_ie_rest_wpa_rsn_parse.c $(IE_REST_WPA_RSN_PARSE_SRCS)

test-rest-wpa-rsn-parse-rust: test_ie_rest_wpa_rsn_parse_rust
	./test_ie_rest_wpa_rsn_parse_rust ie_rest_wpa_rsn_parse_vectors.json

test_ie_rest_wpa_rsn_parse_rust: test_ie_rest_wpa_rsn_parse.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(REST_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ test_ie_rest_wpa_rsn_parse.c \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-wapi-wps: test_ie_rest_wapi_wps
	./test_ie_rest_wapi_wps ie_rest_wapi_wps_vectors.json

test_ie_rest_wapi_wps: test_ie_rest_wapi_wps.c $(IE_REST_WAPI_WPS_SRCS)
	$(CC) $(REST_CFLAGS) -o $@ test_ie_rest_wapi_wps.c $(IE_REST_WAPI_WPS_SRCS)

test-rest-wapi-wps-rust: test_ie_rest_wapi_wps_rust
	./test_ie_rest_wapi_wps_rust ie_rest_wapi_wps_vectors.json

test_ie_rest_wapi_wps_rust: test_ie_rest_wapi_wps.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(REST_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ test_ie_rest_wapi_wps.c \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-mac-str: test_ie_rest_mac_str
	./test_ie_rest_mac_str ie_rest_mac_str_vectors.json

test_ie_rest_mac_str: test_ie_rest_mac_str.c $(IE_REST_MAC_STR_SRCS)
	$(CC) $(REST_CFLAGS) -o $@ test_ie_rest_mac_str.c $(IE_REST_MAC_STR_SRCS)

test-rest-mac-str-rust: test_ie_rest_mac_str_rust
	./test_ie_rest_mac_str_rust ie_rest_mac_str_vectors.json

test_ie_rest_mac_str_rust: test_ie_rest_mac_str.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(REST_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ test_ie_rest_mac_str.c \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-chbw: test_ie_rest_chbw
	./test_ie_rest_chbw ie_rest_chbw_vectors.json

test_ie_rest_chbw: test_ie_rest_chbw.c $(IE_REST_CHBW_SRCS)
	$(CC) $(CHBW_CFLAGS) -o $@ test_ie_rest_chbw.c $(IE_REST_CHBW_SRCS)

test-rest-chbw-rust: test_ie_rest_chbw_rust
	./test_ie_rest_chbw_rust ie_rest_chbw_vectors.json

test_ie_rest_chbw_rust: test_ie_rest_chbw.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) $(RF_REST_SRC)
	$(CC) $(CHBW_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ test_ie_rest_chbw.c \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) $(RF_REST_SRC) librust_ie_rest.a

test-rest-frame-ht: test_ie_rest_frame_ht
	./test_ie_rest_frame_ht ie_rest_frame_ht_vectors.json

test_ie_rest_frame_ht: test_ie_rest_frame_ht.c $(IE_REST_FRAME_HT_SRCS)
	$(CC) $(FRAME_HT_CFLAGS) -o $@ test_ie_rest_frame_ht.c $(IE_REST_FRAME_HT_SRCS)

test-rest-frame-ht-rust: test_ie_rest_frame_ht_rust
	./test_ie_rest_frame_ht_rust ie_rest_frame_ht_vectors.json

test_ie_rest_frame_ht_rust: test_ie_rest_frame_ht.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(FRAME_HT_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ test_ie_rest_frame_ht.c \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-rate-section: test_ie_rest_rate_section
	./test_ie_rest_rate_section ie_rest_rate_section_vectors.json

test_ie_rest_rate_section: test_ie_rest_rate_section.c $(IE_REST_RATE_SECTION_SRCS)
	$(CC) $(RATE_SECTION_CFLAGS) -o $@ test_ie_rest_rate_section.c \
		$(IE_REST_RATE_SECTION_SRCS)

test-rest-rate-section-rust: test_ie_rest_rate_section_rust
	./test_ie_rest_rate_section_rust ie_rest_rate_section_vectors.json

test_ie_rest_rate_section_rust: test_ie_rest_rate_section.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(RATE_SECTION_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ \
		test_ie_rest_rate_section.c ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
		ie_c_oracle.c ../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-ht-mcs-amsdu: test_ie_rest_ht_mcs_amsdu
	./test_ie_rest_ht_mcs_amsdu ie_rest_ht_mcs_amsdu_vectors.json

test_ie_rest_ht_mcs_amsdu: test_ie_rest_ht_mcs_amsdu.c $(IE_REST_HT_MCS_AMSDU_SRCS)
	$(CC) $(HT_MCS_AMSDU_CFLAGS) -o $@ test_ie_rest_ht_mcs_amsdu.c \
		$(IE_REST_HT_MCS_AMSDU_SRCS)

test-rest-ht-mcs-amsdu-rust: test_ie_rest_ht_mcs_amsdu_rust
	./test_ie_rest_ht_mcs_amsdu_rust ie_rest_ht_mcs_amsdu_vectors.json

test_ie_rest_ht_mcs_amsdu_rust: test_ie_rest_ht_mcs_amsdu.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(HT_MCS_AMSDU_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ \
		test_ie_rest_ht_mcs_amsdu.c ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
		ie_c_oracle.c ../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test_ie_rest_rust: test_ie_rest.c librust_ie_rest.a ../wlan_util/wlan_util_c_oracle.c \
		../crypto/host_vector_json.c ie_c_oracle.c $(IE_REST_RUST_SHIM)
	$(CC) $(REST_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ test_ie_rest.c \
		../crypto/host_vector_json.c ie_c_oracle.c ../wlan_util/wlan_util_c_oracle.c \
		$(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-p2p-ie: test_ie_rest_p2p_ie
	./test_ie_rest_p2p_ie ie_rest_p2p_ie_vectors.json

test_ie_rest_p2p_ie: test_ie_rest_p2p_ie.c $(IE_REST_P2P_IE_SRCS)
	$(CC) $(P2P_IE_CFLAGS) -o $@ test_ie_rest_p2p_ie.c $(IE_REST_P2P_IE_SRCS)

test-rest-p2p-ie-rust: test_ie_rest_p2p_ie_rust
	./test_ie_rest_p2p_ie_rust ie_rest_p2p_ie_vectors.json

test_ie_rest_p2p_ie_rust: test_ie_rest_p2p_ie.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(P2P_IE_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ \
		test_ie_rest_p2p_ie.c ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
		ie_c_oracle.c ../wlan_util/wlan_util_c_oracle.c \
		../crypto/host_vector_json.c $(IE_REST_RUST_SHIM) librust_ie_rest.a

test-rest-wfd-multiap: test_ie_rest_wfd_multiap
	./test_ie_rest_wfd_multiap ie_rest_wfd_multiap_vectors.json

test_ie_rest_wfd_multiap: test_ie_rest_wfd_multiap.c $(IE_REST_WFD_MULTIAP_SRCS)
	$(CC) $(WFD_MULTIAP_CFLAGS) -o $@ test_ie_rest_wfd_multiap.c \
		$(IE_REST_WFD_MULTIAP_SRCS)

test-rest-wfd-multiap-rust: test_ie_rest_wfd_multiap_rust
	./test_ie_rest_wfd_multiap_rust ie_rest_wfd_multiap_vectors.json

test_ie_rest_wfd_multiap_rust: test_ie_rest_wfd_multiap.c librust_ie_rest.a \
		ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c ie_c_oracle.c \
		../wlan_util/wlan_util_c_oracle.c ../crypto/host_vector_json.c \
		$(IE_REST_RUST_SHIM)
	$(CC) $(WFD_MULTIAP_CFLAGS) -DRUST_IEEE80211_REST_ORACLE -o $@ \
		test_ie_rest_wfd_multiap.c ie_cipher_c_oracle.c ie_wpa_rsn_parse_c_oracle.c \
		ie_c_oracle.c ../wlan_util/wlan_util_c_oracle.c \
		../crypto/host_vector_json.c $(IE_REST_RUST_SHIM) librust_ie_rest.a

clean:
	rm -f test_ie test_ie_rust test_ie_rest test_ie_rest_rust test_ie_rest_cipher \
		test_ie_rest_cipher_rust test_ie_rest_wpa_rsn_parse \
		test_ie_rest_wpa_rsn_parse_rust test_ie_rest_wapi_wps \
		test_ie_rest_wapi_wps_rust test_ie_rest_mac_str \
		test_ie_rest_mac_str_rust test_ie_rest_chbw \
		test_ie_rest_chbw_rust test_ie_rest_frame_ht \
		test_ie_rest_frame_ht_rust test_ie_rest_rate_section \
		test_ie_rest_rate_section_rust test_ie_rest_ht_mcs_amsdu \
		test_ie_rest_ht_mcs_amsdu_rust test_ie_rest_p2p_ie \
		test_ie_rest_p2p_ie_rust test_ie_rest_wfd_multiap \
		test_ie_rest_wfd_multiap_rust librust_ie.a librust_ie_rest.a
