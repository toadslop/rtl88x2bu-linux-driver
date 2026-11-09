# Host L2 differential tests for rtw_debug (test hooks + sec_cam formatters).

ROOT      := ../../..
HOST_INC  := ../include
HOST_JSON := ../crypto/host_vector_json.c

# W3-138 test hooks (proc/debug)
RUST_HOOKS := $(ROOT)/rust/rtw_debug_test_hooks.rs
HOOKS_ORACLE := rtw_debug_test_hooks_c_leaf.c
HOOKS_SHIM := host_rtw_debug_test_hooks_shim.c
HOOKS_L2 := test_debug_test_hooks_l2.c

# W3-133 sec_cam dump formatters
SEC_ORACLE := debug_sec_cam_ent_c_oracle.c
SEC_SHIM   := host_rtw_debug_sec_cam_security_shim.c
RUST_SEC   := $(ROOT)/rust/rtw_debug_sec_cam.rs

# W3-170 regsty RX AMPDU limit dump
REGSTY_ORACLE := debug_regsty_rx_ampdu_c_oracle.c
REGSTY_SHIM   := host_rtw_debug_regsty_shim.c
REGSTY_RUST   := $(ROOT)/rust/rtw_debug_regsty.rs
REGSTY_L2     := test_dump_regsty_rx_ampdu.c

# W3-210 PHY cap dump helpers (default/driver)
PHY_CAP_ORACLE := debug_phy_cap_c_oracle.c
PHY_CAP_SHIM   := host_rtw_debug_phy_cap_shim.c
PHY_CAP_RUST   := $(ROOT)/rust/rtw_debug_phy_cap.rs
PHY_CAP_L2     := test_debug_phy_cap_dump.c
PHY_CAP_CFG    := -DCONFIG_80211N_HT -DCONFIG_80211AC_VHT -DCONFIG_BEAMFORMING

# W3-211 proc PHY cap registry get/set (stbc/rx_stbc/ldpc/txbf)
PHY_REG_ORACLE := debug_proc_phy_cap_registry_c_oracle.c
PHY_REG_SHIM   := host_rtw_debug_proc_phy_cap_registry_shim.c
PHY_REG_RUST   := $(ROOT)/rust/rtw_debug_proc_phy_cap.rs
PHY_REG_L2     := test_proc_phy_cap_registry.c
PHY_REG_CFG    := -DCONFIG_80211N_HT -DCONFIG_BEAMFORMING

# W3-216 proc rf4ce_state + sreset get/set
SRESET_ORACLE := debug_proc_sreset_c_oracle.c
SRESET_SHIM   := host_rtw_debug_proc_sreset_shim.c
SRESET_RUST   := $(ROOT)/rust/rtw_debug_proc_sreset.rs
SRESET_L2     := test_proc_sreset.c
SRESET_CFG    := -DCONFIG_BT_COEXIST -DCONFIG_RF4CE_COEXIST -DDBG_CONFIG_ERROR_DETECT

# W3-215 proc btcoex dbg get/set + info
BTCOEX_ORACLE := debug_proc_btcoex_c_oracle.c
BTCOEX_SHIM   := host_rtw_debug_proc_btcoex_shim.c
BTCOEX_RUST   := $(ROOT)/rust/rtw_debug_proc_btcoex.rs
BTCOEX_L2     := test_proc_btcoex.c
BTCOEX_CFG    := -DCONFIG_BT_COEXIST

# W3-214 proc malloc cnt + best channel
MISC_ORACLE := debug_proc_misc_c_oracle.c
MISC_SHIM   := host_rtw_debug_proc_misc_shim.c
MISC_RUST   := $(ROOT)/rust/rtw_debug_proc_misc.rs
MISC_L2     := test_proc_misc.c
MISC_CFG    := -DDBG_MEMORY_LEAK -DCONFIG_FIND_BEST_CHANNEL

# W3-227 proc ACK timeout (CONFIG_PROC_DEBUG)
ACK_ORACLE := debug_proc_ack_c_oracle.c
ACK_HOST   := debug_proc_ack_host_core.c
ACK_SHIM   := host_rtw_debug_proc_ack_shim.c
ACK_RUST   := $(ROOT)/rust/rtw_debug_proc_ack.rs
ACK_L2     := test_proc_ack.c
ACK_CFG    := -DCONFIG_PROC_DEBUG
ACK_CFG_8821C := $(ACK_CFG) -DCONFIG_RTL8821C

# W3-224 proc TXSS by TP (CONFIG_CTRL_TXSS_BY_TP)
TXSS_ORACLE := debug_proc_txss_c_oracle.c
TXSS_HOST   := debug_proc_txss_host_core.c
TXSS_SHIM   := host_rtw_debug_proc_txss_shim.c
TXSS_RUST   := $(ROOT)/rust/rtw_debug_proc_txss.rs
TXSS_L2     := test_proc_txss.c
TXSS_CFG    := -DCONFIG_CTRL_TXSS_BY_TP -DDBG_CTRL_TXSS

# W3-222 proc MCC registry setters (CONFIG_MCC_MODE)
MCC_ORACLE := debug_proc_mcc_c_oracle.c
MCC_HOST   := debug_proc_mcc_host_core.c
MCC_SHIM   := host_rtw_debug_proc_mcc_shim.c
MCC_RUST   := $(ROOT)/rust/rtw_debug_proc_mcc.rs
MCC_L2     := test_proc_mcc_setters.c
MCC_CFG    := -DCONFIG_MCC_MODE -DCONFIG_MCC_PHYDM_OFFLOAD

# W3-225 proc LPS check-by-TP (CONFIG_LPS_CHK_BY_TP)
LPS_TP_ORACLE := debug_proc_lps_tp_c_oracle.c
LPS_TP_HOST   := debug_proc_lps_tp_host_core.c
LPS_TP_SHIM   := host_rtw_debug_proc_lps_tp_shim.c
LPS_TP_RUST   := $(ROOT)/rust/rtw_debug_proc_lps_tp.rs
LPS_TP_L2     := test_proc_lps_tp.c
LPS_TP_CFG    := -DCONFIG_LPS_CHK_BY_TP

# W3-220 proc monitor / simple_config / xmit_block
MONITOR_ORACLE := debug_proc_monitor_c_oracle.c
MONITOR_SHIM   := host_rtw_debug_proc_monitor_shim.c
MONITOR_RUST   := $(ROOT)/rust/rtw_debug_proc_monitor.rs
MONITOR_L2     := test_proc_monitor.c
MONITOR_CFG    := -DRTW_SIMPLE_CONFIG -DDBG_XMIT_BLOCK

# W3-219 proc ps_info / wmmps / new_bcn_max
PS_INFO_ORACLE := debug_proc_ps_info_c_oracle.c
PS_INFO_SHIM := host_rtw_debug_proc_ps_info_shim.c
PS_INFO_RUST := $(ROOT)/rust/rtw_debug_proc_ps_info.rs
PS_INFO_L2 := test_proc_ps_info.c
PS_INFO_CFG := -DCONFIG_PROC_DEBUG -DCONFIG_BCN_CNT_CONFIRM_HDL -DCONFIG_FWLPS_IN_IPS -DCONFIG_WOWLAN

# W3-203 proc HT registry get/set
HT_ORACLE := debug_proc_ht_c_oracle.c
HT_SHIM   := host_rtw_debug_proc_ht_shim.c
HT_RUST   := $(ROOT)/rust/rtw_debug_proc_ht.rs
HT_L2     := test_proc_ht_registry.c

# W3-217 proc WoWLAN registry + GPIO (CONFIG_WOWLAN + optional stubs)
WOW_ORACLE := debug_proc_wow_c_oracle.c
WOW_SHIM   := host_rtw_debug_proc_wow_shim.c
WOW_PATTERN := host_rtw_wowlan_pattern_cmd.c
WOW_RUST   := $(ROOT)/rust/rtw_debug_proc_wow.rs
WOW_L2     := test_proc_wow_registry.c
WOW_CFG    := -DCONFIG_WOWLAN

# W3-208 proc vht_24g_enable get/set
VHT_ORACLE := debug_proc_vht_c_oracle.c
VHT_SHIM   := host_rtw_debug_proc_vht_shim.c
VHT_RUST   := $(ROOT)/rust/rtw_debug_proc_vht.rs
VHT_L2     := test_proc_vht_24g.c

# W3-209 proc en_fwps (check_fw_ps) get/set
FWPS_ORACLE := debug_proc_fwps_c_oracle.c
FWPS_SHIM   := host_rtw_debug_proc_fwps_shim.c
FWPS_RUST   := $(ROOT)/rust/rtw_debug_proc_fwps.rs
FWPS_L2     := test_proc_en_fwps.c

# W3-226 proc static SMPS get/set
SMPS_ORACLE := debug_proc_smps_c_oracle.c
SMPS_SHIM   := host_rtw_debug_proc_smps_shim.c
SMPS_L2     := test_proc_smps.c

# W3-204 proc rx_ampdu get/set
AMPDU_ORACLE := debug_proc_rx_ampdu_c_oracle.c
AMPDU_SHIM   := host_rtw_debug_proc_ampdu_shim.c
AMPDU_RUST   := $(ROOT)/rust/rtw_debug_proc_ampdu.rs
AMPDU_L2     := test_proc_rx_ampdu.c

# W3-205 proc driver AMPDU tuning get/set
TUNING_ORACLE := debug_proc_ampdu_tuning_c_oracle.c
TUNING_L2     := test_proc_ampdu_tuning.c
# W3-206 proc tx AMPDU misc (tx_quick_addba + tx_aval)
AMPDU_MISC_ORACLE := debug_proc_tx_ampdu_misc_c_oracle.c
AMPDU_MISC_L2     := test_proc_tx_ampdu_misc.c
# W3-207 proc tx AMSDU get/set
AMSDU_ORACLE := debug_proc_tx_amsdu_c_oracle.c
AMSDU_SHIM    := host_rtw_debug_proc_amsdu_shim.c
AMSDU_RUST    := $(ROOT)/rust/rtw_debug_proc_amsdu.rs
AMSDU_L2     := test_proc_tx_amsdu.c
# W3-221 proc 11w TX mgmt inject test get/set (CONFIG_IEEE80211W)
TX_MGMT_ORACLE := debug_proc_tx_mgmt_c_oracle.c
TX_MGMT_CORE   := debug_proc_tx_mgmt_host_core.c
TX_MGMT_SHIM   := host_rtw_debug_proc_tx_mgmt_shim.c
TX_MGMT_RUST   := $(ROOT)/rust/rtw_debug_proc_tx_mgmt.rs
TX_MGMT_L2     := test_proc_tx_mgmt.c

# W3-212 proc TDLS enable get/set (+ function info in PR3)
# W3-213 proc TDLS network + STA display (PR4 L2)
TDLS_ORACLE := debug_proc_tdls_c_oracle.c
TDLS_DISPLAY_ORACLE := debug_proc_tdls_display_oracle.c
TDLS_NETWORK_ORACLE := debug_proc_tdls_network_oracle.c
TDLS_STA_ORACLE := debug_proc_tdls_sta_oracle.c
TDLS_SHIM   := host_rtw_debug_proc_tdls_shim.c
TDLS_RUST   := $(ROOT)/rust/rtw_debug_proc_tdls.rs
TDLS_L2     := test_proc_tdls_enable.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(ROOT)/include

.PHONY: all test test-all test-c test-rust clean \
	test-hooks-c test-hooks-rust test-sec-cam-c test-sec-cam-rust \
	test-regsty-rx-ampdu-c test-regsty-rx-ampdu-rust \
	test-proc-ht-c test-proc-ht-rust test-proc-wow-c test-proc-wow-rust \
	test-proc-vht-c test-proc-vht-rust \
	test-proc-fwps-c test-proc-fwps-rust test-proc-tdls-c test-proc-tdls-rust \
	test-proc-rx-ampdu-c test-proc-rx-ampdu-rust \
	test-proc-ampdu-tuning-c test-proc-ampdu-tuning-rust \
	test-proc-tx-ampdu-misc-c test-proc-tx-ampdu-misc-rust \
	test-proc-tx-amsdu-c test-proc-tx-amsdu-rust \
	test-proc-phy-cap-registry-c test-proc-phy-cap-registry-rust \
	test-proc-btcoex-c \
	test-proc-sreset-c test-proc-sreset-rust \
	test-proc-misc-c test-proc-misc-rust \
	test-proc-mcc-c test-proc-mcc-rust \
	test-proc-txss-c test-proc-txss-rust \
	test-proc-ack-c test-proc-ack-8821c-c \
	test-proc-lps-tp-c test-proc-lps-tp-rust \
	test-proc-monitor-c test-proc-monitor-rust \
	test-proc-ps-info-c test-proc-ps-info-rust \
	test-proc-tx-mgmt-c test-proc-tx-mgmt-rust \
	test-proc-smps-c \
	test-phy-cap-dump-c test-phy-cap-dump-rust

all: test

test: test-hooks-c test-hooks-rust test-sec-cam-c test-sec-cam-rust \
	test-regsty-rx-ampdu-c test-regsty-rx-ampdu-rust \
	test-proc-ht-c test-proc-ht-rust test-proc-wow-c test-proc-wow-rust \
	test-proc-vht-c test-proc-vht-rust \
	test-proc-fwps-c test-proc-fwps-rust test-proc-tdls-c test-proc-tdls-rust \
	test-proc-rx-ampdu-c test-proc-rx-ampdu-rust \
	test-proc-ampdu-tuning-c test-proc-ampdu-tuning-rust \
	test-proc-tx-ampdu-misc-c test-proc-tx-ampdu-misc-rust \
	test-proc-tx-amsdu-c test-proc-tx-amsdu-rust \
	test-proc-phy-cap-registry-c test-proc-phy-cap-registry-rust \
	test-proc-btcoex-c \
	test-proc-sreset-c \
	test-proc-misc-c test-proc-misc-rust \
	test-proc-mcc-c test-proc-mcc-rust \
	test-proc-txss-c test-proc-txss-rust \
	test-proc-ack-c test-proc-ack-8821c-c \
	test-proc-lps-tp-c test-proc-lps-tp-rust \
	test-proc-monitor-c test-proc-monitor-rust \
	test-proc-ps-info-c test-proc-ps-info-rust \
	test-proc-tx-mgmt-c test-proc-tx-mgmt-rust \
	test-proc-smps-c \
	test-phy-cap-dump-c test-phy-cap-dump-rust

test-all: test

test-c: test-hooks-c test-sec-cam-c test-regsty-rx-ampdu-c test-proc-ht-c \
	test-proc-wow-c \
	test-proc-vht-c test-proc-fwps-c test-proc-rx-ampdu-c test-proc-tdls-c \
	test-proc-ampdu-tuning-c test-proc-tx-amsdu-c test-proc-phy-cap-registry-c \
	test-proc-btcoex-c test-proc-sreset-c test-proc-misc-c test-proc-mcc-c test-proc-txss-c test-proc-ack-c test-proc-ack-8821c-c test-proc-lps-tp-c test-proc-monitor-c \
	test-proc-ps-info-c test-proc-tx-mgmt-c test-proc-smps-c \
	test-phy-cap-dump-c

test-rust: test-hooks-rust test-sec-cam-rust test-regsty-rx-ampdu-rust \
	test-proc-ht-rust test-proc-wow-rust test-proc-vht-rust test-proc-fwps-rust test-proc-tdls-rust test-proc-rx-ampdu-rust test-proc-ampdu-tuning-rust \
	test-proc-tx-ampdu-misc-rust test-proc-tx-amsdu-rust test-proc-phy-cap-registry-rust \
	test-proc-misc-rust test-proc-mcc-rust test-proc-lps-tp-rust test-proc-monitor-rust test-proc-ps-info-rust test-proc-tx-mgmt-rust \
	test-proc-txss-rust \
	test-phy-cap-dump-rust

test-hooks-c: test_debug_test_hooks_c
	./test_debug_test_hooks_c

test_debug_test_hooks_c: $(HOOKS_L2) $(HOOKS_ORACLE)
	$(CC) $(CFLAGS) -o $@ $(HOOKS_L2) $(HOOKS_ORACLE)

librust_debug_test_hooks.a: $(RUST_HOOKS)
	rustc -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib --cfg host_rtw_debug_test_hooks_test \
		--cfg config_ap_mode \
		$(RUST_HOOKS) -o $@

test-hooks-rust: test_debug_test_hooks_rust
	./test_debug_test_hooks_rust

test_debug_test_hooks_rust: $(HOOKS_L2) librust_debug_test_hooks.a $(HOOKS_SHIM)
	$(CC) $(CFLAGS) -DTEST_RUST_HOOKS -o $@ $(HOOKS_L2) $(HOOKS_SHIM) librust_debug_test_hooks.a

test-sec-cam-c: test_dump_sec_cam_ent
	./test_dump_sec_cam_ent debug_sec_cam_vectors.json

test_dump_sec_cam_ent: test_dump_sec_cam_ent.c $(SEC_ORACLE) $(SEC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_sec_cam_ent.c $(SEC_ORACLE) $(SEC_SHIM) $(HOST_JSON)

librust_rtw_debug_sec_cam.a: $(RUST_SEC)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_sec_cam_test --cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(RUST_SEC) -o $@

test-sec-cam-rust: test_dump_sec_cam_ent_rust
	./test_dump_sec_cam_ent_rust debug_sec_cam_vectors.json

test_dump_sec_cam_ent_rust: test_dump_sec_cam_ent.c librust_rtw_debug_sec_cam.a $(SEC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_SEC_CAM_ORACLE -o $@ \
		test_dump_sec_cam_ent.c $(SEC_SHIM) $(HOST_JSON) librust_rtw_debug_sec_cam.a

test-regsty-rx-ampdu-c: test_dump_regsty_rx_ampdu
	./test_dump_regsty_rx_ampdu debug_regsty_rx_ampdu_vectors.json

test_dump_regsty_rx_ampdu: $(REGSTY_L2) $(REGSTY_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(REGSTY_L2) $(REGSTY_ORACLE) $(HOST_JSON)

test-phy-cap-dump-c: test_debug_phy_cap_dump
	./test_debug_phy_cap_dump debug_phy_cap_vectors.json

test_debug_phy_cap_dump: $(PHY_CAP_L2) $(PHY_CAP_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(PHY_CAP_CFG) -o $@ $(PHY_CAP_L2) $(PHY_CAP_ORACLE) $(HOST_JSON)

librust_rtw_debug_phy_cap.a: $(PHY_CAP_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_phy_cap_test \
		--cfg config_80211ac_vht --cfg config_beamforming \
		--crate-type staticlib $(PHY_CAP_RUST) -o $@

test-phy-cap-dump-rust: test_debug_phy_cap_dump_rust
	./test_debug_phy_cap_dump_rust debug_phy_cap_vectors.json

test_debug_phy_cap_dump_rust: $(PHY_CAP_L2) librust_rtw_debug_phy_cap.a $(PHY_CAP_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(PHY_CAP_CFG) -DHOST_RTW_DEBUG_PHY_CAP_TEST -o $@ \
		$(PHY_CAP_L2) $(PHY_CAP_SHIM) $(HOST_JSON) librust_rtw_debug_phy_cap.a

librust_rtw_debug_regsty.a: $(REGSTY_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_regsty_test \
		--crate-type staticlib $(REGSTY_RUST) -o $@

test-regsty-rx-ampdu-rust: test_dump_regsty_rx_ampdu_rust
	./test_dump_regsty_rx_ampdu_rust debug_regsty_rx_ampdu_vectors.json

test_dump_regsty_rx_ampdu_rust: $(REGSTY_L2) librust_rtw_debug_regsty.a $(REGSTY_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_REGSTY_ORACLE -o $@ \
		$(REGSTY_L2) $(REGSTY_SHIM) $(HOST_JSON) librust_rtw_debug_regsty.a

test-proc-ht-c: test_proc_ht_registry
	./test_proc_ht_registry debug_proc_ht_vectors.json

test_proc_ht_registry: $(HT_L2) $(HT_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(HT_L2) $(HT_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_ht.a: $(HT_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(HT_RUST) -o $@

test-proc-ht-rust: test_proc_ht_registry_rust
	./test_proc_ht_registry_rust debug_proc_ht_vectors.json

test_proc_ht_registry_rust: $(HT_L2) librust_rtw_debug_proc_ht.a $(HT_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_PROC_HT_ORACLE -o $@ \
		$(HT_L2) $(HT_SHIM) $(HOST_JSON) librust_rtw_debug_proc_ht.a

test-proc-wow-c: test_proc_wow_registry
	./test_proc_wow_registry debug_proc_wow_vectors.json

test_proc_wow_registry: $(WOW_L2) $(WOW_ORACLE) $(WOW_PATTERN) $(HOST_JSON)
	$(CC) $(CFLAGS) $(WOW_CFG) -o $@ $(WOW_L2) $(WOW_ORACLE) $(WOW_PATTERN) $(HOST_JSON)

librust_rtw_debug_proc_wow.a: $(WOW_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_wow_test \
		--cfg config_wowlan --cfg config_gpio_wakeup \
		--crate-type staticlib $(WOW_RUST) -o $@

test-proc-wow-rust: test_proc_wow_registry_rust
	./test_proc_wow_registry_rust debug_proc_wow_vectors.json

test_proc_wow_registry_rust: $(WOW_L2) librust_rtw_debug_proc_wow.a $(WOW_SHIM) $(WOW_PATTERN) $(HOST_JSON)
	$(CC) $(CFLAGS) $(WOW_CFG) -DRUST_RTW_DEBUG_PROC_WOW_ORACLE -o $@ \
		$(WOW_L2) $(WOW_SHIM) $(WOW_PATTERN) $(HOST_JSON) librust_rtw_debug_proc_wow.a

test-proc-vht-c: test_proc_vht_24g
	./test_proc_vht_24g debug_proc_vht_vectors.json

test_proc_vht_24g: $(VHT_L2) $(VHT_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(VHT_L2) $(VHT_ORACLE) $(HOST_JSON)

test-proc-fwps-c: test_proc_en_fwps
	./test_proc_en_fwps debug_proc_fwps_vectors.json

test_proc_en_fwps: $(FWPS_L2) $(FWPS_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(FWPS_L2) $(FWPS_ORACLE) $(HOST_JSON)

test-proc-smps-c: test_proc_smps
	./test_proc_smps debug_proc_smps_vectors.json

test_proc_smps: $(SMPS_L2) $(SMPS_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(SMPS_L2) $(SMPS_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_fwps.a: $(FWPS_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_fwps_test \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(FWPS_RUST) -o $@

test-proc-fwps-rust: test_proc_en_fwps_rust
	./test_proc_en_fwps_rust debug_proc_fwps_vectors.json

test_proc_en_fwps_rust: $(FWPS_L2) librust_rtw_debug_proc_fwps.a $(FWPS_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_PROC_FWPS_ORACLE -o $@ \
		$(FWPS_L2) $(FWPS_SHIM) $(HOST_JSON) librust_rtw_debug_proc_fwps.a

librust_rtw_debug_proc_vht.a: $(VHT_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_vht_test \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--cfg config_80211ac_vht \
		--crate-type staticlib $(VHT_RUST) -o $@

test-proc-vht-rust: test_proc_vht_24g_rust
	./test_proc_vht_24g_rust debug_proc_vht_vectors.json

test_proc_vht_24g_rust: $(VHT_L2) librust_rtw_debug_proc_vht.a $(VHT_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_PROC_VHT_ORACLE -o $@ \
		$(VHT_L2) $(VHT_SHIM) $(HOST_JSON) librust_rtw_debug_proc_vht.a

test-proc-tdls-c: test_proc_tdls_enable
	./test_proc_tdls_enable debug_proc_tdls_vectors.json

test_proc_tdls_enable: $(TDLS_L2) $(TDLS_ORACLE) $(TDLS_DISPLAY_ORACLE) \
		$(TDLS_NETWORK_ORACLE) $(TDLS_STA_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(TDLS_L2) $(TDLS_ORACLE) $(TDLS_DISPLAY_ORACLE) \
		$(TDLS_NETWORK_ORACLE) $(TDLS_STA_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_tdls.a: $(TDLS_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_tdls_test \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--cfg config_tdls \
		--crate-type staticlib $(TDLS_RUST) -o $@

test-proc-tdls-rust: test_proc_tdls_enable_rust
	./test_proc_tdls_enable_rust debug_proc_tdls_vectors.json

test_proc_tdls_enable_rust: $(TDLS_L2) librust_rtw_debug_proc_tdls.a $(TDLS_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_PROC_TDLS_ORACLE -o $@ \
		$(TDLS_L2) $(TDLS_SHIM) $(HOST_JSON) librust_rtw_debug_proc_tdls.a

test-proc-rx-ampdu-c: test_proc_rx_ampdu
	./test_proc_rx_ampdu debug_proc_rx_ampdu_get_vectors.json debug_proc_rx_ampdu_set_vectors.json

test_proc_rx_ampdu: $(AMPDU_L2) $(AMPDU_ORACLE) $(REGSTY_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(AMPDU_L2) $(AMPDU_ORACLE) $(REGSTY_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_ampdu.a: $(AMPDU_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_ampdu_test \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--cfg config_sdio_tx_enable_aval_int \
		--crate-type staticlib $(AMPDU_RUST) -o $@

test-proc-rx-ampdu-rust: test_proc_rx_ampdu_rust
	./test_proc_rx_ampdu_rust debug_proc_rx_ampdu_get_vectors.json debug_proc_rx_ampdu_set_vectors.json

test_proc_rx_ampdu_rust: $(AMPDU_L2) librust_rtw_debug_proc_ampdu.a librust_rtw_debug_regsty.a \
		$(AMPDU_SHIM) $(REGSTY_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_RTW_DEBUG_PROC_AMPDU_TEST -DRUST_RTW_DEBUG_PROC_AMPDU_ORACLE -o $@ \
		$(AMPDU_L2) $(AMPDU_SHIM) $(REGSTY_SHIM) $(HOST_JSON) \
		librust_rtw_debug_proc_ampdu.a librust_rtw_debug_regsty.a

test-proc-ampdu-tuning-c: test_proc_ampdu_tuning
	./test_proc_ampdu_tuning debug_proc_ampdu_tuning_get_vectors.json \
		debug_proc_ampdu_tuning_set_vectors.json

test_proc_ampdu_tuning: $(TUNING_L2) $(TUNING_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(TUNING_L2) $(TUNING_ORACLE) $(HOST_JSON)

test-proc-ampdu-tuning-rust: test_proc_ampdu_tuning_rust
	./test_proc_ampdu_tuning_rust debug_proc_ampdu_tuning_get_vectors.json \
		debug_proc_ampdu_tuning_set_vectors.json

test_proc_ampdu_tuning_rust: $(TUNING_L2) librust_rtw_debug_proc_ampdu.a librust_rtw_debug_regsty.a \
		$(AMPDU_SHIM) $(REGSTY_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_RTW_DEBUG_PROC_AMPDU_TEST -DRUST_RTW_DEBUG_PROC_AMPDU_ORACLE -o $@ \
		$(TUNING_L2) $(AMPDU_SHIM) $(REGSTY_SHIM) $(HOST_JSON) \
		librust_rtw_debug_proc_ampdu.a librust_rtw_debug_regsty.a

test-proc-tx-ampdu-misc-c: test_proc_tx_ampdu_misc
	./test_proc_tx_ampdu_misc debug_proc_tx_ampdu_misc_get_vectors.json \
		debug_proc_tx_ampdu_misc_set_vectors.json

test_proc_tx_ampdu_misc: $(AMPDU_MISC_L2) $(AMPDU_MISC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(AMPDU_MISC_L2) $(AMPDU_MISC_ORACLE) $(HOST_JSON)

test-proc-tx-ampdu-misc-rust: test_proc_tx_ampdu_misc_rust
	./test_proc_tx_ampdu_misc_rust debug_proc_tx_ampdu_misc_get_vectors.json \
		debug_proc_tx_ampdu_misc_set_vectors.json

test_proc_tx_ampdu_misc_rust: $(AMPDU_MISC_L2) librust_rtw_debug_proc_ampdu.a \
		$(AMPDU_SHIM) $(REGSTY_ORACLE) $(REGSTY_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_RTW_DEBUG_PROC_AMPDU_TEST -DRUST_RTW_DEBUG_PROC_AMPDU_ORACLE -o $@ \
		$(AMPDU_MISC_L2) $(AMPDU_SHIM) $(REGSTY_ORACLE) $(REGSTY_SHIM) $(HOST_JSON) \
		librust_rtw_debug_proc_ampdu.a

test-proc-tx-amsdu-c: test_proc_tx_amsdu
	./test_proc_tx_amsdu debug_proc_tx_amsdu_vectors.json

test_proc_tx_amsdu: $(AMSDU_L2) $(AMSDU_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(AMSDU_L2) $(AMSDU_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_amsdu.a: $(AMSDU_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_amsdu_test \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(AMSDU_RUST) -o $@

test-proc-tx-amsdu-rust: test_proc_tx_amsdu_rust
	./test_proc_tx_amsdu_rust debug_proc_tx_amsdu_vectors.json

test_proc_tx_amsdu_rust: $(AMSDU_L2) librust_rtw_debug_proc_amsdu.a $(AMSDU_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_PROC_AMSDU_ORACLE -o $@ \
		$(AMSDU_L2) $(AMSDU_SHIM) $(HOST_JSON) librust_rtw_debug_proc_amsdu.a

test-proc-phy-cap-registry-c: test_proc_phy_cap_registry
	./test_proc_phy_cap_registry debug_proc_phy_cap_registry_vectors.json

test_proc_phy_cap_registry: $(PHY_REG_L2) $(PHY_REG_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(PHY_REG_CFG) -o $@ $(PHY_REG_L2) $(PHY_REG_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_phy_cap.a: $(PHY_REG_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_phy_cap_registry_test \
		--cfg config_rtw_debug --cfg config_proc_debug --cfg config_beamforming \
		--crate-type staticlib $(PHY_REG_RUST) -o $@

test-proc-phy-cap-registry-rust: test_proc_phy_cap_registry_rust
	./test_proc_phy_cap_registry_rust debug_proc_phy_cap_registry_vectors.json

test_proc_phy_cap_registry_rust: $(PHY_REG_L2) librust_rtw_debug_proc_phy_cap.a \
		$(PHY_REG_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(PHY_REG_CFG) -DRUST_RTW_DEBUG_PROC_PHY_CAP_REGISTRY_ORACLE -o $@ \
		$(PHY_REG_L2) $(PHY_REG_SHIM) $(HOST_JSON) librust_rtw_debug_proc_phy_cap.a

test-proc-btcoex-c: test_proc_btcoex
	./test_proc_btcoex debug_proc_btcoex_vectors.json

test_proc_btcoex: $(BTCOEX_L2) $(BTCOEX_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(BTCOEX_CFG) -o $@ $(BTCOEX_L2) $(BTCOEX_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_btcoex.a: $(BTCOEX_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_btcoex_test \
		--cfg config_bt_coexist --cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(BTCOEX_RUST) -o $@

test-proc-btcoex-rust: test_proc_btcoex_rust
	./test_proc_btcoex_rust debug_proc_btcoex_vectors.json

test_proc_btcoex_rust: $(BTCOEX_L2) librust_rtw_debug_proc_btcoex.a \
		$(BTCOEX_SHIM) $(BTCOEX_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(BTCOEX_CFG) -DRUST_RTW_DEBUG_PROC_BTCOEX_ORACLE -o $@ \
		$(BTCOEX_L2) $(BTCOEX_SHIM) $(BTCOEX_ORACLE) $(HOST_JSON) \
		librust_rtw_debug_proc_btcoex.a

test-proc-sreset-c: test_proc_sreset
	./test_proc_sreset debug_proc_sreset_vectors.json

test_proc_sreset: $(SRESET_L2) $(SRESET_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(SRESET_CFG) -o $@ $(SRESET_L2) $(SRESET_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_sreset.a: $(SRESET_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_sreset_test \
		--cfg config_bt_coexist --cfg config_rf4ce_coexist \
		--cfg dbg_config_error_detect \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(SRESET_RUST) -o $@

test-proc-sreset-rust: test_proc_sreset_rust
	./test_proc_sreset_rust debug_proc_sreset_vectors.json

test_proc_sreset_rust: $(SRESET_L2) librust_rtw_debug_proc_sreset.a $(SRESET_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(SRESET_CFG) -DRUST_RTW_DEBUG_PROC_SRESET_ORACLE -o $@ \
		$(SRESET_L2) $(SRESET_SHIM) $(HOST_JSON) librust_rtw_debug_proc_sreset.a

test-proc-misc-c: test_proc_misc
	./test_proc_misc debug_proc_misc_vectors.json

test_proc_misc: $(MISC_L2) $(MISC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(MISC_CFG) -o $@ $(MISC_L2) $(MISC_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_misc.a: $(MISC_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_misc_test \
		--cfg dbg_memory_leak --cfg config_find_best_channel \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(MISC_RUST) -o $@

test-proc-misc-rust: test_proc_misc_rust
	./test_proc_misc_rust debug_proc_misc_vectors.json

test_proc_misc_rust: $(MISC_L2) librust_rtw_debug_proc_misc.a $(MISC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(MISC_CFG) -DRUST_RTW_DEBUG_PROC_MISC_ORACLE -o $@ \
		$(MISC_L2) $(MISC_SHIM) $(HOST_JSON) librust_rtw_debug_proc_misc.a

test-proc-ack-c: test_proc_ack
	./test_proc_ack debug_proc_ack_vectors.json

test-proc-ack-8821c-c: test_proc_ack_8821c
	./test_proc_ack_8821c debug_proc_ack_vectors.json

test_proc_ack: $(ACK_L2) $(ACK_ORACLE) $(ACK_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(ACK_CFG) -I$(HOST_INC) -o $@ \
		$(ACK_L2) $(ACK_ORACLE) $(ACK_HOST) $(HOST_JSON)

test_proc_ack_8821c: $(ACK_L2) $(ACK_ORACLE) $(ACK_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(ACK_CFG_8821C) -I$(HOST_INC) -o $@ \
		$(ACK_L2) $(ACK_ORACLE) $(ACK_HOST) $(HOST_JSON)

test-proc-txss-c: test_proc_txss
	./test_proc_txss debug_proc_txss_vectors.json

test_proc_txss: $(TXSS_L2) $(TXSS_ORACLE) $(TXSS_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(TXSS_CFG) -I$(HOST_INC) -o $@ \
		$(TXSS_L2) $(TXSS_ORACLE) $(TXSS_HOST) $(HOST_JSON)

librust_rtw_debug_proc_txss.a: $(TXSS_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_txss_test \
		--cfg config_ctrl_txss_by_tp --cfg config_dbg_ctrl_txss \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(TXSS_RUST) -o $@

test-proc-txss-rust: test_proc_txss_rust
	./test_proc_txss_rust debug_proc_txss_vectors.json

test_proc_txss_rust: $(TXSS_L2) librust_rtw_debug_proc_txss.a $(TXSS_SHIM) \
		$(TXSS_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(TXSS_CFG) -DRUST_RTW_DEBUG_PROC_TXSS_ORACLE -I$(HOST_INC) -o $@ \
		$(TXSS_L2) $(TXSS_SHIM) $(TXSS_HOST) $(HOST_JSON) \
		librust_rtw_debug_proc_txss.a

test-proc-mcc-c: test_proc_mcc_setters
	./test_proc_mcc_setters debug_proc_mcc_vectors.json

test_proc_mcc_setters: $(MCC_L2) $(MCC_ORACLE) $(MCC_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(MCC_CFG) -I$(HOST_INC) -o $@ \
		$(MCC_L2) $(MCC_ORACLE) $(MCC_HOST) $(HOST_JSON)

librust_rtw_debug_proc_mcc.a: $(MCC_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_mcc_test \
		--cfg config_mcc_mode --cfg config_mcc_phydm_offload \
		--crate-type staticlib $(MCC_RUST) -o $@

test-proc-mcc-rust: test_proc_mcc_setters_rust
	./test_proc_mcc_setters_rust debug_proc_mcc_vectors.json

test_proc_mcc_setters_rust: $(MCC_L2) librust_rtw_debug_proc_mcc.a $(MCC_SHIM) \
		$(MCC_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(MCC_CFG) -DRUST_RTW_DEBUG_PROC_MCC_ORACLE -I$(HOST_INC) -o $@ \
		$(MCC_L2) $(MCC_SHIM) $(MCC_HOST) $(HOST_JSON) librust_rtw_debug_proc_mcc.a

test-proc-lps-tp-c: test_proc_lps_tp
	./test_proc_lps_tp debug_proc_lps_tp_vectors.json

test_proc_lps_tp: $(LPS_TP_L2) $(LPS_TP_ORACLE) $(LPS_TP_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(LPS_TP_CFG) -I$(HOST_INC) -o $@ \
		$(LPS_TP_L2) $(LPS_TP_ORACLE) $(LPS_TP_HOST) $(HOST_JSON)

librust_rtw_debug_proc_lps_tp.a: $(LPS_TP_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_lps_tp_test \
		--cfg config_lps_chk_by_tp \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(LPS_TP_RUST) -o $@

test-proc-lps-tp-rust: test_proc_lps_tp_rust
	./test_proc_lps_tp_rust debug_proc_lps_tp_vectors.json

test_proc_lps_tp_rust: $(LPS_TP_L2) librust_rtw_debug_proc_lps_tp.a $(LPS_TP_SHIM) \
		$(LPS_TP_HOST) $(HOST_JSON)
	$(CC) $(CFLAGS) $(LPS_TP_CFG) -DRUST_RTW_DEBUG_PROC_LPS_TP_ORACLE -I$(HOST_INC) -o $@ \
		$(LPS_TP_L2) $(LPS_TP_SHIM) $(LPS_TP_HOST) $(HOST_JSON) \
		librust_rtw_debug_proc_lps_tp.a

test-proc-monitor-c: test_proc_monitor
	./test_proc_monitor debug_proc_monitor_vectors.json

test_proc_monitor: $(MONITOR_L2) $(MONITOR_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(MONITOR_CFG) -o $@ $(MONITOR_L2) $(MONITOR_ORACLE) \
		$(HOST_JSON)

librust_rtw_debug_proc_monitor.a: $(MONITOR_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_monitor_test \
		--cfg rtw_simple_config --cfg dbg_xmit_block \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--crate-type staticlib $(MONITOR_RUST) -o $@

test-proc-monitor-rust: test_proc_monitor_rust
	./test_proc_monitor_rust debug_proc_monitor_vectors.json

test_proc_monitor_rust: $(MONITOR_L2) librust_rtw_debug_proc_monitor.a \
		$(MONITOR_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(MONITOR_CFG) -DRUST_RTW_DEBUG_PROC_MONITOR_ORACLE -o $@ \
		$(MONITOR_L2) $(MONITOR_SHIM) $(HOST_JSON) \
		librust_rtw_debug_proc_monitor.a

test-proc-ps-info-c: test_proc_ps_info
	./test_proc_ps_info debug_proc_ps_info_vectors.json

test_proc_ps_info: $(PS_INFO_L2) $(PS_INFO_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(PS_INFO_CFG) -o $@ $(PS_INFO_L2) \
		$(PS_INFO_ORACLE) $(HOST_JSON)

librust_rtw_debug_proc_ps_info.a: $(PS_INFO_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg config_rtw_debug --cfg config_proc_debug \
		--cfg config_power_saving --cfg config_wmmps_sta \
		--cfg config_bcn_cnt_confirm_hdl \
		--crate-type staticlib $(PS_INFO_RUST) -o $@

test-proc-ps-info-rust: test_proc_ps_info_rust
	./test_proc_ps_info_rust debug_proc_ps_info_vectors.json

test_proc_ps_info_rust: $(PS_INFO_L2) librust_rtw_debug_proc_ps_info.a \
		$(PS_INFO_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(PS_INFO_CFG) -DPROC_PS_INFO_RUST_TEST -o $@ \
		$(PS_INFO_L2) $(PS_INFO_SHIM) $(HOST_JSON) librust_rtw_debug_proc_ps_info.a

test-proc-tx-mgmt-c: test_proc_tx_mgmt
	./test_proc_tx_mgmt debug_proc_tx_mgmt_vectors.json

test_proc_tx_mgmt: $(TX_MGMT_L2) $(TX_MGMT_ORACLE) $(TX_MGMT_CORE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(TX_MGMT_L2) $(TX_MGMT_ORACLE) \
		$(TX_MGMT_CORE) $(HOST_JSON)

librust_rtw_debug_proc_tx_mgmt.a: $(TX_MGMT_RUST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_rtw_debug_proc_tx_mgmt_test \
		--crate-type staticlib $(TX_MGMT_RUST) -o $@

test-proc-tx-mgmt-rust: test_proc_tx_mgmt_rust
	./test_proc_tx_mgmt_rust debug_proc_tx_mgmt_vectors.json

test_proc_tx_mgmt_rust: $(TX_MGMT_L2) librust_rtw_debug_proc_tx_mgmt.a \
		$(TX_MGMT_SHIM) $(TX_MGMT_CORE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RTW_DEBUG_PROC_TX_MGMT_ORACLE -o $@ \
		$(TX_MGMT_L2) $(TX_MGMT_SHIM) $(TX_MGMT_CORE) $(HOST_JSON) \
		librust_rtw_debug_proc_tx_mgmt.a

clean:
	rm -f test_debug_test_hooks_c test_debug_test_hooks_rust librust_debug_test_hooks.a \
		test_dump_sec_cam_ent test_dump_sec_cam_ent_rust librust_rtw_debug_sec_cam.a \
		test_dump_regsty_rx_ampdu test_dump_regsty_rx_ampdu_rust librust_rtw_debug_regsty.a \
		test_proc_ht_registry test_proc_ht_registry_rust librust_rtw_debug_proc_ht.a \
		test_proc_wow_registry test_proc_wow_registry_rust librust_rtw_debug_proc_wow.a \
		test_proc_vht_24g test_proc_vht_24g_rust librust_rtw_debug_proc_vht.a \
		test_proc_en_fwps test_proc_en_fwps_rust librust_rtw_debug_proc_fwps.a \
		test_proc_tdls_enable test_proc_tdls_enable_rust librust_rtw_debug_proc_tdls.a \
		test_proc_rx_ampdu test_proc_rx_ampdu_rust librust_rtw_debug_proc_ampdu.a \
		test_proc_ampdu_tuning test_proc_ampdu_tuning_rust \
		test_proc_tx_ampdu_misc test_proc_tx_ampdu_misc_rust \
		test_proc_tx_amsdu test_proc_tx_amsdu_rust librust_rtw_debug_proc_amsdu.a \
		test_proc_phy_cap_registry test_proc_phy_cap_registry_rust \
		librust_rtw_debug_proc_phy_cap.a \
		test_proc_btcoex test_proc_btcoex_rust librust_rtw_debug_proc_btcoex.a \
		test_proc_sreset \
		test_proc_misc test_proc_misc_rust librust_rtw_debug_proc_misc.a \
		test_proc_mcc_setters test_proc_mcc_setters_rust \
		librust_rtw_debug_proc_mcc.a \
		test_proc_ack test_proc_ack_8821c \
		test_proc_txss test_proc_txss_rust librust_rtw_debug_proc_txss.a \
		test_proc_lps_tp test_proc_lps_tp_rust librust_rtw_debug_proc_lps_tp.a \
		test_proc_monitor test_proc_monitor_rust \
		librust_rtw_debug_proc_monitor.a \
		test_proc_ps_info test_proc_ps_info_rust librust_rtw_debug_proc_ps_info.a \
		test_proc_tx_mgmt test_proc_tx_mgmt_rust \
		librust_rtw_debug_proc_tx_mgmt.a \
		test_debug_phy_cap_dump test_debug_phy_cap_dump_rust librust_rtw_debug_phy_cap.a
