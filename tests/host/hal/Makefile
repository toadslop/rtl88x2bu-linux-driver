# Host L2 differential tests for hal_com Rust slices (W4-01 hw rate, W4-02 rsvd page).

ROOT     := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE   := hal_hw_rate_c_oracle.c
RSVD_ORACLE := hal_rsvd_page_c_oracle.c
RSVD_SHIM := hal_rsvd_page_shim.c
CHIP_ORACLE := hal_dump_chip_info_c_oracle.c
TRX_ORACLE := hal_dump_trx_mode_c_oracle.c
FW_ORACLE := hal_dump_fw_info_c_oracle.c
TX_ACLT_FORCE_ORACLE := hal_dump_tx_aclt_force_val_c_oracle.c
TX_ACLT_CONFS_ORACLE := hal_dump_tx_aclt_confs_c_oracle.c
HAL_SPEC_ORACLE := hal_dump_hal_spec_c_oracle.c
MRATE_ORACLE := hal_mrate_to_hw_rate_c_oracle.c
RF_NSS_ORACLE := hal_dump_rf_nss_c_oracle.c
HALMAC_ORACLE := hal_halmac_enum_mappers_c_oracle.c
PHY_BIT_SHIFT_ORACLE := hal_phy_bit_shift_c_oracle.c
PHY_RF_MAX_REG_ORACLE := hal_phy_rf_max_reg_c_oracle.c
PHY_RF_SHADOW_ORACLE := hal_phy_rf_shadow_c_oracle.c
PHY_RF_SHADOW_HW_SHIM := hal_phy_rf_shadow_hw_shim.c
HALMAC_TRX_ORACLE := hal_halmac_trx_share_c_oracle.c
BRATE_ORACLE := hal_set_brate_cfg_c_oracle.c
DM_BOARD_ORACLE := hal_dm_board_type_c_oracle.c
DM_ODM_ENDIAN_ORACLE := hal_dm_odm_endian_hooks_c_oracle.c
DM_DUMP_STA_SHIM := hal_dm_dump_sta_info_shim.c
DM_DUMP_STA_TRAFFIC_SHIM := hal_dm_dump_sta_traffic_shim.c
DM_ODM_IFACE_ORACLE := hal_dm_odm_iface_c_oracle.c
DM_ODM_IFACE_SHIM := hal_dm_odm_iface_host_shim.c
DM_PHYDM_OPS_SHIM := hal_dm_phydm_ops_shim.c
DM_PHYDM_OPS_ORACLE := hal_dm_phydm_ops_c_oracle.c
DM_PHYDM_IGI_IQK_ORACLE := hal_dm_phydm_igi_iqk_c_oracle.c
DM_PHYDM_IGI_IQK_SHIM := hal_dm_phydm_igi_iqk_host_shim.c
DM_SET_ODM_VAR_CORE_ORACLE := hal_dm_set_odm_var_core_c_oracle.c
DM_SET_ODM_VAR_CORE_SHIM := hal_dm_set_odm_var_core_host_shim.c
DM_PHYDM_IQK_LCK_ORACLE := hal_dm_phydm_iqk_lck_c_oracle.c
DM_PHYDM_IQK_LCK_SHIM := hal_dm_phydm_iqk_lck_host_shim.c
DM_PHYDM_CAL_TRIGGER_ORACLE := hal_dm_phydm_cal_trigger_c_oracle.c
DM_PHYDM_CAL_TRIGGER_SHIM := hal_dm_phydm_cal_trigger_host_shim.c
DM_DBG_RF_CAL_TEST_ORACLE := hal_dm_dbg_rf_cal_test_c_oracle.c
DM_DBG_RF_CAL_TEST_SHIM := hal_dm_dbg_rf_cal_test_host_shim.c
DM_PHYDM_IQK_LCK_CFLAGS := -DRTL8822B_SUPPORT=1
DM_SET_ODM_VAR_RX_ORACLE := hal_dm_set_odm_var_rx_c_oracle.c
DM_SET_ODM_VAR_RX_SHIM := hal_dm_set_odm_var_rx_host_shim.c
DM_DYNTPWR_ORACLE := hal_dm_dyntxpwr_c_oracle.c
DM_DYNTPWR_SHIM := hal_dm_dyntxpwr_host_shim.c
DM_MIN_RSSI_PHY_CNT_ORACLE := hal_dm_min_rssi_phy_cnt_c_oracle.c
DM_MIN_RSSI_PHY_CNT_SHIM := hal_dm_min_rssi_phy_cnt_host_shim.c
DM_GET_ODM_VAR_ORACLE := hal_dm_get_odm_var_c_oracle.c
DM_GET_ODM_VAR_SHIM := hal_dm_get_odm_var_host_shim.c
DM_GET_ODM_VAR_CFLAGS := -DCONFIG_ANTENNA_DIVERSITY
DM_PHYDM_RRSR_ORACLE := hal_dm_phydm_rrsr_c_oracle.c
DM_PHYDM_RRSR_SHIM := hal_dm_phydm_rrsr_host_shim.c
DM_PHYDM_CRYSTAL_CAP_ORACLE := hal_dm_phydm_crystal_cap_c_oracle.c
DM_PHYDM_CRYSTAL_CAP_SHIM := hal_dm_phydm_crystal_cap_host_shim.c
DM_RUNTIME_TRX_ORACLE := hal_dm_runtime_trx_path_c_oracle.c
DM_RUNTIME_TRX_SHIM := hal_dm_runtime_trx_path_host_shim.c
RUNTIME_TRX_CFLAGS := $(CFLAGS) -DCONFIG_RTL8822B -DCONFIG_RTW_PATH_DIV \
	-DCONFIG_RTW_TX_NPATH_EN
DM_PHYDM_IQK_TRIGGER_DBG_ORACLE := hal_dm_phydm_iqk_trigger_dbg_c_oracle.c
DM_PHYDM_IQK_TRIGGER_DBG_SHIM := hal_dm_phydm_iqk_trigger_dbg_host_shim.c
DM_PHYDM_RA_REGISTED_ORACLE := hal_dm_phydm_ra_registed_c_oracle.c
DM_PHYDM_RA_REGISTED_SHIM := hal_dm_phydm_ra_registed_host_shim.c
DM_PHYDM_READ_EFUSE_TRIM_ORACLE := hal_dm_phydm_read_efuse_trim_c_oracle.c
DM_PHYDM_READ_EFUSE_TRIM_SHIM := hal_dm_phydm_read_efuse_trim_host_shim.c
DM_PHYDM_TRX_CFG_ORACLE := hal_dm_phydm_trx_cfg_c_oracle.c
DM_PHYDM_TRX_CFG_SHIM := hal_dm_phydm_trx_cfg_host_shim.c
DM_PHYDM_TRX_CFG_CFLAGS := -DCONFIG_CTRL_TXSS_BY_TP
DM_CFG_PHY_PARA_ORACLE := hal_dm_cfg_phy_para_c_oracle.c
DM_CFG_PHY_PARA_SHIM := hal_dm_cfg_phy_para_host_shim.c
DM_HALMAC_DM_LEAF_ORACLE := hal_dm_halmac_dm_leaf_c_oracle.c
DM_HALMAC_DM_LEAF_SHIM := hal_dm_halmac_dm_leaf_host_shim.c
DM_DYN_SOML_ORACLE := hal_dm_dyn_soml_c_oracle.c
DM_DYN_SOML_SHIM := hal_dm_dyn_soml_host_shim.c
DM_DYN_SOML_RUST_SHIM := hal_dm_dyn_soml_rust_host_shim.c
USB_WVALUE_ORACLE := hal_usb_vendor_wvalue_c_oracle.c
USB_SYNC_READ_ORACLE := hal_usb_sync_read_c_oracle.c
USB_SYNC_WRITE_ORACLE := hal_usb_sync_write_c_oracle.c
CHIP_ODM_ORACLE := hal_intf_chip_odm_c_oracle.c
CHIP_RFTYPE_ORACLE := hal_intf_chip_rftype_c_oracle.c
RFPATH_ORACLE := hal_intf_rfpath_c_oracle.c
TRXNSS_ORACLE := hal_intf_trxnss_c_oracle.c
RF_REST_SRC := $(ROOT)/core/rtw_rf_rest.c
RUST_RF_REST := ../rf/librust_rf_rest.a
RF_REST_SHIMS := ../rf/host_rf_regd_exc_shim.c ../rf/host_rf_txpwr_lmt_shim.c
USB_ASYNC_WRITE_ORACLE := hal_usb_async_write_c_oracle.c
RUST_HAL := $(ROOT)/rust/hal_com.rs
RUST_HAL_USB := $(ROOT)/rust/hal_usb.rs
RUST_HAL_DM := $(ROOT)/rust/hal_dm.rs
RUST_HAL_PHY := $(ROOT)/rust/hal_phy.rs
RUST_HAL_INTF := $(ROOT)/rust/hal_intf.rs
RUST_HAL_HALMAC := $(ROOT)/rust/hal_halmac.rs
SEC_CAM_ENT_L2 := hal_com_sec_cam_ent_l2.c
RUST_SEC_CAM_ENT := $(ROOT)/rust/hal_com_sec_cam_ent_host.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC)
RFPATH_CFLAGS := $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/core \
	-include $(HOST_INC)/host_autoconf.h -DHOST_RF_TEST \
	-Wno-format-truncation -Wno-unused-const-variable

.PHONY: all test test-c test-rust test-rsvd test-rsvd-rust test-chip test-chip-rust \
	test-trx test-trx-rust test-fw test-fw-rust test-tx-aclt-force test-tx-aclt-force-rust \
	test-tx-aclt-confs test-tx-aclt-confs-rust test-hal-spec test-hal-spec-rust \
	test-mrate test-mrate-rust \
	test-rf-nss test-rf-nss-rust test-halmac test-halmac-rust test-halmac-rsvd \
	test-halmac-rsvd-rust test-halmac-drv2halmac test-halmac-drv2halmac-rust \
	test-brate test-brate-rust \
	test-legal-channel test-legal-channel-rust \
	test-config-chplan-hw test-config-chplan-hw-rust \
	test-config-chplan-full test-config-chplan-full-rust \
	test-mapping-out-pipe test-mapping-out-pipe-rust test-mgntframe-raid test-mgntframe-raid-rust \
	test-update-sta-wset-mimo \
	test-update-sta-smps-rate test-update-sta-smps-rate-rust \
	test-update-sta-smps-rate-rust-kern \
	test-update-sta-ra-info test-update-sta-ra-info-rust \
	test-hdata-rate test-hdata-rate-rust test-phy-bit-shift test-phy-bit-shift-rust \
	test-dm-board-type test-dm-board-type-rust \
	test-dm-odm-endian-hooks test-dm-odm-endian-hooks-rust \
	test-dm-phydm-ops test-dm-phydm-ops-rust \
	test-dm-dyntxpwr test-dm-dyntxpwr-rust test-dm-dyn-soml test-dm-dyn-soml-rust \
	test-dm-dump-sta-traffic test-dm-dump-sta-traffic-rust \
	test-dm-phydm-igi-iqk test-dm-phydm-igi-iqk-rust \
	test-dm-phydm-init test-dm-phydm-init-rust \
	test-dm-set-odm-var-core test-dm-set-odm-var-core-rust \
	test-dm-set-odm-var-rx \
	test-dm-min-rssi-phy-cnt test-dm-min-rssi-phy-cnt-rust \
	test-dm-get-odm-var test-dm-get-odm-var-rust \
	test-usb-vendor-wvalue test-usb-vendor-wvalue-rust \
	test-usb-sync-read test-usb-sync-read-rust test-usb-sync-write \
	test-usb-async-write test-usb-async-write-rust test-chip-odm test-chip-odm-rust \
	test-sec-read-cam-ent test-sec-read-cam-ent-rust test-customer-str test-customer-str-rust \
	test-req-per-rpt test-req-per-rpt-rust \
	test-dft-port $(if $(wildcard $(ROOT)/rust/hal_com_dft_port_host.rs),test-dft-port-rust,) \
	test-fw-rsvdpage-h2c test-fw-rsvdpage-h2c-rust test-mbid-cam test-mbid-cam-rust \
	test-mbid-cam-alloc test-mbid-cam-alloc-rust \
	test-mbid-cam-hw test-mbid-cam-hw-rust test-mbid-cam-macaddr \
	test-mbid-cam-macaddr-rust \
	test-get-rsvd-page test-get-rsvd-page-rust \
	test-dump-rsvd-fifo test-dump-rsvd-fifo-rust \
	test-tx-aclt-set test-tx-aclt-set-rust \
	test-rcr-leaf test-rcr-leaf-rust test-rcr-chk-bssid \
	test-rcr-am-hwvar test-rcr-am-hwvar-rust \
	test-gpio-wakeup test-gpio-wakeup-rust test-gpio-wakeup-8821asdio \
	test-gpio-wakeup-rust-8821asdio test-tsf-iface test-tsf-iface-mbssid \
	test-tsf-iface-rust test-tsf-iface-rust-mbssid \
	test-tsf-periodic test-tsf-periodic-rust \
	test-halmac-identity test-halmac-identity-rust \
	test-c2h-evt-robustness clean

all: test

test: test-c test-rust test-rsvd test-rsvd-rust test-chip test-chip-rust test-trx test-trx-rust \
	test-fw test-fw-rust test-tx-aclt-force test-tx-aclt-force-rust \
	test-tx-aclt-confs test-tx-aclt-confs-rust test-hal-spec test-hal-spec-rust \
	test-mrate test-mrate-rust test-rf-nss test-rf-nss-rust \
	test-halmac test-halmac-rust test-halmac-rsvd test-halmac-rsvd-rust test-brate test-brate-rust \
	test-legal-channel test-legal-channel-rust \
	test-config-chplan-hw test-config-chplan-hw-rust \
	test-config-chplan-full test-config-chplan-full-rust \
	test-mapping-out-pipe test-mapping-out-pipe-rust \
	test-mgntframe-raid test-mgntframe-raid-rust 	test-update-sta-wset-mimo test-update-sta-wset-mimo-rust \
	test-update-sta-ra-info test-update-sta-ra-info-rust test-hdata-rate test-hdata-rate-rust \
	test-phy-bit-shift test-phy-bit-shift-rust test-dm-board-type test-dm-board-type-rust \
	test-dm-odm-endian-hooks test-dm-odm-endian-hooks-rust \
	test-dm-init-odm-cominfo-tail test-dm-init-odm-cominfo-tail-rust \
	test-dm-phydm-ops test-dm-phydm-ops-rust \
	test-dm-dyntxpwr test-dm-dyntxpwr-rust test-dm-dyn-soml test-dm-dyn-soml-rust \
	test-dm-dump-sta-traffic test-dm-dump-sta-traffic-rust \
	test-dm-phydm-igi-iqk test-dm-phydm-igi-iqk-rust \
	test-dm-phydm-init test-dm-phydm-init-rust \
	test-dm-set-odm-var-core test-dm-set-odm-var-core-rust \
	test-dm-set-odm-var-rx \
	test-dm-min-rssi-phy-cnt test-dm-min-rssi-phy-cnt-rust \
	test-dm-get-odm-var test-dm-get-odm-var-rust \
	test-usb-vendor-wvalue test-usb-vendor-wvalue-rust test-usb-sync-read \
	test-usb-sync-read-rust test-usb-sync-write test-usb-async-write \
	test-usb-async-write-rust test-chip-odm test-chip-odm-rust \
	test-hal-trxnss test-hal-trxnss-rust \
	test-sec-read-cam-ent test-sec-read-cam-ent-rust test-customer-str test-customer-str-rust \
	test-req-per-rpt-rust test-dft-port $(if $(wildcard $(ROOT)/rust/hal_com_dft_port_host.rs),test-dft-port-rust,) \
	test-fw-rsvdpage-h2c test-fw-rsvdpage-h2c-rust test-mbid-cam test-mbid-cam-rust \
	test-mbid-cam-alloc test-mbid-cam-alloc-rust \
	test-mbid-cam-hw test-mbid-cam-hw-rust test-mbid-cam-macaddr \
	test-mbid-cam-macaddr-rust \
	test-get-rsvd-page test-get-rsvd-page-rust \
	test-dump-rsvd-fifo test-dump-rsvd-fifo-rust \
	test-tx-aclt-set test-tx-aclt-set-rust \
	test-rcr-leaf test-rcr-leaf-rust test-rcr-chk-bssid \
	test-rcr-am-hwvar test-rcr-am-hwvar-rust \
	test-gpio-wakeup test-gpio-wakeup-rust test-gpio-wakeup-8821asdio \
	test-gpio-wakeup-rust-8821asdio test-tsf-iface test-tsf-iface-mbssid \
	test-tsf-iface-rust test-tsf-iface-rust-mbssid \
	test-tsf-periodic test-tsf-periodic-rust \
	test-halmac-identity test-halmac-identity-rust \
	test-c2h-evt-robustness

test-c: test_hw_rate_map
	./test_hw_rate_map hal_hw_rate_vectors.json

test_hw_rate_map: test_hw_rate_map.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hw_rate_map.c $(ORACLE) $(HOST_JSON)

librust_hal_com.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_hw_rate_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-rust: test_hw_rate_map_rust test-chip-odm-rust test-chip-rftype-rust test-hal-rfpath-rust \
	test-hal-trxnss-rust
	./test_hw_rate_map_rust hal_hw_rate_vectors.json

test_hw_rate_map_rust: test_hw_rate_map.c librust_hal_com.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_ORACLE -o $@ test_hw_rate_map.c $(HOST_JSON) librust_hal_com.a

test-rsvd: test_rsvd_page_cache
	./test_rsvd_page_cache

test_rsvd_page_cache: test_rsvd_page_cache.c $(RSVD_ORACLE) $(RSVD_SHIM)
	$(CC) $(CFLAGS) -o $@ test_rsvd_page_cache.c $(RSVD_ORACLE) $(RSVD_SHIM)

librust_hal_com_rsvd.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_rsvd_page_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-rsvd-rust: test_rsvd_page_cache_rust
	./test_rsvd_page_cache_rust

test_rsvd_page_cache_rust: test_rsvd_page_cache.c librust_hal_com_rsvd.a $(RSVD_SHIM)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_RSVD_ORACLE -o $@ test_rsvd_page_cache.c $(RSVD_SHIM) librust_hal_com_rsvd.a

GET_RSVD_PAGE_SHIM := hal_com_get_rsvd_page_shim.c
GET_RSVD_PAGE_ORACLE := hal_com_get_rsvd_page_c_oracle.c
GET_RSVD_PAGE_DUMP_LINK := hal_com_get_rsvd_page_dump_link.c

TX_ACLT_SET_SHIM := hal_com_tx_aclt_set_shim.c
TX_ACLT_SET_ORACLE := hal_com_tx_aclt_set_c_oracle.c

test-get-rsvd-page: test_hal_com_get_rsvd_page
	./test_hal_com_get_rsvd_page

test_hal_com_get_rsvd_page: test_hal_com_get_rsvd_page.c \
		$(GET_RSVD_PAGE_SHIM) $(GET_RSVD_PAGE_ORACLE)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_get_rsvd_page.c \
		$(GET_RSVD_PAGE_SHIM) $(GET_RSVD_PAGE_ORACLE)

librust_hal_com_get_rsvd_page.a: ../../../rust/hal_com_rsvd_page.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_get_rsvd_page_test --cfg config_rtw_halmac \
		--crate-type staticlib ../../../rust/hal_com_rsvd_page.rs -o $@

test-get-rsvd-page-rust: test_hal_com_get_rsvd_page_rust
	./test_hal_com_get_rsvd_page_rust

test_hal_com_get_rsvd_page_rust: test_hal_com_get_rsvd_page.c \
		librust_hal_com_get_rsvd_page.a $(GET_RSVD_PAGE_SHIM) $(GET_RSVD_PAGE_ORACLE)
	$(CC) $(CFLAGS) -I../include -DRUST_HAL_COM_GET_RSVD_PAGE_ORACLE -o $@ \
		test_hal_com_get_rsvd_page.c $(GET_RSVD_PAGE_SHIM) $(GET_RSVD_PAGE_ORACLE) \
		librust_hal_com_get_rsvd_page.a

DUMP_RSVD_FIFO_CFLAGS := -I../include -DCONFIG_RTW_DEBUG -DCONFIG_PROC_DEBUG
DUMP_RSVD_FIFO_ORACLE := hal_com_dump_rsvd_fifo_c_oracle.c

test-dump-rsvd-fifo: test_hal_com_dump_rsvd_fifo
	./test_hal_com_dump_rsvd_fifo hal_com_dump_rsvd_fifo_vectors.json

test_hal_com_dump_rsvd_fifo: test_hal_com_dump_rsvd_fifo.c \
		$(DUMP_RSVD_FIFO_ORACLE) $(GET_RSVD_PAGE_SHIM) \
		$(GET_RSVD_PAGE_ORACLE) $(GET_RSVD_PAGE_DUMP_LINK) $(HOST_JSON)
	$(CC) $(CFLAGS) $(DUMP_RSVD_FIFO_CFLAGS) -o $@ \
		test_hal_com_dump_rsvd_fifo.c $(DUMP_RSVD_FIFO_ORACLE) \
		$(GET_RSVD_PAGE_SHIM) $(GET_RSVD_PAGE_ORACLE) \
		$(GET_RSVD_PAGE_DUMP_LINK) $(HOST_JSON)

DUMP_RSVD_PAGE_RUST_SRC := $(ROOT)/rust/hal_com_dump_rsvd_page.rs

librust_hal_com_dump_rsvd_page.a: $(DUMP_RSVD_PAGE_RUST_SRC)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_dump_rsvd_fifo_test \
		--cfg config_rtw_debug \
		--cfg config_support_fifo_dump \
		--crate-type staticlib $(DUMP_RSVD_PAGE_RUST_SRC) -o $@

test-dump-rsvd-fifo-rust: test_hal_com_dump_rsvd_fifo_rust
	./test_hal_com_dump_rsvd_fifo_rust hal_com_dump_rsvd_fifo_vectors.json

test_hal_com_dump_rsvd_fifo_rust: test_hal_com_dump_rsvd_fifo.c \
		$(DUMP_RSVD_FIFO_ORACLE) $(GET_RSVD_PAGE_SHIM) $(GET_RSVD_PAGE_ORACLE) \
		$(GET_RSVD_PAGE_DUMP_LINK) $(HOST_JSON) librust_hal_com_dump_rsvd_page.a
	$(CC) $(CFLAGS) $(DUMP_RSVD_FIFO_CFLAGS) -DRUST_HAL_COM_DUMP_RSVD_FIFO_ORACLE -o $@ \
		test_hal_com_dump_rsvd_fifo.c $(DUMP_RSVD_FIFO_ORACLE) \
		$(GET_RSVD_PAGE_SHIM) $(GET_RSVD_PAGE_ORACLE) \
		$(GET_RSVD_PAGE_DUMP_LINK) $(HOST_JSON) librust_hal_com_dump_rsvd_page.a

test-tx-aclt-set: test_hal_com_tx_aclt_set
	./test_hal_com_tx_aclt_set

test_hal_com_tx_aclt_set: test_hal_com_tx_aclt_set.c \
		$(TX_ACLT_SET_SHIM) $(TX_ACLT_SET_ORACLE)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_tx_aclt_set.c \
		$(TX_ACLT_SET_SHIM) $(TX_ACLT_SET_ORACLE)

librust_hal_com_tx_aclt_set.a: ../../../rust/hal_com_tx_aclt_set.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_tx_aclt_set_test --cfg config_tx_ac_lifetime \
		--crate-type staticlib ../../../rust/hal_com_tx_aclt_set.rs -o $@

test-tx-aclt-set-rust: test_hal_com_tx_aclt_set_rust
	./test_hal_com_tx_aclt_set_rust

test_hal_com_tx_aclt_set_rust: test_hal_com_tx_aclt_set.c librust_hal_com_tx_aclt_set.a \
		$(TX_ACLT_SET_SHIM) $(TX_ACLT_SET_ORACLE)
	$(CC) $(CFLAGS) -I../include -DRUST_HAL_COM_TX_ACLT_SET_ORACLE -o $@ \
		test_hal_com_tx_aclt_set.c $(TX_ACLT_SET_SHIM) $(TX_ACLT_SET_ORACLE) \
		librust_hal_com_tx_aclt_set.a

test-chip: test_dump_chip_info
	./test_dump_chip_info hal_dump_chip_info_vectors.json

test_dump_chip_info: test_dump_chip_info.c $(CHIP_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/include -o $@ test_dump_chip_info.c $(CHIP_ORACLE) $(HOST_JSON)

librust_hal_com_chip.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_chip_info_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-chip-rust: test_dump_chip_info_rust
	./test_dump_chip_info_rust hal_dump_chip_info_vectors.json

test_dump_chip_info_rust: test_dump_chip_info.c librust_hal_com_chip.a $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/include -DRUST_HAL_COM_CHIP_ORACLE -o $@ \
		test_dump_chip_info.c $(HOST_JSON) librust_hal_com_chip.a

test-trx: test_dump_trx_mode
	./test_dump_trx_mode hal_dump_trx_mode_vectors.json

test_dump_trx_mode: test_dump_trx_mode.c $(TRX_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_trx_mode.c $(TRX_ORACLE) $(HOST_JSON)

librust_hal_intf_trx.a: $(RUST_HAL_INTF)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_intf_trx_test \
		--crate-type staticlib $(RUST_HAL_INTF) -o $@

test-trx-rust: test_dump_trx_mode_rust
	./test_dump_trx_mode_rust hal_dump_trx_mode_vectors.json

test_dump_trx_mode_rust: test_dump_trx_mode.c librust_hal_intf_trx.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_INTF_TRX_ORACLE -o $@ test_dump_trx_mode.c \
		$(HOST_JSON) librust_hal_intf_trx.a

test-fw: test_dump_fw_info
	./test_dump_fw_info hal_dump_fw_info_vectors.json

test_dump_fw_info: test_dump_fw_info.c $(FW_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_fw_info.c $(FW_ORACLE) $(HOST_JSON)

librust_hal_com_fw.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_fw_info_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-fw-rust: test_dump_fw_info_rust
	./test_dump_fw_info_rust hal_dump_fw_info_vectors.json

test_dump_fw_info_rust: test_dump_fw_info.c librust_hal_com_fw.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_FW_ORACLE -o $@ test_dump_fw_info.c \
		$(HOST_JSON) librust_hal_com_fw.a

test-tx-aclt-force: test_dump_tx_aclt_force_val
	./test_dump_tx_aclt_force_val hal_dump_tx_aclt_force_val_vectors.json

test_dump_tx_aclt_force_val: test_dump_tx_aclt_force_val.c $(TX_ACLT_FORCE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_tx_aclt_force_val.c $(TX_ACLT_FORCE_ORACLE) $(HOST_JSON)

librust_hal_com_tx_aclt_force.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_tx_aclt_force_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-tx-aclt-force-rust: test_dump_tx_aclt_force_val_rust
	./test_dump_tx_aclt_force_val_rust hal_dump_tx_aclt_force_val_vectors.json

test_dump_tx_aclt_force_val_rust: test_dump_tx_aclt_force_val.c librust_hal_com_tx_aclt_force.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_TX_ACLT_FORCE_ORACLE -o $@ test_dump_tx_aclt_force_val.c \
		$(HOST_JSON) librust_hal_com_tx_aclt_force.a

test-tx-aclt-confs: test_dump_tx_aclt_confs
	./test_dump_tx_aclt_confs hal_dump_tx_aclt_confs_vectors.json

test_dump_tx_aclt_confs: test_dump_tx_aclt_confs.c $(TX_ACLT_CONFS_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_tx_aclt_confs.c $(TX_ACLT_CONFS_ORACLE) $(HOST_JSON)

librust_hal_com_tx_aclt_confs.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_tx_aclt_confs_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-tx-aclt-confs-rust: test_dump_tx_aclt_confs_rust
	./test_dump_tx_aclt_confs_rust hal_dump_tx_aclt_confs_vectors.json

test_dump_tx_aclt_confs_rust: test_dump_tx_aclt_confs.c librust_hal_com_tx_aclt_confs.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_TX_ACLT_CONFS_ORACLE -o $@ test_dump_tx_aclt_confs.c \
		$(HOST_JSON) librust_hal_com_tx_aclt_confs.a

test-hal-spec: test_dump_hal_spec
	./test_dump_hal_spec hal_dump_hal_spec_vectors.json

test_dump_hal_spec: test_dump_hal_spec.c $(HAL_SPEC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_hal_spec.c $(HAL_SPEC_ORACLE) $(HOST_JSON)

librust_hal_com_hal_spec.a: $(RUST_HAL)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_hal_spec_test --cfg config_tx_ac_lifetime \
		--crate-type staticlib $(RUST_HAL) -o $@

test-hal-spec-rust: test_dump_hal_spec_rust
	./test_dump_hal_spec_rust hal_dump_hal_spec_vectors.json

test_dump_hal_spec_rust: test_dump_hal_spec.c librust_hal_com_hal_spec.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_HAL_SPEC_ORACLE -o $@ test_dump_hal_spec.c \
		$(HOST_JSON) librust_hal_com_hal_spec.a

test-mrate: test_m_rate_to_hw_rate
	./test_m_rate_to_hw_rate hal_mrate_to_hw_rate_vectors.json

test_m_rate_to_hw_rate: test_m_rate_to_hw_rate.c $(MRATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_m_rate_to_hw_rate.c $(MRATE_ORACLE) $(HOST_JSON)

librust_hal_com_mrate.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_mrate_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-mrate-rust: test_m_rate_to_hw_rate_rust
	./test_m_rate_to_hw_rate_rust hal_mrate_to_hw_rate_vectors.json

test_m_rate_to_hw_rate_rust: test_m_rate_to_hw_rate.c librust_hal_com_mrate.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_MRATE_ORACLE -o $@ test_m_rate_to_hw_rate.c \
		$(HOST_JSON) librust_hal_com_mrate.a

test-rf-nss: test_dump_rf_nss
	./test_dump_rf_nss hal_dump_rf_path_vectors.json hal_dump_trx_nss_vectors.json

test_dump_rf_nss: test_dump_rf_nss.c $(RF_NSS_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_dump_rf_nss.c $(RF_NSS_ORACLE) $(HOST_JSON)

librust_hal_intf_rf_nss.a: $(RUST_HAL_INTF)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_intf_rf_nss_test \
		--crate-type staticlib $(RUST_HAL_INTF) -o $@

test-rf-nss-rust: test_dump_rf_nss_rust
	./test_dump_rf_nss_rust hal_dump_rf_path_vectors.json hal_dump_trx_nss_vectors.json

test_dump_rf_nss_rust: test_dump_rf_nss.c librust_hal_intf_rf_nss.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_INTF_RF_NSS_ORACLE -o $@ test_dump_rf_nss.c \
		$(HOST_JSON) librust_hal_intf_rf_nss.a

HALMAC_VERSION_ORACLE := hal_halmac_version_c_oracle.c

test-halmac: test_hal_halmac_network test_hal_halmac_bcn test_hal_halmac_bcn_abi \
	test_hal_halmac_rsvd test_hal_halmac_trx_share test_hal_halmac_drv2halmac \
	test_hal_halmac_rf_type 	test_hal_halmac_usb_fifo test_hal_halmac_fw_led_phy \
	test_hal_halmac_phy_param_drv2halmac test_hal_halmac_version
	./test_hal_halmac_network hal_halmac_network_vectors.json
	./test_hal_halmac_bcn hal_halmac_bcn_vectors.json
	./test_hal_halmac_bcn_abi
	./test_hal_halmac_rsvd hal_halmac_rsvd_drv2halmac_vectors.json \
		hal_halmac_rsvd_halmac2drv_vectors.json
	./test_hal_halmac_trx_share hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json
	./test_hal_halmac_drv2halmac hal_halmac_hw_port_drv2halmac_vectors.json \
		hal_halmac_network_drv2halmac_vectors.json
	./test_hal_halmac_rf_type hal_halmac_rf_type_drv2halmac_vectors.json \
		hal_halmac_rf_type_halmac2drv_vectors.json
	./test_hal_halmac_usb_fifo hal_halmac_fifo_sel_drv2halmac_vectors.json \
		hal_halmac_usb_mode_drv2halmac_vectors.json
	./test_hal_halmac_fw_led_phy hal_halmac_fw_mem_drv2halmac_vectors.json \
		hal_halmac_led_mode_drv2halmac_vectors.json \
		hal_halmac_phy_param_val_drv2halmac_vectors.json
	./test_hal_halmac_phy_param_drv2halmac hal_halmac_phy_param_drv2halmac_vectors.json
	./test_hal_halmac_version hal_halmac_get_version_vectors.json

test_hal_halmac_version: test_hal_halmac_version.c $(HALMAC_VERSION_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_version.c $(HALMAC_VERSION_ORACLE) $(HOST_JSON)

test_hal_halmac_network: test_hal_halmac_network.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_network.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_bcn: test_hal_halmac_bcn.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_bcn.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_rsvd: test_hal_halmac_rsvd.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_rsvd.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_trx_share: test_hal_halmac_trx_share.c $(HALMAC_TRX_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_trx_share.c $(HALMAC_TRX_ORACLE) $(HOST_JSON)

test_hal_halmac_drv2halmac: test_hal_halmac_drv2halmac.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_drv2halmac.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_rf_type: test_hal_halmac_rf_type.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_rf_type.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_usb_fifo: test_hal_halmac_usb_fifo.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_usb_fifo.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_fw_led_phy: test_hal_halmac_fw_led_phy.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_fw_led_phy.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_phy_param_drv2halmac: test_hal_halmac_phy_param_drv2halmac.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_phy_param_drv2halmac.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_usb_fifo_rust: test_hal_halmac_usb_fifo.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_usb_fifo.c \
		$(HOST_JSON) librust_hal_halmac.a

test_hal_halmac_rf_type_rust: test_hal_halmac_rf_type.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_rf_type.c \
		$(HOST_JSON) librust_hal_halmac.a

test-halmac-drv2halmac: test_hal_halmac_drv2halmac
	./test_hal_halmac_drv2halmac hal_halmac_hw_port_drv2halmac_vectors.json \
		hal_halmac_network_drv2halmac_vectors.json

test-halmac-drv2halmac-rust: test_hal_halmac_drv2halmac_rust
	./test_hal_halmac_drv2halmac_rust hal_halmac_hw_port_drv2halmac_vectors.json \
		hal_halmac_network_drv2halmac_vectors.json

test_hal_halmac_drv2halmac_rust: test_hal_halmac_drv2halmac.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_drv2halmac.c \
		$(HOST_JSON) librust_hal_halmac.a

test-halmac-trx-share: test_hal_halmac_trx_share
	./test_hal_halmac_trx_share hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json

test-halmac-trx-share-rust: test_hal_halmac_trx_share_rust
	./test_hal_halmac_trx_share_rust hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json

librust_hal_halmac.a: $(RUST_HAL_HALMAC)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_halmac_enum_test \
		--crate-type staticlib $(RUST_HAL_HALMAC) -o $@

librust_hal_halmac_with_version.a: $(RUST_HAL_HALMAC)
	rustc -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_halmac_enum_test --cfg host_hal_halmac_version_test \
		--crate-type staticlib $(RUST_HAL_HALMAC) -o $@

test_hal_halmac_bcn_abi: test_hal_halmac_bcn_abi.c
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_bcn_abi.c

test_hal_halmac_bcn_abi_rust: test_hal_halmac_bcn_abi.c librust_hal_halmac.a
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_bcn_abi.c \
		librust_hal_halmac.a

test-halmac-rust: test_hal_halmac_network_rust test_hal_halmac_bcn_rust \
	test_hal_halmac_bcn_abi_rust test_hal_halmac_rsvd_rust test_hal_halmac_trx_share_rust \
	test_hal_halmac_drv2halmac_rust test_hal_halmac_rf_type_rust \
	test_hal_halmac_usb_fifo_rust test_hal_halmac_fw_led_phy_rust \
	test_hal_halmac_phy_param_drv2halmac_rust test_hal_halmac_version_rust
	./test_hal_halmac_network_rust hal_halmac_network_vectors.json
	./test_hal_halmac_bcn_rust hal_halmac_bcn_vectors.json
	./test_hal_halmac_bcn_abi_rust
	./test_hal_halmac_rsvd_rust hal_halmac_rsvd_drv2halmac_vectors.json \
		hal_halmac_rsvd_halmac2drv_vectors.json
	./test_hal_halmac_trx_share_rust hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json
	./test_hal_halmac_drv2halmac_rust hal_halmac_hw_port_drv2halmac_vectors.json \
		hal_halmac_network_drv2halmac_vectors.json
	./test_hal_halmac_rf_type_rust hal_halmac_rf_type_drv2halmac_vectors.json \
		hal_halmac_rf_type_halmac2drv_vectors.json
	./test_hal_halmac_usb_fifo_rust hal_halmac_fifo_sel_drv2halmac_vectors.json \
		hal_halmac_usb_mode_drv2halmac_vectors.json
	./test_hal_halmac_fw_led_phy_rust hal_halmac_fw_mem_drv2halmac_vectors.json \
		hal_halmac_led_mode_drv2halmac_vectors.json \
		hal_halmac_phy_param_val_drv2halmac_vectors.json
	./test_hal_halmac_phy_param_drv2halmac_rust hal_halmac_phy_param_drv2halmac_vectors.json
	./test_hal_halmac_version_rust hal_halmac_get_version_vectors.json

test_hal_halmac_phy_param_drv2halmac_rust: test_hal_halmac_phy_param_drv2halmac.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_phy_param_drv2halmac.c \
		$(HOST_JSON) librust_hal_halmac.a

test_hal_halmac_version_rust: test_hal_halmac_version.c $(HALMAC_VERSION_ORACLE) \
	librust_hal_halmac_with_version.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_version.c \
		$(HALMAC_VERSION_ORACLE) $(HOST_JSON) librust_hal_halmac_with_version.a

test_hal_halmac_fw_led_phy_rust: test_hal_halmac_fw_led_phy.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_fw_led_phy.c \
		$(HOST_JSON) librust_hal_halmac.a

test-halmac-rsvd: test_hal_halmac_rsvd
	./test_hal_halmac_rsvd hal_halmac_rsvd_drv2halmac_vectors.json \
		hal_halmac_rsvd_halmac2drv_vectors.json

test-halmac-rsvd-rust: test_hal_halmac_rsvd_rust
	./test_hal_halmac_rsvd_rust hal_halmac_rsvd_drv2halmac_vectors.json \
		hal_halmac_rsvd_halmac2drv_vectors.json

test_hal_halmac_network_rust: test_hal_halmac_network.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_network.c \
		$(HOST_JSON) librust_hal_halmac.a

test_hal_halmac_bcn_rust: test_hal_halmac_bcn.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_bcn.c \
		$(HOST_JSON) librust_hal_halmac.a

test_hal_halmac_rsvd_rust: test_hal_halmac_rsvd.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_rsvd.c \
		$(HOST_JSON) librust_hal_halmac.a

test_hal_halmac_trx_share_rust: test_hal_halmac_trx_share.c librust_hal_halmac.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_trx_share.c \
		$(HOST_JSON) librust_hal_halmac.a

LEGAL_CH_ORACLE := hal_is_legal_channel_c_oracle.c
CHPLAN_HW_ORACLE := hal_config_chplan_hw_c_oracle.c
CHPLAN_CORE := $(ROOT)/core/rtw_chplan.c
CHPLAN_DATA := chplan_hw_data.o
CHPLAN_SHIM := ../shim/host_chplan_rf.c ../shim/host_chplan_hal.c
CHPLAN_CFLAGS := $(CFLAGS) -I$(ROOT)/core -I$(ROOT)/include \
	-include $(HOST_INC)/host_autoconf.h -DHOST_CHPLAN_TEST \
	-Wno-unterminated-string-initialization -Wno-unused-const-variable

test-legal-channel: test_hal_is_legal_channel
	./test_hal_is_legal_channel hal_is_legal_channel_vectors.json

test_hal_is_legal_channel: test_hal_is_legal_channel.c $(LEGAL_CH_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_is_legal_channel.c $(LEGAL_CH_ORACLE) $(HOST_JSON)

librust_hal_com_legal.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_legal_channel_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-legal-channel-rust: test_hal_is_legal_channel_rust
	./test_hal_is_legal_channel_rust hal_is_legal_channel_vectors.json

test_hal_is_legal_channel_rust: test_hal_is_legal_channel.c librust_hal_com_legal.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_LEGAL_CHANNEL_ORACLE -o $@ test_hal_is_legal_channel.c \
		$(HOST_JSON) librust_hal_com_legal.a

test-config-chplan-hw: test_hal_config_chplan_hw
	./test_hal_config_chplan_hw hal_config_chplan_hw_vectors.json

chplan_hw_data.o: $(ROOT)/core/rtw_chplan_rest.c
	$(CC) $(CHPLAN_CFLAGS) -DHOST_CHPLAN_DATA_ONLY -c -o $@ $(ROOT)/core/rtw_chplan_rest.c

test_hal_config_chplan_hw: test_hal_config_chplan_hw.c $(CHPLAN_HW_ORACLE) \
		$(CHPLAN_CORE) $(CHPLAN_DATA) $(CHPLAN_SHIM) $(HOST_JSON)
	$(CC) $(CHPLAN_CFLAGS) -o $@ test_hal_config_chplan_hw.c $(CHPLAN_HW_ORACLE) \
		$(CHPLAN_CORE) $(CHPLAN_DATA) $(CHPLAN_SHIM) $(HOST_JSON)

RUST_CHPLAN_HW_HOST := $(ROOT)/rust/hal_com_config_chplan_hw_host.rs

librust_hal_com_config_chplan_hw.a: $(RUST_CHPLAN_HW_HOST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_config_chplan_hw_test \
		--crate-type staticlib $(RUST_CHPLAN_HW_HOST) -o $@

test-config-chplan-hw-rust: test_hal_config_chplan_hw_rust
	./test_hal_config_chplan_hw_rust hal_config_chplan_hw_vectors.json

test_hal_config_chplan_hw_rust: test_hal_config_chplan_hw.c \
		librust_hal_com_config_chplan_hw.a $(CHPLAN_CORE) $(CHPLAN_DATA) \
		$(CHPLAN_SHIM) $(HOST_JSON)
	$(CC) $(CHPLAN_CFLAGS) -DRUST_HAL_COM_CONFIG_CHPLAN_HW_ORACLE -o $@ \
		test_hal_config_chplan_hw.c $(CHPLAN_CORE) $(CHPLAN_DATA) \
		$(CHPLAN_SHIM) $(HOST_JSON) librust_hal_com_config_chplan_hw.a

test-config-chplan-full: test_hal_config_chplan_full
	./test_hal_config_chplan_full hal_config_chplan_full_vectors.json

test_hal_config_chplan_full: hal_config_chplan_full_l2.c \
		$(CHPLAN_CORE) $(CHPLAN_DATA) $(CHPLAN_SHIM) $(HOST_JSON)
	$(CC) $(CHPLAN_CFLAGS) -o $@ hal_config_chplan_full_l2.c \
		$(CHPLAN_CORE) $(CHPLAN_DATA) $(CHPLAN_SHIM) $(HOST_JSON)

RUST_CHPLAN_FULL_HOST := $(ROOT)/rust/hal_com_config_chplan_full_host.rs

librust_hal_com_config_chplan_full.a: $(RUST_CHPLAN_FULL_HOST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_config_chplan_full_test \
		--crate-type staticlib $(RUST_CHPLAN_FULL_HOST) -o $@

test-config-chplan-full-rust: test_hal_config_chplan_full_rust
	./test_hal_config_chplan_full_rust hal_config_chplan_full_vectors.json

test_hal_config_chplan_full_rust: hal_config_chplan_full_l2.c \
		librust_hal_com_config_chplan_full.a $(CHPLAN_CORE) $(CHPLAN_DATA) \
		$(CHPLAN_SHIM) $(HOST_JSON)
	$(CC) $(CHPLAN_CFLAGS) -DRUST_HAL_COM_CONFIG_CHPLAN_FULL_ORACLE -o $@ \
		hal_config_chplan_full_l2.c $(CHPLAN_CORE) $(CHPLAN_DATA) \
		$(CHPLAN_SHIM) $(HOST_JSON) librust_hal_com_config_chplan_full.a

MAPPING_ORACLE := hal_mapping_out_pipe_c_oracle.c

test-mapping-out-pipe: test_hal_mapping_out_pipe
	./test_hal_mapping_out_pipe hal_mapping_out_pipe_vectors.json

test_hal_mapping_out_pipe: test_hal_mapping_out_pipe.c $(MAPPING_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_mapping_out_pipe.c $(MAPPING_ORACLE) $(HOST_JSON)

librust_hal_com_mapping.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_mapping_out_pipe_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-mapping-out-pipe-rust: test_hal_mapping_out_pipe_rust
	./test_hal_mapping_out_pipe_rust hal_mapping_out_pipe_vectors.json

test_hal_mapping_out_pipe_rust: test_hal_mapping_out_pipe.c librust_hal_com_mapping.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_mapping_out_pipe.c \
		$(HOST_JSON) librust_hal_com_mapping.a

MGNTFRAME_RAID_ORACLE := hal_mgntframe_raid_c_oracle.c

test-mgntframe-raid: test_hal_mgntframe_raid
	./test_hal_mgntframe_raid hal_mgntframe_raid_vectors.json

test_hal_mgntframe_raid: test_hal_mgntframe_raid.c $(MGNTFRAME_RAID_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_mgntframe_raid.c $(MGNTFRAME_RAID_ORACLE) $(HOST_JSON)

librust_hal_com_mgntframe_raid.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_mgntframe_raid_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-mgntframe-raid-rust: test_hal_mgntframe_raid_rust
	./test_hal_mgntframe_raid_rust hal_mgntframe_raid_vectors.json

test_hal_mgntframe_raid_rust: test_hal_mgntframe_raid.c librust_hal_com_mgntframe_raid.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_mgntframe_raid.c \
		$(HOST_JSON) librust_hal_com_mgntframe_raid.a

UPDATE_STA_WSET_MIMO_ORACLE := hal_update_sta_wset_mimo_c_oracle.c

test-update-sta-wset-mimo: test_hal_update_sta_wset_mimo
	./test_hal_update_sta_wset_mimo hal_update_sta_wset_mimo_vectors.json

test_hal_update_sta_wset_mimo: test_hal_update_sta_wset_mimo.c \
		$(UPDATE_STA_WSET_MIMO_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_update_sta_wset_mimo.c \
		$(UPDATE_STA_WSET_MIMO_ORACLE) $(HOST_JSON)

librust_hal_com_update_sta_wset_mimo.a: ../../../rust/hal_com_update_sta_wset_mimo.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_update_sta_wset_mimo_test \
		--crate-type staticlib ../../../rust/hal_com_update_sta_wset_mimo.rs -o $@

test-update-sta-wset-mimo-rust: test_hal_update_sta_wset_mimo_rust
	./test_hal_update_sta_wset_mimo_rust hal_update_sta_wset_mimo_vectors.json

test_hal_update_sta_wset_mimo_rust: test_hal_update_sta_wset_mimo.c \
		librust_hal_com_update_sta_wset_mimo.a $(HOST_JSON)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_update_sta_wset_mimo.c \
		$(HOST_JSON) librust_hal_com_update_sta_wset_mimo.a

UPDATE_STA_SMPS_RATE_ORACLE := hal_update_sta_smps_rate_c_oracle.c

test-update-sta-smps-rate: test_hal_update_sta_smps_rate
	./test_hal_update_sta_smps_rate hal_update_sta_smps_rate_vectors.json

test_hal_update_sta_smps_rate: test_hal_update_sta_smps_rate.c \
		$(UPDATE_STA_SMPS_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_update_sta_smps_rate.c \
		$(UPDATE_STA_SMPS_RATE_ORACLE) $(HOST_JSON)

UPDATE_STA_SMPS_RATE_SHIM := hal_update_sta_smps_rate_host_shim.c

librust_hal_com_update_sta_smps_rate.a: ../../../rust/hal_com_update_sta_smps_rate.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_update_sta_smps_rate_test \
		--crate-type staticlib ../../../rust/hal_com_update_sta_smps_rate.rs -o $@

librust_hal_com_update_sta_smps_rate_kern.a: ../../../rust/hal_com_update_sta_smps_rate.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg config_80211n_ht --cfg config_80211ac_vht \
		--crate-type staticlib ../../../rust/hal_com_update_sta_smps_rate.rs -o $@

test-update-sta-smps-rate-rust: test_hal_update_sta_smps_rate_rust
	./test_hal_update_sta_smps_rate_rust hal_update_sta_smps_rate_vectors.json

test_hal_update_sta_smps_rate_rust: test_hal_update_sta_smps_rate.c \
		librust_hal_com_update_sta_smps_rate.a $(HOST_JSON)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_update_sta_smps_rate.c \
		$(HOST_JSON) librust_hal_com_update_sta_smps_rate.a

test-update-sta-smps-rate-rust-kern: test_hal_update_sta_smps_rate_rust_kern
	./test_hal_update_sta_smps_rate_rust_kern hal_update_sta_smps_rate_vectors.json

test_hal_update_sta_smps_rate_rust_kern: test_hal_update_sta_smps_rate.c \
		librust_hal_com_update_sta_smps_rate_kern.a $(UPDATE_STA_SMPS_RATE_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_update_sta_smps_rate.c \
		$(UPDATE_STA_SMPS_RATE_SHIM) $(HOST_JSON) librust_hal_com_update_sta_smps_rate_kern.a

UPDATE_STA_RA_INFO_ORACLE := hal_com_update_sta_ra_info_c_oracle.c
UPDATE_STA_RA_INFO_SHIM := hal_com_update_sta_ra_info_host_shim.c

test-update-sta-ra-info: test_hal_com_update_sta_ra_info
	./test_hal_com_update_sta_ra_info hal_com_update_sta_ra_info_vectors.json

test_hal_com_update_sta_ra_info: test_hal_com_update_sta_ra_info.c \
		$(UPDATE_STA_RA_INFO_ORACLE) $(UPDATE_STA_RA_INFO_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_update_sta_ra_info.c \
		$(UPDATE_STA_RA_INFO_ORACLE) $(UPDATE_STA_RA_INFO_SHIM) $(HOST_JSON)

RUST_HAL_COM_UPDATE_STA_RA_INFO := $(ROOT)/rust/hal_com_update_sta_ra_info_host.rs

librust_hal_com_update_sta_ra_info.a: $(RUST_HAL_COM_UPDATE_STA_RA_INFO)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_update_sta_ra_info_test \
		--crate-type staticlib $(RUST_HAL_COM_UPDATE_STA_RA_INFO) -o $@

test-update-sta-ra-info-rust: test_hal_com_update_sta_ra_info_rust
	./test_hal_com_update_sta_ra_info_rust hal_com_update_sta_ra_info_vectors.json

test_hal_com_update_sta_ra_info_rust: test_hal_com_update_sta_ra_info.c \
		librust_hal_com_update_sta_ra_info.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_UPDATE_STA_RA_INFO_ORACLE -I../include -o $@ \
		test_hal_com_update_sta_ra_info.c $(UPDATE_STA_RA_INFO_SHIM) $(HOST_JSON) \
		librust_hal_com_update_sta_ra_info.a

HDATA_RATE_ORACLE := hal_hdata_rate_c_oracle.c

test-hdata-rate: test_hal_hdata_rate
	./test_hal_hdata_rate hal_hdata_rate_vectors.json

test_hal_hdata_rate: test_hal_hdata_rate.c $(HDATA_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_hdata_rate.c $(HDATA_RATE_ORACLE) $(HOST_JSON)

librust_hal_com_hdata_rate.a: $(RUST_HAL)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_hdata_rate_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-hdata-rate-rust: test_hal_hdata_rate_rust
	./test_hal_hdata_rate_rust hal_hdata_rate_vectors.json

test_hal_hdata_rate_rust: test_hal_hdata_rate.c librust_hal_com_hdata_rate.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_hdata_rate.c \
		$(HOST_JSON) librust_hal_com_hdata_rate.a

test-brate: test_hal_set_brate_cfg
	./test_hal_set_brate_cfg hal_set_brate_cfg_vectors.json

test_hal_set_brate_cfg: test_hal_set_brate_cfg.c $(BRATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_set_brate_cfg.c $(BRATE_ORACLE) $(HOST_JSON)

librust_hal_com_brate.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_brate_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-brate-rust: test_hal_set_brate_cfg_rust
	./test_hal_set_brate_cfg_rust hal_set_brate_cfg_vectors.json

test_hal_set_brate_cfg_rust: test_hal_set_brate_cfg.c librust_hal_com_brate.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_COM_BRATE_ORACLE -o $@ test_hal_set_brate_cfg.c \
		$(HOST_JSON) librust_hal_com_brate.a

test-phy-rf-shadow: test_hal_phy_rf_shadow
	./test_hal_phy_rf_shadow hal_phy_rf_shadow_vectors.json

test_hal_phy_rf_shadow: test_hal_phy_rf_shadow.c $(PHY_RF_SHADOW_ORACLE) \
		$(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_phy_rf_shadow.c $(PHY_RF_SHADOW_ORACLE) \
		$(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON)

librust_hal_phy_rf_shadow.a: $(RUST_HAL_PHY)
	rustc -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_phy_rf_shadow_test \
		--crate-type staticlib $(RUST_HAL_PHY) -o $@

librust_hal_phy_rf_shadow_compare.a: $(RUST_HAL_PHY)
	rustc -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_phy_rf_shadow_test \
		--cfg host_hal_phy_rf_shadow_compare_test \
		--crate-type staticlib $(RUST_HAL_PHY) -o $@

test-phy-rf-shadow-rust: test_hal_phy_rf_shadow_rust
	./test_hal_phy_rf_shadow_rust hal_phy_rf_shadow_vectors.json

test_hal_phy_rf_shadow_rust: test_hal_phy_rf_shadow.c librust_hal_phy_rf_shadow.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_PHY_RF_SHADOW_ORACLE -o $@ test_hal_phy_rf_shadow.c \
		$(HOST_JSON) librust_hal_phy_rf_shadow.a

test-phy-rf-shadow-flags: test_hal_phy_rf_shadow_flags
	./test_hal_phy_rf_shadow_flags hal_phy_rf_shadow_flags_vectors.json

test_hal_phy_rf_shadow_flags: test_hal_phy_rf_shadow.c $(PHY_RF_SHADOW_ORACLE) \
		$(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_HAL_PHY_RF_SHADOW_FLAGS_TEST -o $@ test_hal_phy_rf_shadow.c \
		$(PHY_RF_SHADOW_ORACLE) $(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON)

test-phy-rf-shadow-compare: test_hal_phy_rf_shadow_compare
	./test_hal_phy_rf_shadow_compare hal_phy_rf_shadow_compare_vectors.json

test_hal_phy_rf_shadow_compare: test_hal_phy_rf_shadow.c $(PHY_RF_SHADOW_ORACLE) \
		$(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_HAL_PHY_RF_SHADOW_COMPARE_TEST -o $@ test_hal_phy_rf_shadow.c \
		$(PHY_RF_SHADOW_ORACLE) $(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON)

test-phy-rf-shadow-compare-rust: test_hal_phy_rf_shadow_compare_rust
	./test_hal_phy_rf_shadow_compare_rust hal_phy_rf_shadow_compare_vectors.json

test_hal_phy_rf_shadow_compare_rust: test_hal_phy_rf_shadow.c \
		librust_hal_phy_rf_shadow_compare.a $(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_PHY_RF_SHADOW_ORACLE -DHOST_HAL_PHY_RF_SHADOW_COMPARE_TEST \
		-o $@ test_hal_phy_rf_shadow.c $(PHY_RF_SHADOW_HW_SHIM) $(HOST_JSON) \
		librust_hal_phy_rf_shadow_compare.a

test-phy-rf-shadow-flags-rust: test_hal_phy_rf_shadow_flags_rust
	./test_hal_phy_rf_shadow_flags_rust hal_phy_rf_shadow_flags_vectors.json

test_hal_phy_rf_shadow_flags_rust: test_hal_phy_rf_shadow.c librust_hal_phy_rf_shadow.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_HAL_PHY_RF_SHADOW_FLAGS_TEST -DRUST_HAL_PHY_RF_SHADOW_ORACLE \
		-o $@ test_hal_phy_rf_shadow.c $(HOST_JSON) librust_hal_phy_rf_shadow.a

test-phy-rf-max-reg: test_hal_phy_rf_max_reg
	./test_hal_phy_rf_max_reg hal_phy_rf_max_reg_vectors.json

test_hal_phy_rf_max_reg: test_hal_phy_rf_max_reg.c $(PHY_RF_MAX_REG_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_phy_rf_max_reg.c \
		$(PHY_RF_MAX_REG_ORACLE) $(HOST_JSON)

librust_hal_phy_rf_max_reg.a: $(RUST_HAL_PHY)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_phy_rf_max_reg_test \
		--crate-type staticlib $(RUST_HAL_PHY) -o $@

test-phy-rf-max-reg-rust: test_hal_phy_rf_max_reg_rust
	./test_hal_phy_rf_max_reg_rust hal_phy_rf_max_reg_vectors.json

test_hal_phy_rf_max_reg_rust: test_hal_phy_rf_max_reg.c librust_hal_phy_rf_max_reg.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_PHY_RF_MAX_REG_ORACLE -o $@ test_hal_phy_rf_max_reg.c \
		$(HOST_JSON) librust_hal_phy_rf_max_reg.a

test-phy-bit-shift: test_hal_phy_bit_shift
	./test_hal_phy_bit_shift hal_phy_bit_shift_vectors.json

test_hal_phy_bit_shift: test_hal_phy_bit_shift.c $(PHY_BIT_SHIFT_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_phy_bit_shift.c $(PHY_BIT_SHIFT_ORACLE) \
		$(HOST_JSON)

librust_hal_phy.a: $(RUST_HAL_PHY)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_phy_bit_shift_test \
		--crate-type staticlib $(RUST_HAL_PHY) -o $@

test-phy-bit-shift-rust: test_hal_phy_bit_shift_rust
	./test_hal_phy_bit_shift_rust hal_phy_bit_shift_vectors.json

test_hal_phy_bit_shift_rust: test_hal_phy_bit_shift.c librust_hal_phy.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_PHY_ORACLE -o $@ test_hal_phy_bit_shift.c \
		$(HOST_JSON) librust_hal_phy.a

test-dm-board-type: test_hal_dm_board_type
	./test_hal_dm_board_type hal_dm_board_type_vectors.json

test_hal_dm_board_type: test_hal_dm_board_type.c $(DM_BOARD_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_dm_board_type.c $(DM_BOARD_ORACLE) $(HOST_JSON)

librust_hal_dm.a: $(RUST_HAL_DM)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_board_type_test --crate-type staticlib $(RUST_HAL_DM) -o $@

test-dm-board-type-rust: test_hal_dm_board_type_rust
	./test_hal_dm_board_type_rust hal_dm_board_type_vectors.json

test_hal_dm_board_type_rust: test_hal_dm_board_type.c librust_hal_dm.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_ORACLE -o $@ test_hal_dm_board_type.c \
		$(HOST_JSON) librust_hal_dm.a

test-dm-odm-endian-hooks: test_hal_dm_odm_endian_hooks
	./test_hal_dm_odm_endian_hooks hal_dm_odm_endian_hooks_vectors.json

test_hal_dm_odm_endian_hooks: test_hal_dm_odm_endian_hooks.c $(DM_ODM_ENDIAN_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_dm_odm_endian_hooks.c $(DM_ODM_ENDIAN_ORACLE) $(HOST_JSON)

librust_hal_dm_odm_endian.a: $(RUST_HAL_DM)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_odm_endian_hooks_test --crate-type staticlib $(RUST_HAL_DM) -o $@

test-dm-odm-endian-hooks-rust: test_hal_dm_odm_endian_hooks_rust
	./test_hal_dm_odm_endian_hooks_rust hal_dm_odm_endian_hooks_vectors.json

test_hal_dm_odm_endian_hooks_rust: test_hal_dm_odm_endian_hooks.c librust_hal_dm_odm_endian.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_ODM_ENDIAN_ORACLE -o $@ test_hal_dm_odm_endian_hooks.c \
		$(HOST_JSON) librust_hal_dm_odm_endian.a

test-dm-dump-sta-info: test_hal_dm_dump_sta_info
	./test_hal_dm_dump_sta_info hal_dm_dump_sta_info_vectors.json

test_hal_dm_dump_sta_info: test_hal_dm_dump_sta_info.c $(DM_DUMP_STA_SHIM) \
		$(HDATA_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_dm_dump_sta_info.c $(DM_DUMP_STA_SHIM) \
		$(HDATA_RATE_ORACLE) $(HOST_JSON)

librust_hal_dm_dump_sta.a: $(RUST_HAL_DM)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_board_type_test --cfg host_hal_dm_dump_sta_info_test \
		--crate-type staticlib $(RUST_HAL_DM) -o $@

test-dm-dump-sta-info-rust: test_hal_dm_dump_sta_info_rust
	./test_hal_dm_dump_sta_info_rust hal_dm_dump_sta_info_vectors.json

test_hal_dm_dump_sta_info_rust: test_hal_dm_dump_sta_info.c librust_hal_dm_dump_sta.a \
		$(DM_DUMP_STA_SHIM) $(HDATA_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_DUMP_STA_ORACLE -o $@ test_hal_dm_dump_sta_info.c \
		$(DM_DUMP_STA_SHIM) $(HDATA_RATE_ORACLE) $(HOST_JSON) librust_hal_dm_dump_sta.a

test-dm-dump-sta-traffic: test_hal_dm_dump_sta_traffic
	./test_hal_dm_dump_sta_traffic hal_dm_dump_sta_traffic_vectors.json

test_hal_dm_dump_sta_traffic: test_hal_dm_dump_sta_traffic.c $(DM_DUMP_STA_TRAFFIC_SHIM) \
		$(HDATA_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_dm_dump_sta_traffic.c $(DM_DUMP_STA_TRAFFIC_SHIM) \
		$(HDATA_RATE_ORACLE) $(HOST_JSON)

librust_hal_dm_dump_sta_traffic.a: $(RUST_HAL_DM)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_board_type_test --cfg host_hal_dm_dump_sta_info_test \
		--cfg host_hal_dm_dump_sta_traffic_test \
		--crate-type staticlib $(RUST_HAL_DM) -o $@

test-dm-dump-sta-traffic-rust: test_hal_dm_dump_sta_traffic_rust
	./test_hal_dm_dump_sta_traffic_rust hal_dm_dump_sta_traffic_vectors.json

test_hal_dm_dump_sta_traffic_rust: test_hal_dm_dump_sta_traffic.c librust_hal_dm_dump_sta_traffic.a \
		$(DM_DUMP_STA_TRAFFIC_SHIM) $(HDATA_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_DUMP_STA_TRAFFIC_ORACLE -o $@ test_hal_dm_dump_sta_traffic.c \
		$(DM_DUMP_STA_TRAFFIC_SHIM) $(HDATA_RATE_ORACLE) $(HOST_JSON) \
		librust_hal_dm_dump_sta_traffic.a

test-dm-odm-iface: test_hal_dm_odm_iface
	./test_hal_dm_odm_iface hal_dm_odm_iface_vectors.json

test_hal_dm_odm_iface: test_hal_dm_odm_iface.c $(DM_ODM_IFACE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ test_hal_dm_odm_iface.c \
		$(DM_ODM_IFACE_ORACLE) $(HOST_JSON)

librust_hal_dm_odm_iface.a: $(RUST_HAL_DM)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_odm_iface_test --crate-type staticlib $(RUST_HAL_DM) -o $@

test-dm-odm-iface-rust: test_hal_dm_odm_iface_rust
	./test_hal_dm_odm_iface_rust hal_dm_odm_iface_vectors.json

test_hal_dm_odm_iface_rust: test_hal_dm_odm_iface.c librust_hal_dm_odm_iface.a \
		$(DM_ODM_IFACE_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_ODM_IFACE_ORACLE -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_odm_iface.c $(DM_ODM_IFACE_SHIM) $(HOST_JSON) librust_hal_dm_odm_iface.a

test-dm-min-rssi-phy-cnt: test_hal_dm_min_rssi_phy_cnt
	./test_hal_dm_min_rssi_phy_cnt hal_dm_min_rssi_phy_cnt_vectors.json

test_hal_dm_min_rssi_phy_cnt: test_hal_dm_min_rssi_phy_cnt.c \
		$(DM_MIN_RSSI_PHY_CNT_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ test_hal_dm_min_rssi_phy_cnt.c \
		$(DM_MIN_RSSI_PHY_CNT_ORACLE) $(HOST_JSON)

test-dm-min-rssi-phy-cnt-rust: test_hal_dm_min_rssi_phy_cnt_rust
	./test_hal_dm_min_rssi_phy_cnt_rust hal_dm_min_rssi_phy_cnt_vectors.json

RUST_HAL_DM_MIN_RSSI_PHY_CNT := $(ROOT)/rust/hal_dm_min_rssi_phy_cnt_host.rs

librust_hal_dm_min_rssi_phy_cnt.a: $(RUST_HAL_DM_MIN_RSSI_PHY_CNT)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_HAL_DM_MIN_RSSI_PHY_CNT) -o $@

test_hal_dm_min_rssi_phy_cnt_rust: test_hal_dm_min_rssi_phy_cnt.c \
		librust_hal_dm_min_rssi_phy_cnt.a $(DM_MIN_RSSI_PHY_CNT_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_MIN_RSSI_PHY_CNT_ORACLE -I$(ROOT)/tests/host/include \
		-o $@ test_hal_dm_min_rssi_phy_cnt.c $(DM_MIN_RSSI_PHY_CNT_SHIM) \
		$(HOST_JSON) librust_hal_dm_min_rssi_phy_cnt.a

test-dm-get-odm-var: test_hal_dm_get_odm_var
	./test_hal_dm_get_odm_var hal_dm_get_odm_var_vectors.json

test_hal_dm_get_odm_var: test_hal_dm_get_odm_var.c $(DM_GET_ODM_VAR_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(DM_GET_ODM_VAR_CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_get_odm_var.c $(DM_GET_ODM_VAR_ORACLE) $(HOST_JSON)

RUST_HAL_DM_GET_ODM_VAR := $(ROOT)/rust/hal_dm_get_odm_var_host.rs

librust_hal_dm_get_odm_var.a: $(RUST_HAL_DM_GET_ODM_VAR)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg CONFIG_ANTENNA_DIVERSITY --cfg host_hal_dm_get_odm_var_test \
		--crate-type staticlib $(RUST_HAL_DM_GET_ODM_VAR) -o $@

test-dm-get-odm-var-rust: test_hal_dm_get_odm_var_rust
	./test_hal_dm_get_odm_var_rust hal_dm_get_odm_var_vectors.json

test_hal_dm_get_odm_var_rust: test_hal_dm_get_odm_var.c librust_hal_dm_get_odm_var.a \
		$(DM_GET_ODM_VAR_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(DM_GET_ODM_VAR_CFLAGS) -DRUST_HAL_DM_GET_ODM_VAR_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ test_hal_dm_get_odm_var.c \
		$(DM_GET_ODM_VAR_SHIM) $(HOST_JSON) librust_hal_dm_get_odm_var.a

test-dm-phydm-rrsr: test_hal_dm_phydm_rrsr
	./test_hal_dm_phydm_rrsr hal_dm_phydm_rrsr_vectors.json

test_hal_dm_phydm_rrsr: test_hal_dm_phydm_rrsr.c $(DM_PHYDM_RRSR_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ test_hal_dm_phydm_rrsr.c \
		$(DM_PHYDM_RRSR_ORACLE) $(HOST_JSON)

RUST_HAL_DM_PHYDM_RRSR := $(ROOT)/rust/hal_dm_phydm_rrsr_host.rs

librust_hal_dm_phydm_rrsr.a: $(RUST_HAL_DM_PHYDM_RRSR)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_phydm_rrsr_test --crate-type staticlib \
		$(RUST_HAL_DM_PHYDM_RRSR) -o $@

test-dm-phydm-rrsr-rust: test_hal_dm_phydm_rrsr_rust
	./test_hal_dm_phydm_rrsr_rust hal_dm_phydm_rrsr_vectors.json

test_hal_dm_phydm_rrsr_rust: test_hal_dm_phydm_rrsr.c librust_hal_dm_phydm_rrsr.a \
		$(DM_PHYDM_RRSR_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_RRSR_ORACLE -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_rrsr.c $(DM_PHYDM_RRSR_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_rrsr.a

test-dm-phydm-crystal-cap: test_hal_dm_phydm_crystal_cap
	./test_hal_dm_phydm_crystal_cap hal_dm_phydm_crystal_cap_vectors.json

test_hal_dm_phydm_crystal_cap: test_hal_dm_phydm_crystal_cap.c \
		$(DM_PHYDM_CRYSTAL_CAP_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_crystal_cap.c $(DM_PHYDM_CRYSTAL_CAP_ORACLE) $(HOST_JSON)

RUST_HAL_DM_PHYDM_CRYSTAL_CAP := $(ROOT)/rust/hal_dm_phydm_crystal_cap_host.rs
RUST_HAL_DM_CRYSTAL_CAP_CFG := --cfg host_hal_dm_phydm_crystal_cap_test

librust_hal_dm_phydm_crystal_cap.a: $(RUST_HAL_DM_PHYDM_CRYSTAL_CAP)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_CRYSTAL_CAP_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_PHYDM_CRYSTAL_CAP) -o $@

test-dm-phydm-crystal-cap-rust: test_hal_dm_phydm_crystal_cap_rust
	./test_hal_dm_phydm_crystal_cap_rust hal_dm_phydm_crystal_cap_vectors.json

test_hal_dm_phydm_crystal_cap_rust: test_hal_dm_phydm_crystal_cap.c \
		librust_hal_dm_phydm_crystal_cap.a $(DM_PHYDM_CRYSTAL_CAP_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_CRYSTAL_CAP_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_crystal_cap.c $(DM_PHYDM_CRYSTAL_CAP_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_crystal_cap.a

test-dm-runtime-trx-path: test_hal_dm_runtime_trx_path
	./test_hal_dm_runtime_trx_path hal_dm_runtime_trx_path_vectors.json

test_hal_dm_runtime_trx_path: test_hal_dm_runtime_trx_path.c \
		$(DM_RUNTIME_TRX_ORACLE) $(DM_RUNTIME_TRX_SHIM) $(HOST_JSON)
	$(CC) $(RUNTIME_TRX_CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_runtime_trx_path.c $(DM_RUNTIME_TRX_ORACLE) \
		$(DM_RUNTIME_TRX_SHIM) $(HOST_JSON)

RUST_HAL_DM_RUNTIME_TRX := $(ROOT)/rust/hal_dm_runtime_trx_path_host.rs

librust_hal_dm_runtime_trx_path.a: $(RUST_HAL_DM_RUNTIME_TRX)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_runtime_trx_path_test \
		--cfg CONFIG_RTL8822B --cfg CONFIG_RTW_PATH_DIV \
		--cfg CONFIG_RTW_TX_NPATH_EN --crate-type staticlib \
		$(RUST_HAL_DM_RUNTIME_TRX) -o $@

test-dm-runtime-trx-path-rust: test_hal_dm_runtime_trx_path_rust
	./test_hal_dm_runtime_trx_path_rust hal_dm_runtime_trx_path_vectors.json

test_hal_dm_runtime_trx_path_rust: test_hal_dm_runtime_trx_path.c \
		librust_hal_dm_runtime_trx_path.a $(DM_RUNTIME_TRX_SHIM) $(HOST_JSON)
	$(CC) $(RUNTIME_TRX_CFLAGS) -DRUST_HAL_DM_RUNTIME_TRX_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_runtime_trx_path.c $(DM_RUNTIME_TRX_SHIM) $(HOST_JSON) \
		librust_hal_dm_runtime_trx_path.a

test-dm-phydm-iqk-trigger-dbg: test_hal_dm_phydm_iqk_trigger_dbg
	./test_hal_dm_phydm_iqk_trigger_dbg hal_dm_phydm_iqk_trigger_dbg_vectors.json

test_hal_dm_phydm_iqk_trigger_dbg: test_hal_dm_phydm_iqk_trigger_dbg.c \
		$(DM_PHYDM_IQK_TRIGGER_DBG_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRTL8822B_SUPPORT=1 -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_iqk_trigger_dbg.c $(DM_PHYDM_IQK_TRIGGER_DBG_ORACLE) $(HOST_JSON)

RUST_HAL_DM_PHYDM_IQK_TRIGGER_DBG := $(ROOT)/rust/hal_dm_phydm_iqk_trigger_dbg_host.rs
RUST_HAL_DM_IQK_TRIGGER_DBG_CFG := --cfg host_hal_dm_phydm_iqk_trigger_dbg_test \
	--cfg CONFIG_RTL8822B

librust_hal_dm_phydm_iqk_trigger_dbg.a: $(RUST_HAL_DM_PHYDM_IQK_TRIGGER_DBG)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_IQK_TRIGGER_DBG_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_PHYDM_IQK_TRIGGER_DBG) -o $@

test-dm-phydm-iqk-trigger-dbg-rust: test_hal_dm_phydm_iqk_trigger_dbg_rust
	./test_hal_dm_phydm_iqk_trigger_dbg_rust hal_dm_phydm_iqk_trigger_dbg_vectors.json

test_hal_dm_phydm_iqk_trigger_dbg_rust: test_hal_dm_phydm_iqk_trigger_dbg.c \
		librust_hal_dm_phydm_iqk_trigger_dbg.a \
		$(DM_PHYDM_IQK_TRIGGER_DBG_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_IQK_TRIGGER_DBG_ORACLE \
		-DRTL8822B_SUPPORT=1 -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_iqk_trigger_dbg.c $(DM_PHYDM_IQK_TRIGGER_DBG_SHIM) \
		$(HOST_JSON) librust_hal_dm_phydm_iqk_trigger_dbg.a

test-dm-phydm-ra-registed: test_hal_dm_phydm_ra_registed
	./test_hal_dm_phydm_ra_registed hal_dm_phydm_ra_registed_vectors.json

test_hal_dm_phydm_ra_registed: test_hal_dm_phydm_ra_registed.c \
		$(DM_PHYDM_RA_REGISTED_ORACLE) $(DM_PHYDM_RA_REGISTED_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_ra_registed.c $(DM_PHYDM_RA_REGISTED_ORACLE) \
		$(DM_PHYDM_RA_REGISTED_SHIM) $(HOST_JSON)

RUST_HAL_DM_PHYDM_RA_REGISTED := $(ROOT)/rust/hal_dm_phydm_ra_registed_host.rs
RUST_HAL_DM_RA_REGISTED_CFG := --cfg host_hal_dm_phydm_ra_registed_test

librust_hal_dm_phydm_ra_registed.a: $(RUST_HAL_DM_PHYDM_RA_REGISTED)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_RA_REGISTED_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_PHYDM_RA_REGISTED) -o $@

test-dm-phydm-ra-registed-rust: test_hal_dm_phydm_ra_registed_rust
	./test_hal_dm_phydm_ra_registed_rust hal_dm_phydm_ra_registed_vectors.json

test_hal_dm_phydm_ra_registed_rust: test_hal_dm_phydm_ra_registed.c \
		librust_hal_dm_phydm_ra_registed.a $(DM_PHYDM_RA_REGISTED_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_RA_REGISTED_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_ra_registed.c $(DM_PHYDM_RA_REGISTED_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_ra_registed.a

test-dm-phydm-read-efuse-trim: test_hal_dm_phydm_read_efuse_trim
	./test_hal_dm_phydm_read_efuse_trim hal_dm_phydm_read_efuse_trim_vectors.json

test_hal_dm_phydm_read_efuse_trim: test_hal_dm_phydm_read_efuse_trim.c \
		$(DM_PHYDM_READ_EFUSE_TRIM_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_read_efuse_trim.c \
		$(DM_PHYDM_READ_EFUSE_TRIM_ORACLE) $(HOST_JSON)

RUST_HAL_DM_PHYDM_READ_EFUSE_TRIM := $(ROOT)/rust/hal_dm_phydm_read_efuse_trim_host.rs
RUST_HAL_DM_READ_EFUSE_TRIM_CFG := --cfg host_hal_dm_phydm_read_efuse_trim_test

librust_hal_dm_phydm_read_efuse_trim.a: $(RUST_HAL_DM_PHYDM_READ_EFUSE_TRIM)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_READ_EFUSE_TRIM_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_PHYDM_READ_EFUSE_TRIM) -o $@

test-dm-phydm-read-efuse-trim-rust: test_hal_dm_phydm_read_efuse_trim_rust
	./test_hal_dm_phydm_read_efuse_trim_rust hal_dm_phydm_read_efuse_trim_vectors.json

test_hal_dm_phydm_read_efuse_trim_rust: test_hal_dm_phydm_read_efuse_trim.c \
		librust_hal_dm_phydm_read_efuse_trim.a \
		$(DM_PHYDM_READ_EFUSE_TRIM_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_READ_EFUSE_TRIM_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_read_efuse_trim.c $(DM_PHYDM_READ_EFUSE_TRIM_SHIM) \
		$(HOST_JSON) librust_hal_dm_phydm_read_efuse_trim.a

test-dm-phydm-trx-cfg: test_hal_dm_phydm_trx_cfg
	./test_hal_dm_phydm_trx_cfg hal_dm_phydm_trx_cfg_vectors.json

test_hal_dm_phydm_trx_cfg: test_hal_dm_phydm_trx_cfg.c \
		$(DM_PHYDM_TRX_CFG_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(DM_PHYDM_TRX_CFG_CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_trx_cfg.c $(DM_PHYDM_TRX_CFG_ORACLE) $(HOST_JSON)

RUST_HAL_DM_PHYDM_TRX_CFG := $(ROOT)/rust/hal_dm_phydm_trx_cfg_host.rs
RUST_HAL_DM_TRX_CFG_CFG := --cfg CONFIG_CTRL_TXSS_BY_TP --cfg host_hal_dm_phydm_trx_cfg_test

librust_hal_dm_phydm_trx_cfg.a: $(RUST_HAL_DM_PHYDM_TRX_CFG)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_TRX_CFG_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_PHYDM_TRX_CFG) -o $@

test-dm-phydm-trx-cfg-rust: test_hal_dm_phydm_trx_cfg_rust
	./test_hal_dm_phydm_trx_cfg_rust hal_dm_phydm_trx_cfg_vectors.json

test_hal_dm_phydm_trx_cfg_rust: test_hal_dm_phydm_trx_cfg.c \
		librust_hal_dm_phydm_trx_cfg.a $(DM_PHYDM_TRX_CFG_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) $(DM_PHYDM_TRX_CFG_CFLAGS) -DRUST_HAL_DM_PHYDM_TRX_CFG_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_trx_cfg.c $(DM_PHYDM_TRX_CFG_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_trx_cfg.a

test-dm-cfg-phy-para: test_hal_dm_cfg_phy_para
	./test_hal_dm_cfg_phy_para hal_dm_cfg_phy_para_vectors.json

test_hal_dm_cfg_phy_para: test_hal_dm_cfg_phy_para.c \
		$(DM_CFG_PHY_PARA_ORACLE) $(DM_CFG_PHY_PARA_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_cfg_phy_para.c $(DM_CFG_PHY_PARA_ORACLE) \
		$(DM_CFG_PHY_PARA_SHIM) $(HOST_JSON)

RUST_HAL_DM_CFG_PHY_PARA := $(ROOT)/rust/hal_dm_cfg_phy_para_host.rs
RUST_HAL_DM_CFG_PHY_PARA_CFG := --cfg host_hal_dm_cfg_phy_para_test \
	--cfg config_rtw_halmac

librust_hal_dm_cfg_phy_para.a: $(RUST_HAL_DM_CFG_PHY_PARA)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_CFG_PHY_PARA_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_CFG_PHY_PARA) -o $@

test-dm-cfg-phy-para-rust: test_hal_dm_cfg_phy_para_rust
	./test_hal_dm_cfg_phy_para_rust hal_dm_cfg_phy_para_vectors.json

test_hal_dm_cfg_phy_para_rust: test_hal_dm_cfg_phy_para.c \
		librust_hal_dm_cfg_phy_para.a $(DM_CFG_PHY_PARA_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_CFG_PHY_PARA_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_cfg_phy_para.c $(DM_CFG_PHY_PARA_SHIM) $(HOST_JSON) \
		librust_hal_dm_cfg_phy_para.a

test-dm-halmac-dm-leaf: test_hal_dm_halmac_dm_leaf
	./test_hal_dm_halmac_dm_leaf hal_dm_halmac_dm_leaf_vectors.json

test_hal_dm_halmac_dm_leaf: test_hal_dm_halmac_dm_leaf.c \
		$(DM_HALMAC_DM_LEAF_ORACLE) $(DM_HALMAC_DM_LEAF_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_halmac_dm_leaf.c $(DM_HALMAC_DM_LEAF_ORACLE) \
		$(DM_HALMAC_DM_LEAF_SHIM) $(HOST_JSON)

RUST_HAL_DM_HALMAC_DM_LEAF := $(ROOT)/rust/hal_dm_halmac_dm_leaf_host.rs
RUST_HAL_DM_HALMAC_DM_LEAF_CFG := --cfg host_hal_dm_halmac_dm_leaf_test --cfg config_rtw_halmac

librust_hal_dm_halmac_dm_leaf.a: $(RUST_HAL_DM_HALMAC_DM_LEAF)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_HALMAC_DM_LEAF_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_HALMAC_DM_LEAF) -o $@

test-dm-halmac-dm-leaf-rust: test_hal_dm_halmac_dm_leaf_rust
	./test_hal_dm_halmac_dm_leaf_rust hal_dm_halmac_dm_leaf_vectors.json

test_hal_dm_halmac_dm_leaf_rust: test_hal_dm_halmac_dm_leaf.c \
		librust_hal_dm_halmac_dm_leaf.a $(DM_HALMAC_DM_LEAF_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_HALMAC_DM_LEAF_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_halmac_dm_leaf.c $(DM_HALMAC_DM_LEAF_SHIM) $(HOST_JSON) \
		librust_hal_dm_halmac_dm_leaf.a

DM_PHYDM_INIT_ORACLE := hal_dm_phydm_init_c_oracle.c
DM_PHYDM_INIT_SHIM := hal_dm_phydm_init_host_shim.c
PHYDM_INIT_CFLAGS := $(CFLAGS) -DCONFIG_RTL8822B

test-dm-phydm-init: test_hal_dm_phydm_init
	./test_hal_dm_phydm_init hal_dm_phydm_init_vectors.json

test_hal_dm_phydm_init: test_hal_dm_phydm_init.c \
		$(DM_PHYDM_INIT_ORACLE) $(DM_PHYDM_INIT_SHIM) $(HOST_JSON)
	$(CC) $(PHYDM_INIT_CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_init.c $(DM_PHYDM_INIT_ORACLE) \
		$(DM_PHYDM_INIT_SHIM) $(HOST_JSON)

RUST_HAL_DM_PHYDM_INIT := $(ROOT)/rust/hal_dm_phydm_init_host.rs
RUST_HAL_DM_PHYDM_INIT_CFG := --cfg host_hal_dm_phydm_init_test --cfg CONFIG_RTL8822B

librust_hal_dm_phydm_init.a: $(RUST_HAL_DM_PHYDM_INIT)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		$(RUST_HAL_DM_PHYDM_INIT_CFG) --crate-type staticlib \
		$(RUST_HAL_DM_PHYDM_INIT) -o $@

test-dm-phydm-init-rust: test_hal_dm_phydm_init_rust
	./test_hal_dm_phydm_init_rust hal_dm_phydm_init_vectors.json

test_hal_dm_phydm_init_rust: test_hal_dm_phydm_init.c \
		librust_hal_dm_phydm_init.a $(DM_PHYDM_INIT_SHIM) $(HOST_JSON)
	$(CC) $(PHYDM_INIT_CFLAGS) -DRUST_HAL_DM_PHYDM_INIT_ORACLE \
		-I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_init.c $(DM_PHYDM_INIT_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_init.a

DM_INIT_ODM_TAIL_SHIM := hal_dm_init_odm_cominfo_tail_shim.c
DM_INIT_ODM_TAIL_ORACLE := hal_dm_init_odm_cominfo_tail_c_oracle.c

test-dm-init-odm-cominfo-tail-rust: test_hal_dm_init_odm_cominfo_tail_rust
	./test_hal_dm_init_odm_cominfo_tail_rust

RUST_INIT_ODM_COMINFO_TAIL := $(ROOT)/rust/hal_dm_init_odm_cominfo_tail_host.rs

librust_hal_dm_init_odm_cominfo_tail.a: $(RUST_INIT_ODM_COMINFO_TAIL)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_INIT_ODM_COMINFO_TAIL) -o $@

test_hal_dm_init_odm_cominfo_tail_rust: test_hal_dm_init_odm_cominfo_tail.c \
		$(DM_INIT_ODM_TAIL_SHIM) librust_hal_dm_init_odm_cominfo_tail.a
	$(CC) $(CFLAGS) -DCONFIG_USB_HCI -I$(ROOT)/tests/host/include -o $@ test_hal_dm_init_odm_cominfo_tail.c \
		$(DM_INIT_ODM_TAIL_SHIM) librust_hal_dm_init_odm_cominfo_tail.a

test-dm-init-odm-cominfo-tail: test_hal_dm_init_odm_cominfo_tail
	./test_hal_dm_init_odm_cominfo_tail

test_hal_dm_init_odm_cominfo_tail: test_hal_dm_init_odm_cominfo_tail.c $(DM_INIT_ODM_TAIL_ORACLE)
	$(CC) $(CFLAGS) -DCONFIG_USB_HCI -I$(ROOT)/tests/host/include -o $@ test_hal_dm_init_odm_cominfo_tail.c \
		$(DM_INIT_ODM_TAIL_ORACLE)

test-dm-phydm-ops: test_hal_dm_phydm_ops
	./test_hal_dm_phydm_ops

test_hal_dm_phydm_ops: test_hal_dm_phydm_ops.c $(DM_PHYDM_OPS_ORACLE) $(DM_PHYDM_OPS_SHIM)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_ops.c $(DM_PHYDM_OPS_ORACLE) $(DM_PHYDM_OPS_SHIM)

.PHONY: test-dm-phydm-ops-rust
test-dm-phydm-ops-rust: test_hal_dm_phydm_ops_rust
	./test_hal_dm_phydm_ops_rust

RUST_PHYDM_OPS := $(ROOT)/rust/hal_dm_phydm_ops_host.rs

librust_hal_dm_phydm_ops.a: $(RUST_PHYDM_OPS)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_phydm_ops_test --cfg CONFIG_SUPPORT_DYNAMIC_TXPWR \
		--crate-type staticlib $(RUST_PHYDM_OPS) -o $@

test_hal_dm_phydm_ops_rust: test_hal_dm_phydm_ops.c librust_hal_dm_phydm_ops.a \
		$(DM_PHYDM_OPS_SHIM)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_OPS_ORACLE -DCONFIG_SUPPORT_DYNAMIC_TXPWR \
		-I$(ROOT)/tests/host/include -o $@ test_hal_dm_phydm_ops.c \
		$(DM_PHYDM_OPS_SHIM) librust_hal_dm_phydm_ops.a

test-dm-phydm-igi-iqk: test_hal_dm_phydm_igi_iqk
	./test_hal_dm_phydm_igi_iqk hal_dm_phydm_igi_iqk_vectors.json

test_hal_dm_phydm_igi_iqk: test_hal_dm_phydm_igi_iqk.c $(DM_PHYDM_IGI_IQK_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ test_hal_dm_phydm_igi_iqk.c \
		$(DM_PHYDM_IGI_IQK_ORACLE) $(HOST_JSON)

RUST_HAL_DM_PHYDM_IGI_IQK := $(ROOT)/rust/hal_dm_phydm_igi_iqk_host.rs

librust_hal_dm_phydm_igi_iqk.a: $(RUST_HAL_DM_PHYDM_IGI_IQK)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_HAL_DM_PHYDM_IGI_IQK) -o $@

test-dm-phydm-igi-iqk-rust: test_hal_dm_phydm_igi_iqk_rust
	./test_hal_dm_phydm_igi_iqk_rust hal_dm_phydm_igi_iqk_vectors.json

test_hal_dm_phydm_igi_iqk_rust: test_hal_dm_phydm_igi_iqk.c librust_hal_dm_phydm_igi_iqk.a \
		$(DM_PHYDM_IGI_IQK_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_IGI_IQK_ORACLE -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_igi_iqk.c $(DM_PHYDM_IGI_IQK_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_igi_iqk.a

test-dm-set-odm-var-core: test_hal_dm_set_odm_var_core
	./test_hal_dm_set_odm_var_core hal_dm_set_odm_var_core_vectors.json

test_hal_dm_set_odm_var_core: test_hal_dm_set_odm_var_core.c \
		$(DM_SET_ODM_VAR_CORE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ test_hal_dm_set_odm_var_core.c \
		$(DM_SET_ODM_VAR_CORE_ORACLE) $(HOST_JSON)

RUST_HAL_DM_SET_ODM_VAR_CORE := $(ROOT)/rust/hal_dm_set_odm_var_core_host.rs

librust_hal_dm_set_odm_var_core.a: $(RUST_HAL_DM_SET_ODM_VAR_CORE)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_dm_set_odm_var_core_with_rx \
		--crate-type staticlib $(RUST_HAL_DM_SET_ODM_VAR_CORE) -o $@

test-dm-set-odm-var-core-rust: test_hal_dm_set_odm_var_core_rust
	./test_hal_dm_set_odm_var_core_rust hal_dm_set_odm_var_core_vectors.json

test_hal_dm_set_odm_var_core_rust: test_hal_dm_set_odm_var_core.c \
		librust_hal_dm_set_odm_var_core.a $(DM_SET_ODM_VAR_CORE_SHIM) \
		$(DM_SET_ODM_VAR_RX_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_SET_ODM_VAR_CORE_ORACLE -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_set_odm_var_core.c $(DM_SET_ODM_VAR_CORE_SHIM) \
		$(DM_SET_ODM_VAR_RX_SHIM) $(HOST_JSON) \
		librust_hal_dm_set_odm_var_core.a
test-dm-phydm-iqk-lck: test_hal_dm_phydm_iqk_lck
	./test_hal_dm_phydm_iqk_lck hal_dm_phydm_iqk_lck_vectors.json

test_hal_dm_phydm_iqk_lck: test_hal_dm_phydm_iqk_lck.c $(DM_PHYDM_IQK_LCK_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(DM_PHYDM_IQK_LCK_CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_iqk_lck.c $(DM_PHYDM_IQK_LCK_ORACLE) $(HOST_JSON)

RUST_HAL_DM_PHYDM_IQK_LCK := $(ROOT)/rust/hal_dm_phydm_iqk_lck_host.rs
RUST_HAL_DM_IQK_LCK_CFG := --cfg CONFIG_RTL8822B --cfg host_hal_dm_phydm_iqk_lck_test

librust_hal_dm_phydm_iqk_lck.a: $(RUST_HAL_DM_PHYDM_IQK_LCK)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on $(RUST_HAL_DM_IQK_LCK_CFG) \
		--crate-type staticlib $(RUST_HAL_DM_PHYDM_IQK_LCK) -o $@

test-dm-phydm-iqk-lck-rust: test_hal_dm_phydm_iqk_lck_rust
	./test_hal_dm_phydm_iqk_lck_rust hal_dm_phydm_iqk_lck_vectors.json

test_hal_dm_phydm_iqk_lck_rust: test_hal_dm_phydm_iqk_lck.c librust_hal_dm_phydm_iqk_lck.a \
		$(DM_PHYDM_IQK_LCK_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_IQK_LCK_ORACLE -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_iqk_lck.c $(DM_PHYDM_IQK_LCK_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_iqk_lck.a

test-dm-set-odm-var-rx: test_hal_dm_set_odm_var_rx
	./test_hal_dm_set_odm_var_rx hal_dm_set_odm_var_rx_vectors.json

test_hal_dm_set_odm_var_rx: test_hal_dm_set_odm_var_rx.c \
		$(DM_SET_ODM_VAR_RX_ORACLE) $(DM_SET_ODM_VAR_RX_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ test_hal_dm_set_odm_var_rx.c \
		$(DM_SET_ODM_VAR_RX_ORACLE) $(DM_SET_ODM_VAR_RX_SHIM) $(HOST_JSON)

DYNTPWR_CFLAGS := $(CFLAGS) -DCONFIG_SUPPORT_DYNAMIC_TXPWR -DCONFIG_RTL8822B \
	-I$(ROOT)/tests/host/include

DYN_SOML_CFLAGS := $(CFLAGS) -DCONFIG_DYNAMIC_SOML -I$(ROOT)/tests/host/include

test-dm-dyn-soml: test_hal_dm_dyn_soml
	./test_hal_dm_dyn_soml

test_hal_dm_dyn_soml: test_hal_dm_dyn_soml.c $(DM_DYN_SOML_ORACLE) $(DM_DYN_SOML_SHIM) \
		$(ROOT)/tests/host/include/host_hal_dm_dyn_soml_types.h
	$(CC) $(DYN_SOML_CFLAGS) -o $@ test_hal_dm_dyn_soml.c $(DM_DYN_SOML_ORACLE) \
		$(DM_DYN_SOML_SHIM)

RUST_HAL_DM_DYN_SOML := $(ROOT)/rust/hal_dm_dyn_soml_host.rs
RUST_HAL_DM_DYN_SOML_CFG := --cfg host_hal_dm_dyn_soml_test --cfg config_dynamic_soml

librust_hal_dm_dyn_soml.a: $(RUST_HAL_DM_DYN_SOML)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on $(RUST_HAL_DM_DYN_SOML_CFG) \
		--crate-type staticlib $(RUST_HAL_DM_DYN_SOML) -o $@

test-dm-dyn-soml-rust: test_hal_dm_dyn_soml_rust
	./test_hal_dm_dyn_soml_rust

test_hal_dm_dyn_soml_rust: test_hal_dm_dyn_soml.c librust_hal_dm_dyn_soml.a \
		$(DM_DYN_SOML_ORACLE) $(DM_DYN_SOML_SHIM) $(DM_DYN_SOML_RUST_SHIM) \
		$(ROOT)/tests/host/include/host_hal_dm_dyn_soml_types.h
	$(CC) $(DYN_SOML_CFLAGS) -DRUST_HAL_DM_DYN_SOML_ORACLE -o $@ \
		test_hal_dm_dyn_soml.c $(DM_DYN_SOML_SHIM) $(DM_DYN_SOML_RUST_SHIM) \
		$(DM_DYN_SOML_ORACLE) librust_hal_dm_dyn_soml.a

test-dm-dyntxpwr: test_hal_dm_dyntxpwr
	./test_hal_dm_dyntxpwr hal_dm_dyntxpwr_vectors.json

test_hal_dm_dyntxpwr: test_hal_dm_dyntxpwr.c $(DM_DYNTPWR_ORACLE) $(DM_DYNTPWR_SHIM) \
		$(HOST_JSON) $(ROOT)/tests/host/include/host_hal_dm_dyntxpwr_types.h
	$(CC) $(DYNTPWR_CFLAGS) -o $@ test_hal_dm_dyntxpwr.c $(DM_DYNTPWR_ORACLE) \
		$(DM_DYNTPWR_SHIM) $(HOST_JSON)

test-dm-phydm-cal-trigger: test_hal_dm_phydm_cal_trigger
	./test_hal_dm_phydm_cal_trigger hal_dm_phydm_cal_trigger_vectors.json

test_hal_dm_phydm_cal_trigger: test_hal_dm_phydm_cal_trigger.c \
		$(DM_PHYDM_CAL_TRIGGER_ORACLE) $(DM_PHYDM_CAL_TRIGGER_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_cal_trigger.c $(DM_PHYDM_CAL_TRIGGER_ORACLE) \
		$(DM_PHYDM_CAL_TRIGGER_SHIM) $(HOST_JSON)

test-dm-dbg-rf-cal-test: test_hal_dm_dbg_rf_cal_test
	./test_hal_dm_dbg_rf_cal_test hal_dm_dbg_rf_cal_test_vectors.json

test_hal_dm_dbg_rf_cal_test: test_hal_dm_dbg_rf_cal_test.c \
		$(DM_DBG_RF_CAL_TEST_ORACLE) $(DM_DBG_RF_CAL_TEST_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_dbg_rf_cal_test.c $(DM_DBG_RF_CAL_TEST_ORACLE) \
		$(DM_DBG_RF_CAL_TEST_SHIM) $(HOST_JSON)

RUST_HAL_DM_DBG_RF_CAL_TEST := $(ROOT)/rust/hal_dm_dbg_rf_cal_test_host.rs
RUST_HAL_DM_DBG_RF_CAL_CFG := --cfg host_hal_dm_dbg_rf_cal_test

librust_hal_dm_dbg_rf_cal_test.a: $(RUST_HAL_DM_DBG_RF_CAL_TEST)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on $(RUST_HAL_DM_DBG_RF_CAL_CFG) \
		--crate-type staticlib $(RUST_HAL_DM_DBG_RF_CAL_TEST) -o $@

test-dm-dbg-rf-cal-test-rust: test_hal_dm_dbg_rf_cal_test_rust
	./test_hal_dm_dbg_rf_cal_test_rust hal_dm_dbg_rf_cal_test_vectors.json

test_hal_dm_dbg_rf_cal_test_rust: test_hal_dm_dbg_rf_cal_test.c \
		librust_hal_dm_dbg_rf_cal_test.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_DBG_RF_CAL_TEST_ORACLE -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_dbg_rf_cal_test.c $(DM_DBG_RF_CAL_TEST_SHIM) $(HOST_JSON) \
		librust_hal_dm_dbg_rf_cal_test.a

RUST_HAL_DM_PHYDM_CAL_TRIGGER := $(ROOT)/rust/hal_dm_phydm_cal_trigger_host.rs
RUST_HAL_DM_CAL_TRIGGER_CFG := --cfg host_hal_dm_phydm_cal_trigger_test

librust_hal_dm_phydm_cal_trigger.a: $(RUST_HAL_DM_PHYDM_CAL_TRIGGER)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on $(RUST_HAL_DM_CAL_TRIGGER_CFG) \
		--crate-type staticlib $(RUST_HAL_DM_PHYDM_CAL_TRIGGER) -o $@

test-dm-phydm-cal-trigger-rust: test_hal_dm_phydm_cal_trigger_rust
	./test_hal_dm_phydm_cal_trigger_rust hal_dm_phydm_cal_trigger_vectors.json

test_hal_dm_phydm_cal_trigger_rust: test_hal_dm_phydm_cal_trigger.c \
		librust_hal_dm_phydm_cal_trigger.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_DM_PHYDM_CAL_TRIGGER_ORACLE -I$(ROOT)/tests/host/include -o $@ \
		test_hal_dm_phydm_cal_trigger.c $(DM_PHYDM_CAL_TRIGGER_SHIM) $(HOST_JSON) \
		librust_hal_dm_phydm_cal_trigger.a

RUST_HAL_DM_DYNTPWR := $(ROOT)/rust/hal_dm_dyntxpwr_host.rs

librust_hal_dm_dyntxpwr.a: $(RUST_HAL_DM_DYNTPWR)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg CONFIG_SUPPORT_DYNAMIC_TXPWR --cfg CONFIG_RTL8822B \
		--cfg host_hal_dm_dyntxpwr_test --crate-type staticlib \
		$(RUST_HAL_DM_DYNTPWR) -o $@

test-dm-dyntxpwr-rust: test_hal_dm_dyntxpwr_rust
	./test_hal_dm_dyntxpwr_rust hal_dm_dyntxpwr_vectors.json

test_hal_dm_dyntxpwr_rust: test_hal_dm_dyntxpwr.c librust_hal_dm_dyntxpwr.a \
		$(DM_DYNTPWR_SHIM) $(HOST_JSON)
	$(CC) $(DYNTPWR_CFLAGS) -DRUST_HAL_DM_DYNTPWR_ORACLE -o $@ \
		test_hal_dm_dyntxpwr.c $(DM_DYNTPWR_SHIM) $(HOST_JSON) \
		librust_hal_dm_dyntxpwr.a

test-usb-vendor-wvalue: test_hal_usb_vendor_wvalue
	./test_hal_usb_vendor_wvalue hal_usb_vendor_wvalue_vectors.json

test_hal_usb_vendor_wvalue: test_hal_usb_vendor_wvalue.c $(USB_WVALUE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DCONFIG_RTL8710B -o $@ test_hal_usb_vendor_wvalue.c \
		$(USB_WVALUE_ORACLE) $(HOST_JSON)

librust_hal_usb_wvalue.a: $(RUST_HAL_USB)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_usb_wvalue_test \
		--crate-type staticlib $(RUST_HAL_USB) -o $@

test-usb-vendor-wvalue-rust: test_hal_usb_vendor_wvalue_rust
	./test_hal_usb_vendor_wvalue_rust hal_usb_vendor_wvalue_vectors.json

test_hal_usb_vendor_wvalue_rust: test_hal_usb_vendor_wvalue.c librust_hal_usb_wvalue.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_USB_ORACLE -o $@ test_hal_usb_vendor_wvalue.c \
		$(HOST_JSON) librust_hal_usb_wvalue.a

test-usb-sync-read: test_hal_usb_sync_read
	./test_hal_usb_sync_read hal_usb_sync_read_vectors.json

test_hal_usb_sync_read: test_hal_usb_sync_read.c $(USB_SYNC_READ_ORACLE) $(USB_WVALUE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DCONFIG_RTL8710B -o $@ test_hal_usb_sync_read.c \
		$(USB_SYNC_READ_ORACLE) $(USB_WVALUE_ORACLE) $(HOST_JSON)

librust_hal_usb_sync_read.a: $(RUST_HAL_USB)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_usb_read_test \
		--crate-type staticlib $(RUST_HAL_USB) -o $@

test-usb-sync-read-rust: test_hal_usb_sync_read_rust
	./test_hal_usb_sync_read_rust hal_usb_sync_read_vectors.json

test_hal_usb_sync_read_rust: test_hal_usb_sync_read.c librust_hal_usb_sync_read.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_USB_SYNC_READ_ORACLE -DCONFIG_RTL8710B -o $@ \
		test_hal_usb_sync_read.c $(HOST_JSON) librust_hal_usb_sync_read.a

test-chip-odm: test_chip_odm_map
	./test_chip_odm_map hal_intf_chip_odm_vectors.json

test_chip_odm_map: test_chip_odm_map.c $(CHIP_ODM_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_chip_odm_map.c $(CHIP_ODM_ORACLE) $(HOST_JSON)

librust_hal_intf_chip_odm.a: $(RUST_HAL_INTF)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_intf_chip_odm_test \
		--crate-type staticlib $(RUST_HAL_INTF) -o $@

test-chip-odm-rust: test_chip_odm_map_rust
	./test_chip_odm_map_rust hal_intf_chip_odm_vectors.json

test_chip_odm_map_rust: test_chip_odm_map.c librust_hal_intf_chip_odm.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_INTF_CHIP_ODM_ORACLE -o $@ test_chip_odm_map.c \
		$(HOST_JSON) librust_hal_intf_chip_odm.a

librust_hal_intf_chip_rftype.a: $(RUST_HAL_INTF)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_intf_chip_rftype_test \
		--crate-type staticlib $(RUST_HAL_INTF) -o $@

test-chip-rftype-rust: test_chip_rftype_rust
	./test_chip_rftype_rust hal_intf_chip_rftype_vectors.json

test_chip_rftype_rust: test_chip_rftype.c librust_hal_intf_chip_rftype.a \
		$(CHIP_RFTYPE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_INTF_CHIP_RFTYPE_ORACLE -o $@ test_chip_rftype.c \
		$(CHIP_RFTYPE_ORACLE) $(HOST_JSON) librust_hal_intf_chip_rftype.a

librust_hal_intf_trxnss.a: $(RUST_HAL_INTF)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_intf_trxnss_test \
		--crate-type staticlib $(RUST_HAL_INTF) -o $@

test-hal-trxnss: test_hal_trxnss
	./test_hal_trxnss hal_intf_trxnss_vectors.json

test_hal_trxnss: test_hal_trxnss.c $(TRXNSS_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_trxnss.c $(TRXNSS_ORACLE) $(HOST_JSON)

test-hal-trxnss-rust: test_hal_trxnss_rust
	./test_hal_trxnss_rust hal_intf_trxnss_vectors.json

test_hal_trxnss_rust: test_hal_trxnss.c librust_hal_intf_trxnss.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_INTF_TRXNSS_ORACLE -o $@ test_hal_trxnss.c \
		$(HOST_JSON) librust_hal_intf_trxnss.a

librust_hal_intf_rfpath.a: $(RUST_HAL_INTF)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_intf_rfpath_test \
		--crate-type staticlib $(RUST_HAL_INTF) -o $@

$(RUST_RF_REST):
	$(MAKE) -C ../rf librust_rf_rest.a

test-hal-rfpath: test_hal_rfpath
	./test_hal_rfpath hal_intf_rfpath_vectors.json

test_hal_rfpath: test_hal_rfpath.c $(RFPATH_ORACLE) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(RFPATH_CFLAGS) -o $@ test_hal_rfpath.c $(RFPATH_ORACLE) \
		$(RF_REST_SRC) $(HOST_JSON)

test-hal-rfpath-rust: test_hal_rfpath_rust
	./test_hal_rfpath_rust hal_intf_rfpath_vectors.json

test_hal_rfpath_rust: test_hal_rfpath.c librust_hal_intf_rfpath.a $(RUST_RF_REST) \
		$(RF_REST_SHIMS) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_INTF_RFPATH_ORACLE -o $@ test_hal_rfpath.c \
		$(RF_REST_SHIMS) $(HOST_JSON) librust_hal_intf_rfpath.a $(RUST_RF_REST)

test-usb-sync-write: test_hal_usb_sync_write
	./test_hal_usb_sync_write hal_usb_sync_write_vectors.json

test_hal_usb_sync_write: test_hal_usb_sync_write.c $(USB_SYNC_WRITE_ORACLE) $(USB_WVALUE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DCONFIG_RTL8710B -o $@ test_hal_usb_sync_write.c \
		$(USB_SYNC_WRITE_ORACLE) $(USB_WVALUE_ORACLE) $(HOST_JSON)

test-usb-async-write: test_hal_usb_async_write
	./test_hal_usb_async_write hal_usb_async_write_vectors.json

test_hal_usb_async_write: test_hal_usb_async_write.c $(USB_ASYNC_WRITE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DCONFIG_USB_SUPPORT_ASYNC_VDN_REQ -o $@ \
		test_hal_usb_async_write.c $(USB_ASYNC_WRITE_ORACLE) $(HOST_JSON)

librust_hal_usb_async_write.a: $(RUST_HAL_USB)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_usb_async_write_test \
		--crate-type staticlib $(RUST_HAL_USB) -o $@

test-usb-async-write-rust: test_hal_usb_async_write_rust
	./test_hal_usb_async_write_rust hal_usb_async_write_vectors.json

test_hal_usb_async_write_rust: test_hal_usb_async_write.c librust_hal_usb_async_write.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_HAL_USB_ASYNC_WRITE_ORACLE \
		-DCONFIG_USB_SUPPORT_ASYNC_VDN_REQ -o $@ test_hal_usb_async_write.c \
		$(HOST_JSON) librust_hal_usb_async_write.a

test-sec-read-cam-ent: test_sec_read_cam_ent_null_mac
	./test_sec_read_cam_ent_null_mac

test_sec_read_cam_ent_null_mac: test_sec_read_cam_ent_null_mac.c $(SEC_CAM_ENT_L2)
	$(CC) $(CFLAGS) -o $@ test_sec_read_cam_ent_null_mac.c $(SEC_CAM_ENT_L2)

librust_hal_com_sec_cam_ent.a: $(RUST_SEC_CAM_ENT)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_SEC_CAM_ENT) -o $@

test-sec-read-cam-ent-rust: test_sec_read_cam_ent_null_mac_rust
	./test_sec_read_cam_ent_null_mac_rust

test_sec_read_cam_ent_null_mac_rust: test_sec_read_cam_ent_null_mac.c \
		$(SEC_CAM_ENT_L2) librust_hal_com_sec_cam_ent.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_SEC_CAM_ENT_ORACLE -o $@ \
		test_sec_read_cam_ent_null_mac.c $(SEC_CAM_ENT_L2) librust_hal_com_sec_cam_ent.a

MBID_CAM_SHIM := hal_com_mbid_cam_shim.c
MBID_CAM_ORACLE := hal_com_mbid_cam_c_oracle.c

test-mbid-cam: test_hal_com_mbid_cam
	./test_hal_com_mbid_cam

test_hal_com_mbid_cam: test_hal_com_mbid_cam.c $(MBID_CAM_SHIM) $(MBID_CAM_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_hal_com_mbid_cam.c $(MBID_CAM_SHIM) $(MBID_CAM_ORACLE)

RUST_MBID_CAM := $(ROOT)/rust/hal_com_mbid_cam_host.rs

librust_hal_com_mbid_cam.a: $(RUST_MBID_CAM)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_mbid_cam_test --crate-type staticlib \
		$(RUST_MBID_CAM) -o $@

test-mbid-cam-rust: test_hal_com_mbid_cam_rust
	./test_hal_com_mbid_cam_rust
test_hal_com_mbid_cam_rust: test_hal_com_mbid_cam.c $(MBID_CAM_SHIM) $(MBID_CAM_ORACLE) \
		librust_hal_com_mbid_cam.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_MBID_CAM_ORACLE -o $@ test_hal_com_mbid_cam.c \
		$(MBID_CAM_SHIM) $(MBID_CAM_ORACLE) librust_hal_com_mbid_cam.a

test-mbid-cam-alloc: test_hal_com_mbid_cam_alloc
	./test_hal_com_mbid_cam_alloc

test-mbid-cam-alloc-rust: test_hal_com_mbid_cam_alloc_rust
	./test_hal_com_mbid_cam_alloc_rust

test_hal_com_mbid_cam_alloc: test_hal_com_mbid_cam_alloc.c $(MBID_CAM_SHIM) $(MBID_CAM_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_hal_com_mbid_cam_alloc.c $(MBID_CAM_SHIM) $(MBID_CAM_ORACLE)

test_hal_com_mbid_cam_alloc_rust: test_hal_com_mbid_cam_alloc.c $(MBID_CAM_SHIM) \
		$(MBID_CAM_ORACLE) librust_hal_com_mbid_cam.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_MBID_CAM_ORACLE -o $@ test_hal_com_mbid_cam_alloc.c \
		$(MBID_CAM_SHIM) $(MBID_CAM_ORACLE) librust_hal_com_mbid_cam.a

RCR_SHIM := hal_com_rcr_shim.c
RCR_ORACLE := hal_com_rcr_c_oracle.c

test-rcr-leaf: test_hal_com_rcr
	./test_hal_com_rcr

test_hal_com_rcr: test_hal_com_rcr.c $(RCR_SHIM) $(RCR_ORACLE)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_rcr.c $(RCR_SHIM) $(RCR_ORACLE)

test-mbid-cam-hw: test_hal_com_mbid_cam_hw
	./test_hal_com_mbid_cam_hw

test_hal_com_mbid_cam_hw: test_hal_com_mbid_cam_hw.c $(MBID_CAM_SHIM) $(MBID_CAM_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_hal_com_mbid_cam_hw.c $(MBID_CAM_SHIM) $(MBID_CAM_ORACLE)

test-mbid-cam-hw-rust: test_hal_com_mbid_cam_hw_rust
	./test_hal_com_mbid_cam_hw_rust

test_hal_com_mbid_cam_hw_rust: test_hal_com_mbid_cam_hw.c $(MBID_CAM_SHIM) \
		$(MBID_CAM_ORACLE) librust_hal_com_mbid_cam.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_MBID_CAM_ORACLE -o $@ test_hal_com_mbid_cam_hw.c \
		$(MBID_CAM_SHIM) $(MBID_CAM_ORACLE) librust_hal_com_mbid_cam.a

include hal_com_rcr_rust.mk

RCR_CHK_BSSID_CFLAGS := $(CFLAGS) -DCONFIG_AP_MODE -DCONFIG_RTW_MESH -DCONFIG_TDLS \
	-DCONFIG_FIND_BEST_CHANNEL -DCONFIG_MI_WITH_MBSSID_CAM -DCONFIG_CLIENT_PORT_CFG \
	-DCONFIG_RTW_MULTI_AP
RCR_CHK_BSSID_SHIM := hal_com_rcr_shim.c hal_com_rcr_chk_bssid_ext.c
RCR_CHK_BSSID_ORACLE := hal_com_rcr_chk_bssid_c_oracle.c

test-rcr-chk-bssid: test_hal_com_rcr_chk_bssid
	./test_hal_com_rcr_chk_bssid

test_hal_com_rcr_chk_bssid: test_hal_com_rcr_chk_bssid.c $(RCR_CHK_BSSID_SHIM) $(RCR_CHK_BSSID_ORACLE)
	$(CC) $(RCR_CHK_BSSID_CFLAGS) -I../include -o $@ test_hal_com_rcr_chk_bssid.c \
		$(RCR_CHK_BSSID_SHIM) $(RCR_CHK_BSSID_ORACLE)

# Rust chk_bssid L2 is wired in PR4 (hal_com_rcr_chk_bssid_rust.mk).

test-mbid-cam-macaddr: test_hal_com_mbid_cam_macaddr
	./test_hal_com_mbid_cam_macaddr

test_hal_com_mbid_cam_macaddr: test_hal_com_mbid_cam_macaddr.c $(MBID_CAM_SHIM) \
		$(MBID_CAM_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_hal_com_mbid_cam_macaddr.c \
		$(MBID_CAM_SHIM) $(MBID_CAM_ORACLE)

test-mbid-cam-macaddr-rust: test_hal_com_mbid_cam_macaddr_rust
	./test_hal_com_mbid_cam_macaddr_rust

test_hal_com_mbid_cam_macaddr_rust: test_hal_com_mbid_cam_macaddr.c $(MBID_CAM_SHIM) \
		$(MBID_CAM_ORACLE) librust_hal_com_mbid_cam.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_MBID_CAM_ORACLE -o $@ test_hal_com_mbid_cam_macaddr.c \
		$(MBID_CAM_SHIM) $(MBID_CAM_ORACLE) librust_hal_com_mbid_cam.a

HALMAC_IDENTITY_SUPPORT := hal_com_halmac_identity_support.c

test-halmac-identity: test_hal_com_halmac_identity
	./test_hal_com_halmac_identity

test_hal_com_halmac_identity: test_hal_com_halmac_identity.c $(HALMAC_IDENTITY_SUPPORT)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_halmac_identity.c \
		$(HALMAC_IDENTITY_SUPPORT)

RUST_HALMAC_IDENTITY := $(ROOT)/rust/hal_com_halmac_identity_host.rs

librust_hal_com_halmac_identity.a: $(RUST_HALMAC_IDENTITY)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_halmac_identity_test --cfg config_rtw_halmac \
		--crate-type staticlib $(RUST_HALMAC_IDENTITY) -o $@

test_hal_com_halmac_identity_rust: test_hal_com_halmac_identity.c $(HALMAC_IDENTITY_SUPPORT) \
		librust_hal_com_halmac_identity.a
	$(CC) $(CFLAGS) -I../include -DRUST_HAL_COM_HALMAC_IDENTITY_ORACLE -o $@ \
		test_hal_com_halmac_identity.c $(HALMAC_IDENTITY_SUPPORT) \
		librust_hal_com_halmac_identity.a

test-halmac-identity-rust: test_hal_com_halmac_identity_rust
	./test_hal_com_halmac_identity_rust

RCR_AM_SHIM := hal_com_rcr_am_shim.c
RCR_AM_ORACLE := hal_com_rcr_am_c_oracle.c

test-rcr-am-hwvar: test_hal_com_rcr_am
	./test_hal_com_rcr_am

test_hal_com_rcr_am: test_hal_com_rcr_am.c $(RCR_AM_SHIM) $(RCR_AM_ORACLE)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_rcr_am.c $(RCR_AM_SHIM) $(RCR_AM_ORACLE)

include hal_com_rcr_am_rust.mk

GPIO_WAKEUP_SHIM := hal_com_gpio_wakeup_shim.c
GPIO_WAKEUP_ORACLE := hal_com_gpio_wakeup_c_oracle.c

test-gpio-wakeup: test_hal_com_gpio_wakeup
	./test_hal_com_gpio_wakeup

test_hal_com_gpio_wakeup: test_hal_com_gpio_wakeup.c $(GPIO_WAKEUP_SHIM) $(GPIO_WAKEUP_ORACLE)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_gpio_wakeup.c \
		$(GPIO_WAKEUP_SHIM) $(GPIO_WAKEUP_ORACLE)

GPIO_WAKEUP_8821AS_CFLAGS := $(CFLAGS) -DCONFIG_RTL8821A -DCONFIG_SDIO_HCI

test-gpio-wakeup-8821asdio: test_hal_com_gpio_wakeup_8821asdio
	./test_hal_com_gpio_wakeup_8821asdio

test_hal_com_gpio_wakeup_8821asdio: test_hal_com_gpio_wakeup.c $(GPIO_WAKEUP_SHIM) \
		$(GPIO_WAKEUP_ORACLE)
	$(CC) $(GPIO_WAKEUP_8821AS_CFLAGS) -I../include -o $@ test_hal_com_gpio_wakeup.c \
		$(GPIO_WAKEUP_SHIM) $(GPIO_WAKEUP_ORACLE)

RUST_GPIO_WAKEUP := $(ROOT)/rust/hal_com_gpio_wakeup_host.rs

librust_hal_com_gpio_wakeup.a: $(RUST_GPIO_WAKEUP)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_gpio_wakeup_test --crate-type staticlib \
		$(RUST_GPIO_WAKEUP) -o $@

librust_hal_com_gpio_wakeup_8821as.a: $(RUST_GPIO_WAKEUP)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_gpio_wakeup_test --cfg config_rtl8821a --cfg config_sdio_hci \
		--crate-type staticlib $(RUST_GPIO_WAKEUP) -o $@

test-gpio-wakeup-rust: test_hal_com_gpio_wakeup_rust
	./test_hal_com_gpio_wakeup_rust

test-gpio-wakeup-rust-8821asdio: test_hal_com_gpio_wakeup_rust_8821asdio
	./test_hal_com_gpio_wakeup_rust_8821asdio

test_hal_com_gpio_wakeup_rust: test_hal_com_gpio_wakeup.c $(GPIO_WAKEUP_SHIM) $(GPIO_WAKEUP_ORACLE) \
		librust_hal_com_gpio_wakeup.a
	$(CC) $(CFLAGS) -I../include -DRUST_HAL_COM_GPIO_WAKEUP_ORACLE -o $@ \
		test_hal_com_gpio_wakeup.c $(GPIO_WAKEUP_SHIM) $(GPIO_WAKEUP_ORACLE) \
		librust_hal_com_gpio_wakeup.a

test_hal_com_gpio_wakeup_rust_8821asdio: test_hal_com_gpio_wakeup.c $(GPIO_WAKEUP_SHIM) \
		$(GPIO_WAKEUP_ORACLE) librust_hal_com_gpio_wakeup_8821as.a
	$(CC) $(GPIO_WAKEUP_8821AS_CFLAGS) -I../include -DRUST_HAL_COM_GPIO_WAKEUP_ORACLE -o $@ \
		test_hal_com_gpio_wakeup.c $(GPIO_WAKEUP_SHIM) $(GPIO_WAKEUP_ORACLE) \
		librust_hal_com_gpio_wakeup_8821as.a

TSF_IFACE_SHIM := hal_com_tsf_shim.c
TSF_IFACE_ORACLE := hal_com_tsf_c_oracle.c
TSF_IFACE_PROD := hal_com_tsf_prod.c
TSF_IFACE_MBSSID_CFLAGS := $(CFLAGS) -DCONFIG_MI_WITH_MBSSID_CAM

test-tsf-iface: test_hal_com_tsf_iface
	./test_hal_com_tsf_iface

test-tsf-iface-mbssid: test_hal_com_tsf_iface_mbssid
	./test_hal_com_tsf_iface_mbssid

test_hal_com_tsf_iface: test_hal_com_tsf_iface.c $(TSF_IFACE_SHIM) \
		$(TSF_IFACE_ORACLE) $(TSF_IFACE_PROD)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_tsf_iface.c \
		$(TSF_IFACE_SHIM) $(TSF_IFACE_ORACLE) $(TSF_IFACE_PROD)

test_hal_com_tsf_iface_mbssid: test_hal_com_tsf_iface.c $(TSF_IFACE_SHIM) \
		$(TSF_IFACE_ORACLE) $(TSF_IFACE_PROD)
	$(CC) $(TSF_IFACE_MBSSID_CFLAGS) -I../include -o $@ test_hal_com_tsf_iface.c \
		$(TSF_IFACE_SHIM) $(TSF_IFACE_ORACLE) $(TSF_IFACE_PROD)

RUST_TSF_IFACE := $(ROOT)/rust/hal_com_tsf_host.rs

librust_hal_com_tsf.a: $(RUST_TSF_IFACE)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_tsf_test --crate-type staticlib \
		$(RUST_TSF_IFACE) -o $@

librust_hal_com_tsf_mbssid.a: $(RUST_TSF_IFACE)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_tsf_test --cfg config_mi_with_mbssid_cam \
		--crate-type staticlib $(RUST_TSF_IFACE) -o $@

test-tsf-iface-rust: test_hal_com_tsf_iface_rust
	./test_hal_com_tsf_iface_rust

test-tsf-iface-rust-mbssid: test_hal_com_tsf_iface_rust_mbssid
	./test_hal_com_tsf_iface_rust_mbssid

test_hal_com_tsf_iface_rust: test_hal_com_tsf_iface.c $(TSF_IFACE_SHIM) \
		$(TSF_IFACE_ORACLE) librust_hal_com_tsf.a
	$(CC) $(CFLAGS) -I../include -DRUST_HAL_COM_TSF_ORACLE -o $@ \
		test_hal_com_tsf_iface.c $(TSF_IFACE_SHIM) $(TSF_IFACE_ORACLE) \
		librust_hal_com_tsf.a

test_hal_com_tsf_iface_rust_mbssid: test_hal_com_tsf_iface.c $(TSF_IFACE_SHIM) \
		$(TSF_IFACE_ORACLE) librust_hal_com_tsf_mbssid.a
	$(CC) $(TSF_IFACE_MBSSID_CFLAGS) -I../include -DRUST_HAL_COM_TSF_ORACLE -o $@ \
		test_hal_com_tsf_iface.c $(TSF_IFACE_SHIM) $(TSF_IFACE_ORACLE) \
		librust_hal_com_tsf_mbssid.a

TSF_PERIODIC_HOST := hal_com_tsf_periodic_host.c
TSF_PERIODIC_ORACLE := hal_com_tsf_periodic_c_oracle.c
TSF_PERIODIC_TIMER_PROD := hal_com_tsf_periodic_timer_prod.c
TSF_PERIODIC_CHK_PROD := hal_com_tsf_periodic_chk_prod.c

test-tsf-periodic: test_hal_com_tsf_periodic
	./test_hal_com_tsf_periodic

test-tsf-periodic-smoke: test_hal_com_tsf_periodic_smoke
	./test_hal_com_tsf_periodic_smoke

test_hal_com_tsf_periodic_smoke: test_hal_com_tsf_periodic_smoke.c $(TSF_PERIODIC_HOST) \
		$(TSF_PERIODIC_TIMER_PROD)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_tsf_periodic_smoke.c \
		$(TSF_PERIODIC_HOST) $(TSF_PERIODIC_TIMER_PROD)

test_hal_com_tsf_periodic: test_hal_com_tsf_periodic.c $(TSF_PERIODIC_HOST) \
		$(TSF_PERIODIC_CHK_PROD) $(TSF_PERIODIC_TIMER_PROD) \
		$(TSF_PERIODIC_ORACLE)
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_tsf_periodic.c \
		$(TSF_PERIODIC_HOST) $(TSF_PERIODIC_CHK_PROD) $(TSF_PERIODIC_TIMER_PROD) \
		$(TSF_PERIODIC_ORACLE)

test-tsf-periodic-rust: test_hal_com_tsf_periodic_rust
	./test_hal_com_tsf_periodic_rust

RUST_TSF_PERIODIC := $(ROOT)/rust/hal_com_tsf_periodic_host.rs

librust_hal_com_tsf_periodic.a: $(RUST_TSF_PERIODIC)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_tsf_test --crate-type staticlib \
		$(RUST_TSF_PERIODIC) -o $@

test_hal_com_tsf_periodic_rust: test_hal_com_tsf_periodic.c $(TSF_PERIODIC_HOST) \
		$(TSF_PERIODIC_ORACLE) librust_hal_com_tsf_periodic.a
	$(CC) $(CFLAGS) -I../include -DRUST_HAL_COM_TSF_PERIODIC_ORACLE -o $@ \
		test_hal_com_tsf_periodic.c $(TSF_PERIODIC_HOST) $(TSF_PERIODIC_ORACLE) \
		librust_hal_com_tsf_periodic.a

RUST_C2H_EVT := $(ROOT)/rust/hal_com_c2h_evt.rs
CUSTOMER_STR_SHIM := hal_com_customer_str_shim.c
CUSTOMER_STR_ORACLE := hal_com_customer_str_c_oracle.c
RUST_CUSTOMER_STR := $(ROOT)/rust/hal_com_customer_str_host.rs

test-customer-str: test_hal_com_customer_str
	./test_hal_com_customer_str

test-customer-str-rust: test_hal_com_customer_str_rust
	./test_hal_com_customer_str_rust

test_hal_com_customer_str: test_hal_com_customer_str.c $(CUSTOMER_STR_SHIM) $(CUSTOMER_STR_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_hal_com_customer_str.c $(CUSTOMER_STR_SHIM) \
		$(CUSTOMER_STR_ORACLE)

librust_hal_com_customer_str.a: $(RUST_CUSTOMER_STR)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_CUSTOMER_STR) -o $@

test_hal_com_customer_str_rust: test_hal_com_customer_str.c $(CUSTOMER_STR_SHIM) \
		$(CUSTOMER_STR_ORACLE) librust_hal_com_customer_str.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_CUSTOMER_STR_ORACLE -o $@ test_hal_com_customer_str.c \
		$(CUSTOMER_STR_SHIM) $(CUSTOMER_STR_ORACLE) librust_hal_com_customer_str.a

REQ_PER_RPT_SHIM := hal_com_req_per_rpt_shim.c
REQ_PER_RPT_ORACLE := hal_com_req_per_rpt_c_oracle.c
RUST_REQ_PER_RPT := $(ROOT)/rust/hal_com_req_per_rpt_host.rs
REQ_PER_RPT_CFLAGS := $(CFLAGS) -DRTW_PER_CMD_SUPPORT_FW

test-req-per-rpt: test_hal_com_req_per_rpt
	./test_hal_com_req_per_rpt

test-req-per-rpt-rust: test_hal_com_req_per_rpt_rust
	./test_hal_com_req_per_rpt_rust

test_hal_com_req_per_rpt: test_hal_com_req_per_rpt.c $(REQ_PER_RPT_SHIM) $(REQ_PER_RPT_ORACLE)
	$(CC) $(REQ_PER_RPT_CFLAGS) -o $@ test_hal_com_req_per_rpt.c \
		$(REQ_PER_RPT_SHIM) $(REQ_PER_RPT_ORACLE)

librust_hal_com_req_per_rpt.a: $(RUST_REQ_PER_RPT)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg rtw_per_cmd_support_fw \
		--crate-type staticlib $(RUST_REQ_PER_RPT) -o $@

test_hal_com_req_per_rpt_rust: test_hal_com_req_per_rpt.c $(REQ_PER_RPT_SHIM) \
		$(REQ_PER_RPT_ORACLE) librust_hal_com_req_per_rpt.a
	$(CC) $(REQ_PER_RPT_CFLAGS) -DRUST_HAL_COM_REQ_PER_RPT_ORACLE -o $@ \
		test_hal_com_req_per_rpt.c $(REQ_PER_RPT_SHIM) $(REQ_PER_RPT_ORACLE) \
		librust_hal_com_req_per_rpt.a

DFT_PORT_SHIM := hal_com_dft_port_shim.c
DFT_PORT_ORACLE := hal_com_dft_port_c_oracle.c
RUST_DFT_PORT := $(ROOT)/rust/hal_com_dft_port_host.rs

test-dft-port: test_hal_com_dft_port
	./test_hal_com_dft_port

test-dft-port-rust: test_hal_com_dft_port_rust
	./test_hal_com_dft_port_rust

test_hal_com_dft_port: test_hal_com_dft_port.c $(DFT_PORT_SHIM) $(DFT_PORT_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_hal_com_dft_port.c $(DFT_PORT_SHIM) $(DFT_PORT_ORACLE)

ifneq ($(wildcard $(RUST_DFT_PORT)),)
librust_hal_com_dft_port.a: $(RUST_DFT_PORT)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg config_fw_multi_port_support \
		--crate-type staticlib $(RUST_DFT_PORT) -o $@

test_hal_com_dft_port_rust: test_hal_com_dft_port.c $(DFT_PORT_SHIM) \
		$(DFT_PORT_ORACLE) librust_hal_com_dft_port.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_DFT_PORT_ORACLE -o $@ \
		test_hal_com_dft_port.c $(DFT_PORT_SHIM) $(DFT_PORT_ORACLE) \
		librust_hal_com_dft_port.a
endif

test-fw-rsvdpage-h2c: test_hal_com_fw_rsvdpage_h2c
	./test_hal_com_fw_rsvdpage_h2c

test_hal_com_fw_rsvdpage_h2c: test_hal_com_fw_rsvdpage_h2c.c hal_com_fw_rsvdpage_h2c_shim.c \
		hal_com_fw_rsvdpage_h2c_c_oracle.c
	$(CC) $(CFLAGS) -o $@ test_hal_com_fw_rsvdpage_h2c.c hal_com_fw_rsvdpage_h2c_shim.c \
		hal_com_fw_rsvdpage_h2c_c_oracle.c

RUST_FW_RSVDPAGE_H2C := $(ROOT)/rust/hal_com_fw_rsvdpage_h2c_host.rs

librust_hal_com_fw_rsvdpage_h2c.a: $(RUST_FW_RSVDPAGE_H2C)
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--cfg host_hal_com_fw_rsvdpage_h2c_test \
		--crate-type staticlib $(RUST_FW_RSVDPAGE_H2C) -o $@

test-fw-rsvdpage-h2c-rust: test_hal_com_fw_rsvdpage_h2c_rust
	./test_hal_com_fw_rsvdpage_h2c_rust

test_hal_com_fw_rsvdpage_h2c_rust: test_hal_com_fw_rsvdpage_h2c.c \
		hal_com_fw_rsvdpage_h2c_shim.c hal_com_fw_rsvdpage_h2c_c_oracle.c \
		librust_hal_com_fw_rsvdpage_h2c.a
	$(CC) $(CFLAGS) -DRUST_HAL_COM_FW_RSVDPAGE_H2C_ORACLE -o $@ \
		test_hal_com_fw_rsvdpage_h2c.c hal_com_fw_rsvdpage_h2c_shim.c \
		hal_com_fw_rsvdpage_h2c_c_oracle.c librust_hal_com_fw_rsvdpage_h2c.a

test-c2h-evt-robustness: test_hal_com_c2h_evt_fx19
	./test_hal_com_c2h_evt_fx19

test_hal_com_c2h_evt_fx19: test_hal_com_c2h_evt_fx19.c librust_hal_com_c2h_evt.a
	$(CC) $(CFLAGS) -o $@ test_hal_com_c2h_evt_fx19.c librust_hal_com_c2h_evt.a

librust_hal_com_c2h_evt.a: $(RUST_C2H_EVT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_c2h_evt_test \
		--crate-type staticlib $(RUST_C2H_EVT) -o $@

clean:
	rm -f test_hw_rate_map test_hw_rate_map_rust librust_hal_com.a \
		test_rsvd_page_cache test_rsvd_page_cache_rust librust_hal_com_rsvd.a \
		test_dump_chip_info test_dump_chip_info_rust librust_hal_com_chip.a \
		test_dump_trx_mode test_dump_trx_mode_rust librust_hal_intf_trx.a \
		test_dump_fw_info test_dump_fw_info_rust librust_hal_com_fw.a \
		test_dump_tx_aclt_force_val test_dump_tx_aclt_force_val_rust librust_hal_com_tx_aclt_force.a \
		test_dump_tx_aclt_confs test_dump_tx_aclt_confs_rust librust_hal_com_tx_aclt_confs.a \
		test_dump_hal_spec test_dump_hal_spec_rust librust_hal_com_hal_spec.a \
		test_m_rate_to_hw_rate test_m_rate_to_hw_rate_rust librust_hal_com_mrate.a \
		test_dump_rf_nss test_dump_rf_nss_rust librust_hal_intf_rf_nss.a \
		test_hal_halmac_network test_hal_halmac_bcn test_hal_halmac_bcn_abi \
		test_hal_halmac_rsvd test_hal_halmac_trx_share test_hal_halmac_drv2halmac \
		test_hal_halmac_rf_type test_hal_halmac_usb_fifo \
		test_hal_halmac_phy_param_drv2halmac test_hal_halmac_version \
		test_hal_halmac_network_rust test_hal_halmac_bcn_rust \
		test_hal_halmac_bcn_abi_rust test_hal_halmac_rsvd_rust \
		test_hal_halmac_trx_share_rust test_hal_halmac_drv2halmac_rust \
		test_hal_halmac_rf_type_rust test_hal_halmac_usb_fifo_rust \
		test_hal_halmac_phy_param_drv2halmac_rust test_hal_halmac_version_rust \
		librust_hal_halmac.a librust_hal_halmac_with_version.a \
		test_hal_set_brate_cfg test_hal_set_brate_cfg_rust librust_hal_com_brate.a \
		test_hal_is_legal_channel test_hal_is_legal_channel_rust librust_hal_com_legal.a \
		test_hal_mapping_out_pipe test_hal_mapping_out_pipe_rust librust_hal_com_mapping.a \
		test_hal_mgntframe_raid test_hal_mgntframe_raid_rust librust_hal_com_mgntframe_raid.a \
		test_hal_update_sta_wset_mimo test_hal_update_sta_wset_mimo_rust \
		librust_hal_com_update_sta_wset_mimo.a \
		test_hal_update_sta_smps_rate test_hal_update_sta_smps_rate_rust \
		test_hal_update_sta_smps_rate_rust_kern \
		librust_hal_com_update_sta_smps_rate.a librust_hal_com_update_sta_smps_rate_kern.a \
		test_hal_com_update_sta_ra_info test_hal_com_update_sta_ra_info_rust \
		librust_hal_com_update_sta_ra_info.a \
		test_hal_hdata_rate test_hal_hdata_rate_rust librust_hal_com_hdata_rate.a \
		test_hal_phy_rf_max_reg test_hal_phy_rf_max_reg_rust librust_hal_phy_rf_max_reg.a \
		test_hal_phy_bit_shift test_hal_phy_bit_shift_rust librust_hal_phy.a \
		test_hal_phy_rf_shadow test_hal_phy_rf_shadow_rust librust_hal_phy_rf_shadow.a \
		test_hal_phy_rf_shadow_flags test_hal_phy_rf_shadow_flags_rust \
		test_hal_phy_rf_shadow_compare test_hal_phy_rf_shadow_compare_rust \
		librust_hal_phy_rf_shadow_compare.a \
		test_hal_dm_board_type test_hal_dm_board_type_rust librust_hal_dm.a \
		test_hal_dm_odm_endian_hooks test_hal_dm_odm_endian_hooks_rust \
		librust_hal_dm_odm_endian.a \
		test_hal_dm_dump_sta_info test_hal_dm_dump_sta_info_rust \
		librust_hal_dm_dump_sta.a \
		test_hal_dm_dump_sta_traffic test_hal_dm_dump_sta_traffic_rust \
		librust_hal_dm_dump_sta_traffic.a \
		test_hal_dm_odm_iface test_hal_dm_odm_iface_rust librust_hal_dm_odm_iface.a \
		test_hal_dm_phydm_igi_iqk test_hal_dm_phydm_igi_iqk_rust \
		librust_hal_dm_phydm_igi_iqk.a \
		test_hal_dm_set_odm_var_core test_hal_dm_set_odm_var_core_rust \
		librust_hal_dm_set_odm_var_core.a \
		test_hal_dm_phydm_iqk_lck test_hal_dm_phydm_iqk_lck_rust \
		librust_hal_dm_phydm_iqk_lck.a \
		test_hal_dm_set_odm_var_rx \
		test_hal_dm_dyntxpwr test_hal_dm_dyntxpwr_rust librust_hal_dm_dyntxpwr.a \
		test_hal_dm_phydm_cal_trigger test_hal_dm_phydm_cal_trigger_rust \
		librust_hal_dm_phydm_cal_trigger.a \
		test_hal_dm_dbg_rf_cal_test test_hal_dm_dbg_rf_cal_test_rust \
		librust_hal_dm_dbg_rf_cal_test.a \
		test_hal_dm_min_rssi_phy_cnt test_hal_dm_min_rssi_phy_cnt_rust \
		librust_hal_dm_min_rssi_phy_cnt.a \
		test_hal_usb_vendor_wvalue test_hal_usb_vendor_wvalue_rust librust_hal_usb_wvalue.a \
		test_hal_usb_sync_read test_hal_usb_sync_write test_hal_usb_async_write \
		test_hal_usb_async_write_rust librust_hal_usb_async_write.a \
		test_chip_odm_map test_chip_odm_map_rust librust_hal_intf_chip_odm.a \
		test_chip_rftype_rust librust_hal_intf_chip_rftype.a \
		test_sec_read_cam_ent_null_mac test_sec_read_cam_ent_null_mac_rust \
		librust_hal_com_sec_cam_ent.a \
		test_hal_com_c2h_evt_fx19 librust_hal_com_c2h_evt.a \
		test_hal_com_req_per_rpt test_hal_com_req_per_rpt_rust \
		librust_hal_com_req_per_rpt.a \
		test_hal_com_dft_port $(if $(wildcard $(ROOT)/rust/hal_com_dft_port_host.rs),test_hal_com_dft_port_rust,) \
		$(if $(wildcard $(ROOT)/rust/hal_com_dft_port_host.rs),librust_hal_com_dft_port.a,) \
		test_hal_com_mbid_cam_rust librust_hal_com_mbid_cam.a \
		test_hal_com_rcr test_hal_com_rcr_rust librust_hal_com_rcr.a \
		test_hal_com_rcr_chk_bssid test_hal_com_rcr_chk_bssid_rust librust_hal_com_rcr_chk.a \
		test_hal_com_rcr_am test_hal_com_rcr_am_rust librust_hal_com_rcr_am.a \
		test_hal_com_gpio_wakeup test_hal_com_gpio_wakeup_rust \
		test_hal_com_gpio_wakeup_8821asdio test_hal_com_gpio_wakeup_rust_8821asdio \
		test_hal_com_halmac_identity test_hal_com_halmac_identity_rust \
		librust_hal_com_halmac_identity.a \
		librust_hal_com_gpio_wakeup.a librust_hal_com_gpio_wakeup_8821as.a \
		test_hal_com_tsf_iface test_hal_com_tsf_iface_mbssid \
		test_hal_com_tsf_iface_rust test_hal_com_tsf_iface_rust_mbssid \
		librust_hal_com_tsf.a librust_hal_com_tsf_mbssid.a \
		test_hal_com_tsf_periodic_smoke test_hal_com_tsf_periodic \
		test_hal_com_tsf_periodic_rust librust_hal_com_tsf_periodic.a \
		test_hal_com_tx_aclt_set test_hal_com_tx_aclt_set_rust \
		librust_hal_com_tx_aclt_set.a
