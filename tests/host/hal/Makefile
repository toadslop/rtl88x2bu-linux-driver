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
DM_DUMP_STA_SHIM := hal_dm_dump_sta_info_shim.c
DM_DUMP_STA_TRAFFIC_SHIM := hal_dm_dump_sta_traffic_shim.c
USB_WVALUE_ORACLE := hal_usb_vendor_wvalue_c_oracle.c
USB_SYNC_READ_ORACLE := hal_usb_sync_read_c_oracle.c
RUST_HAL := $(ROOT)/rust/hal_com.rs
RUST_HAL_USB := $(ROOT)/rust/hal_usb.rs
RUST_HAL_DM := $(ROOT)/rust/hal_dm.rs
RUST_HAL_PHY := $(ROOT)/rust/hal_phy.rs
RUST_HAL_INTF := $(ROOT)/rust/hal_intf.rs
RUST_HAL_HALMAC := $(ROOT)/rust/hal_halmac.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC)

.PHONY: all test test-c test-rust test-rsvd test-rsvd-rust test-chip test-chip-rust \
	test-trx test-trx-rust test-fw test-fw-rust test-tx-aclt-force test-mrate test-mrate-rust \
	test-rf-nss test-rf-nss-rust test-halmac test-halmac-rust test-halmac-rsvd \
	test-halmac-rsvd-rust test-halmac-drv2halmac test-halmac-drv2halmac-rust \
	test-brate test-brate-rust \
	test-legal-channel test-legal-channel-rust \
	test-mapping-out-pipe test-mapping-out-pipe-rust test-mgntframe-raid test-mgntframe-raid-rust \
	test-hdata-rate test-hdata-rate-rust test-phy-bit-shift test-phy-bit-shift-rust \
	test-dm-board-type test-dm-board-type-rust \
	test-dm-dump-sta-traffic test-dm-dump-sta-traffic-rust \
	test-usb-vendor-wvalue test-usb-vendor-wvalue-rust \
	test-usb-sync-read test-usb-sync-read-rust clean

all: test

test: test-c test-rust test-rsvd test-chip test-chip-rust test-trx test-trx-rust \
	test-fw test-fw-rust test-tx-aclt-force test-mrate test-mrate-rust test-rf-nss test-rf-nss-rust \
	test-halmac test-halmac-rust test-halmac-rsvd test-brate test-brate-rust \
	test-legal-channel test-legal-channel-rust test-mapping-out-pipe test-mapping-out-pipe-rust \
	test-mgntframe-raid test-mgntframe-raid-rust test-hdata-rate test-hdata-rate-rust \
	test-phy-bit-shift test-phy-bit-shift-rust test-dm-board-type test-dm-board-type-rust \
	test-dm-dump-sta-traffic test-dm-dump-sta-traffic-rust \
	test-usb-vendor-wvalue test-usb-vendor-wvalue-rust test-usb-sync-read \
	test-usb-sync-read-rust

test-c: test_hw_rate_map
	./test_hw_rate_map hal_hw_rate_vectors.json

test_hw_rate_map: test_hw_rate_map.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hw_rate_map.c $(ORACLE) $(HOST_JSON)

librust_hal_com.a: $(RUST_HAL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_com_hw_rate_test \
		--crate-type staticlib $(RUST_HAL) -o $@

test-rust: test_hw_rate_map_rust
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

clean:
	rm -f test_hw_rate_map test_hw_rate_map_rust librust_hal_com.a \
		test_rsvd_page_cache test_rsvd_page_cache_rust librust_hal_com_rsvd.a \
		test_dump_chip_info test_dump_chip_info_rust librust_hal_com_chip.a \
		test_dump_trx_mode test_dump_trx_mode_rust librust_hal_intf_trx.a \
		test_dump_fw_info test_dump_fw_info_rust librust_hal_com_fw.a \
		test_dump_tx_aclt_force_val \
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
		test_hal_hdata_rate test_hal_hdata_rate_rust librust_hal_com_hdata_rate.a \
		test_hal_phy_rf_max_reg test_hal_phy_rf_max_reg_rust librust_hal_phy_rf_max_reg.a \
		test_hal_phy_bit_shift test_hal_phy_bit_shift_rust librust_hal_phy.a \
		test_hal_phy_rf_shadow test_hal_phy_rf_shadow_rust librust_hal_phy_rf_shadow.a \
		test_hal_phy_rf_shadow_flags test_hal_phy_rf_shadow_flags_rust \
		test_hal_phy_rf_shadow_compare test_hal_phy_rf_shadow_compare_rust \
		librust_hal_phy_rf_shadow_compare.a \
		test_hal_dm_board_type test_hal_dm_board_type_rust librust_hal_dm.a \
		test_hal_dm_dump_sta_info test_hal_dm_dump_sta_info_rust \
		librust_hal_dm_dump_sta.a \
		test_hal_dm_dump_sta_traffic test_hal_dm_dump_sta_traffic_rust \
		librust_hal_dm_dump_sta_traffic.a \
		test_hal_usb_vendor_wvalue test_hal_usb_vendor_wvalue_rust librust_hal_usb_wvalue.a \
		test_hal_usb_sync_read
