# Host L2 differential tests for core/rtw_ap_rest.c (W3-55, W3-73).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
AP_REST_SRC := $(CORE)/rtw_ap_rest.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_AP_REST := $(ROOT)/rust/rtw_ap_rest.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_AP_REST_TEST -DCONFIG_FW_HANDLE_TXBCN -DCONFIG_LIMITED_AP_NUM=4

AP_STA_IE_SRC := $(CORE)/rtw_ap_sta_ie.c
AP_STA_IE_RATES_SRC := $(CORE)/rtw_ap_sta_ie_rates.c
AP_STA_IE_WMM_HT_SRC := $(CORE)/rtw_ap_sta_ie_wmm_ht.c
AP_STA_IE_VHT_MULTIAP_SRC := $(CORE)/rtw_ap_sta_ie_vht_multiap.c
AP_STA_IE_SEC_SRC := $(CORE)/rtw_ap_sta_ie_sec.c
AP_BCN_IE_SRC := $(CORE)/rtw_ap_bcn_ie.c
BCN_UPDATE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 -I$(HOST_INC) -I$(ROOT)/include -include $(HOST_INC)/host_autoconf.h
BCN_UPDATE_DEPS := ap_bcn_update_oracle.c
IE_REST_SRC := $(CORE)/rtw_ieee80211_rest.c
WLAN_UTIL := ../wlan_util
IE_ORACLE := ../ie/ie_c_oracle.c
STA_IE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_AP_STA_IE_TEST
STA_IE_RATES_CFLAGS := $(STA_IE_CFLAGS)
STA_IE_WMM_HT_CFLAGS := $(STA_IE_CFLAGS)
STA_IE_VHT_MULTIAP_CFLAGS := $(STA_IE_CFLAGS) -DCONFIG_RTW_MULTI_AP
STA_IE_SEC_CFLAGS := $(STA_IE_CFLAGS) -DHOST_AP_STA_IE_SEC_TEST \
	-DHOST_IEEE80211_REST_TEST -I$(WLAN_UTIL) -Wno-sign-compare
BCN_IE_CFLAGS := $(STA_IE_CFLAGS) -DHOST_AP_BCN_IE_TEST -Wno-sign-compare -Wno-pointer-sign
BCN_IE_DEPS := ap_bcn_ie_oracle.c
STA_IE_RATES_DEPS := ap_sta_ie_rates_oracle.c $(IE_ORACLE)
STA_IE_SEC_DEPS := ap_sta_ie_sec_oracle.c $(IE_ORACLE) ../ie/ie_cipher_c_oracle.c \
	../ie/ie_wpa_rsn_parse_c_oracle.c $(IE_REST_SRC) \
	$(WLAN_UTIL)/wlan_util_c_oracle.c ../ie/ie_mac_str_host_shim.c
STA_IE_WMM_HT_DEPS := $(IE_ORACLE)
STA_IE_VHT_MULTIAP_DEPS := ap_sta_ie_vht_multiap_oracle.c $(IE_ORACLE)

BMC_RATE_CFLAGS := $(CFLAGS) -DCONFIG_BMC_TX_RATE_SELECT

AP_STA_ALIVE_SRC := $(CORE)/rtw_ap_sta_alive.c
STA_ALIVE_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_STA_ALIVE_TEST -I$(CORE)
AP_EXPIRE_AUTH_SRC := $(CORE)/rtw_ap_expire_auth.c
EXPIRE_AUTH_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_EXPIRE_AUTH_TEST -I$(CORE)
EXPIRE_AUTH_DEPS := host_expire_auth_shim.c
AP_EXPIRE_ASOC_SRC := $(CORE)/rtw_ap_expire_asoc.c $(CORE)/rtw_ap_sta_alive.c
EXPIRE_ASOC_CFLAGS := $(BCN_UPDATE_CFLAGS) -DHOST_AP_EXPIRE_ASOC_TEST -I$(CORE)
AKA_CHK_CFLAGS := $(BCN_UPDATE_CFLAGS) -DCONFIG_ACTIVE_KEEP_ALIVE_CHECK \
	-DHOST_AP_AKA_CHK_TEST -I$(CORE)
AP_AKA_CHK_SRC := $(CORE)/rtw_ap_aka_chk.c
AKA_CHK_DEPS := host_aka_chk_shim.c

.PHONY: all test test-c test-rust test-sta-ie test-bmc-rate test-bcn-update test-sta-alive test-rust-sta-alive test-expire-auth test-rust-expire-auth test-expire-asoc test-rust-expire-asoc test-aka-chk test-rust-aka-chk clean

all: test

test: test-c test-rust test-sta-ie test-bcn-ie test-bcn-update test-sta-alive test-rust-sta-alive test-expire-auth test-rust-expire-auth test-expire-asoc test-rust-expire-asoc test-aka-chk test-rust-aka-chk test-bmc-rate

test-c: test_ap_rest
	./test_ap_rest ap_rest_vectors.json

test_ap_rest: test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)

librust_ap_rest.a: $(RUST_AP_REST)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_ap_rest_test --cfg fw_handle_txbcn \
		--crate-type staticlib $(RUST_AP_REST) -o $@

test-rust: test_ap_rest_rust
	./test_ap_rest_rust ap_rest_vectors.json

test_ap_rest_rust: test_ap_rest.c librust_ap_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_AP_REST_ORACLE -o $@ test_ap_rest.c $(HOST_JSON) librust_ap_rest.a

test-sta-ie: test_ap_sta_ie test_ap_sta_ie_rates test_ap_sta_ie_wmm_ht test_ap_sta_ie_vht_multiap test_ap_sta_ie_sec
	./test_ap_sta_ie ap_sta_ie_vectors.json
	./test_ap_sta_ie_rates
	./test_ap_sta_ie_wmm_ht
	./test_ap_sta_ie_vht_multiap
	./test_ap_sta_ie_sec ap_sta_ie_sec_vectors.json

test-bcn-ie: test_ap_bcn_ie
	./test_ap_bcn_ie ap_bcn_ie_vectors.json

test-bcn-update: test_ap_bcn_update
	./test_ap_bcn_update ap_bcn_update_vectors.json

test-sta-alive: test_ap_sta_alive
	./test_ap_sta_alive ap_sta_alive_vectors.json

test-expire-auth: test_ap_expire_auth
	./test_ap_expire_auth ap_expire_auth_vectors.json

test-rust-expire-auth: test_ap_expire_auth_rust
	./test_ap_expire_auth_rust ap_expire_auth_vectors.json

librust_ap_expire_auth.a: ../../../rust/rtw_ap_expire_auth.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_expire_auth_test \
		--crate-type staticlib ../../../rust/rtw_ap_expire_auth.rs -o $@

test_ap_expire_auth_rust: test_ap_expire_auth.c librust_ap_expire_auth.a $(HOST_JSON) host_expire_auth_shim.c
	$(CC) $(EXPIRE_AUTH_CFLAGS) -DRUST_AP_EXPIRE_AUTH_ORACLE -o $@ test_ap_expire_auth.c \
		$(HOST_JSON) host_expire_auth_shim.c librust_ap_expire_auth.a

test-expire-asoc: test_ap_expire_asoc
	./test_ap_expire_asoc ap_expire_asoc_vectors.json

test-aka-chk: test_ap_aka_chk
	./test_ap_aka_chk ap_aka_chk_vectors.json

test-rust-aka-chk: test_ap_aka_chk_rust
	./test_ap_aka_chk_rust ap_aka_chk_vectors.json

librust_ap_aka_chk.a: ../../../rust/rtw_ap_aka_chk.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_aka_chk_test \
		--crate-type staticlib ../../../rust/rtw_ap_aka_chk.rs -o $@

test_ap_aka_chk_rust: test_ap_aka_chk.c librust_ap_aka_chk.a $(HOST_JSON) host_aka_chk_shim.c
	$(CC) $(AKA_CHK_CFLAGS) -DRUST_AP_AKA_CHK_ORACLE -o $@ test_ap_aka_chk.c \
		$(HOST_JSON) host_aka_chk_shim.c librust_ap_aka_chk.a

test_ap_aka_chk: test_ap_aka_chk.c $(HOST_JSON) $(AP_AKA_CHK_SRC) $(AKA_CHK_DEPS)
	$(CC) $(AKA_CHK_CFLAGS) -o $@ test_ap_aka_chk.c $(HOST_JSON) \
		$(AP_AKA_CHK_SRC) $(AKA_CHK_DEPS)

test-rust-expire-asoc: test_ap_expire_asoc_rust
	./test_ap_expire_asoc_rust ap_expire_asoc_vectors.json

librust_ap_expire_asoc.a: ../../../rust/rtw_ap_expire_asoc.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_expire_asoc_test \
		--crate-type staticlib ../../../rust/rtw_ap_expire_asoc.rs -o $@

test_ap_expire_asoc_rust: test_ap_expire_asoc.c librust_ap_expire_asoc.a $(HOST_JSON) $(CORE)/rtw_ap_sta_alive.c
	$(CC) $(EXPIRE_ASOC_CFLAGS) -DRUST_AP_EXPIRE_ASOC_ORACLE -o $@ test_ap_expire_asoc.c \
		$(HOST_JSON) $(CORE)/rtw_ap_sta_alive.c librust_ap_expire_asoc.a

test_ap_expire_asoc: test_ap_expire_asoc.c $(HOST_JSON) $(AP_EXPIRE_ASOC_SRC)
	$(CC) $(EXPIRE_ASOC_CFLAGS) -o $@ test_ap_expire_asoc.c $(HOST_JSON) \
		$(AP_EXPIRE_ASOC_SRC)

test_ap_expire_auth: test_ap_expire_auth.c $(HOST_JSON) $(AP_EXPIRE_AUTH_SRC) $(EXPIRE_AUTH_DEPS)
	$(CC) $(EXPIRE_AUTH_CFLAGS) -o $@ test_ap_expire_auth.c $(HOST_JSON) \
		$(AP_EXPIRE_AUTH_SRC) $(EXPIRE_AUTH_DEPS)

test-rust-sta-alive: test_ap_sta_alive_rust
	./test_ap_sta_alive_rust ap_sta_alive_vectors.json

librust_ap_sta_alive.a: ../../../rust/rtw_ap_sta_alive.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_ap_sta_alive_test \
		--crate-type staticlib ../../../rust/rtw_ap_sta_alive.rs -o $@

test_ap_sta_alive_rust: test_ap_sta_alive.c librust_ap_sta_alive.a $(HOST_JSON)
	$(CC) $(STA_ALIVE_CFLAGS) -DRUST_AP_STA_ALIVE_ORACLE -o $@ test_ap_sta_alive.c \
		$(HOST_JSON) librust_ap_sta_alive.a

test_ap_sta_alive: test_ap_sta_alive.c $(HOST_JSON) $(AP_STA_ALIVE_SRC)
	$(CC) $(STA_ALIVE_CFLAGS) -o $@ test_ap_sta_alive.c $(HOST_JSON) \
		$(AP_STA_ALIVE_SRC)

test_ap_bcn_update: test_ap_bcn_update.c $(HOST_JSON) $(BCN_UPDATE_DEPS)
	$(CC) $(BCN_UPDATE_CFLAGS) -o $@ test_ap_bcn_update.c $(HOST_JSON) \
		$(BCN_UPDATE_DEPS)

test-bmc-rate: test-c-bmc test-rust-bmc

test-c-bmc: test_ap_bmc_rate
	./test_ap_bmc_rate ap_bmc_rate_vectors.json

test-rust-bmc: test_ap_bmc_rate_rust
	./test_ap_bmc_rate_rust ap_bmc_rate_vectors.json

test_ap_bmc_rate: test_ap_bmc_rate.c ap_bmc_rate_oracle.c $(HOST_JSON)
	$(CC) $(BMC_RATE_CFLAGS) -o $@ test_ap_bmc_rate.c ap_bmc_rate_oracle.c $(HOST_JSON)

librust_ap_bmc_rate.a: $(RUST_AP_REST)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_ap_bmc_rate_test \
		--crate-type staticlib $(RUST_AP_REST) -o $@

test_ap_bmc_rate_rust: test_ap_bmc_rate.c librust_ap_bmc_rate.a $(HOST_JSON)
	$(CC) $(BMC_RATE_CFLAGS) -DRUST_AP_BMC_RATE_ORACLE -o $@ test_ap_bmc_rate.c $(HOST_JSON) librust_ap_bmc_rate.a

test_ap_sta_ie: test_ap_sta_ie.c $(AP_STA_IE_SRC) $(HOST_JSON)
	$(CC) $(STA_IE_CFLAGS) -o $@ test_ap_sta_ie.c $(AP_STA_IE_SRC) $(HOST_JSON)

test_ap_sta_ie_rates: test_ap_sta_ie_rates.c $(AP_STA_IE_RATES_SRC) $(STA_IE_RATES_DEPS)
	$(CC) $(STA_IE_RATES_CFLAGS) -o $@ test_ap_sta_ie_rates.c $(AP_STA_IE_RATES_SRC) \
		$(STA_IE_RATES_DEPS)

test_ap_sta_ie_wmm_ht: test_ap_sta_ie_wmm_ht.c $(AP_STA_IE_WMM_HT_SRC) $(STA_IE_WMM_HT_DEPS)
	$(CC) $(STA_IE_WMM_HT_CFLAGS) -o $@ test_ap_sta_ie_wmm_ht.c $(AP_STA_IE_WMM_HT_SRC) \
		$(STA_IE_WMM_HT_DEPS)

test_ap_sta_ie_vht_multiap: test_ap_sta_ie_vht_multiap.c $(AP_STA_IE_VHT_MULTIAP_SRC) $(STA_IE_VHT_MULTIAP_DEPS)
	$(CC) $(STA_IE_VHT_MULTIAP_CFLAGS) -o $@ test_ap_sta_ie_vht_multiap.c \
		$(AP_STA_IE_VHT_MULTIAP_SRC) $(STA_IE_VHT_MULTIAP_DEPS)

test_ap_sta_ie_sec: test_ap_sta_ie_sec.c $(AP_STA_IE_SEC_SRC) $(HOST_JSON) $(STA_IE_SEC_DEPS)
	$(CC) $(STA_IE_SEC_CFLAGS) -o $@ test_ap_sta_ie_sec.c $(AP_STA_IE_SEC_SRC) \
		$(HOST_JSON) $(STA_IE_SEC_DEPS)

test_ap_bcn_ie: test_ap_bcn_ie.c $(AP_BCN_IE_SRC) $(HOST_JSON) $(BCN_IE_DEPS)
	$(CC) $(BCN_IE_CFLAGS) -o $@ test_ap_bcn_ie.c $(AP_BCN_IE_SRC) \
		$(HOST_JSON) $(BCN_IE_DEPS)

clean:
	rm -f test_ap_rest test_ap_rest_rust test_ap_sta_ie test_ap_sta_ie_rates \
		test_ap_sta_ie_wmm_ht test_ap_sta_ie_vht_multiap test_ap_sta_ie_sec \
		test_ap_bcn_ie test_ap_bcn_update test_ap_sta_alive test_ap_sta_alive_rust \
		test_ap_expire_auth test_ap_expire_auth_rust test_ap_expire_asoc test_ap_expire_asoc_rust \
		test_ap_aka_chk test_ap_aka_chk_rust \
		test_ap_bmc_rate test_ap_bmc_rate_rust \
		librust_ap_rest.a librust_ap_bmc_rate.a librust_ap_sta_alive.a \
		librust_ap_expire_asoc.a librust_ap_expire_auth.a librust_ap_aka_chk.a
