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
MRATE_ORACLE := hal_mrate_to_hw_rate_c_oracle.c
RF_NSS_ORACLE := hal_dump_rf_nss_c_oracle.c
HALMAC_ORACLE := hal_halmac_enum_mappers_c_oracle.c
PHY_BIT_SHIFT_ORACLE := hal_phy_bit_shift_c_oracle.c
HALMAC_TRX_ORACLE := hal_halmac_trx_share_c_oracle.c
RUST_HAL := $(ROOT)/rust/hal_com.rs
RUST_HAL_PHY := $(ROOT)/rust/hal_phy.rs
RUST_HAL_INTF := $(ROOT)/rust/hal_intf.rs
RUST_HAL_HALMAC := $(ROOT)/rust/hal_halmac.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC)

.PHONY: all test test-c test-rust test-rsvd test-rsvd-rust test-chip test-chip-rust \
	test-trx test-trx-rust test-fw test-fw-rust test-mrate test-mrate-rust \
	test-rf-nss test-rf-nss-rust test-halmac test-halmac-rust test-halmac-rsvd \
	test-halmac-rsvd-rust test-phy-bit-shift test-phy-bit-shift-rust clean

all: test

test: test-c test-rust test-rsvd test-chip test-chip-rust test-trx test-trx-rust \
	test-fw test-fw-rust test-mrate test-mrate-rust test-rf-nss test-rf-nss-rust \
	test-halmac test-halmac-rust test-halmac-rsvd test-phy-bit-shift test-phy-bit-shift-rust

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

test-halmac: test_hal_halmac_network test_hal_halmac_bcn test_hal_halmac_bcn_abi \
	test_hal_halmac_rsvd test_hal_halmac_trx_share
	./test_hal_halmac_network hal_halmac_network_vectors.json
	./test_hal_halmac_bcn hal_halmac_bcn_vectors.json
	./test_hal_halmac_bcn_abi
	./test_hal_halmac_rsvd hal_halmac_rsvd_drv2halmac_vectors.json \
		hal_halmac_rsvd_halmac2drv_vectors.json
	./test_hal_halmac_trx_share hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json

test_hal_halmac_network: test_hal_halmac_network.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_network.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_bcn: test_hal_halmac_bcn.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_bcn.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_rsvd: test_hal_halmac_rsvd.c $(HALMAC_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_rsvd.c $(HALMAC_ORACLE) $(HOST_JSON)

test_hal_halmac_trx_share: test_hal_halmac_trx_share.c $(HALMAC_TRX_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_trx_share.c $(HALMAC_TRX_ORACLE) $(HOST_JSON)

test-halmac-trx-share: test_hal_halmac_trx_share
	./test_hal_halmac_trx_share hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json

test-halmac-trx-share-rust: test_hal_halmac_trx_share_rust
	./test_hal_halmac_trx_share_rust hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json

librust_hal_halmac.a: $(RUST_HAL_HALMAC)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_hal_halmac_enum_test \
		--crate-type staticlib $(RUST_HAL_HALMAC) -o $@

test_hal_halmac_bcn_abi: test_hal_halmac_bcn_abi.c
	$(CC) $(CFLAGS) -o $@ test_hal_halmac_bcn_abi.c

test_hal_halmac_bcn_abi_rust: test_hal_halmac_bcn_abi.c librust_hal_halmac.a
	$(CC) $(CFLAGS) -DRUST_HAL_HALMAC_ORACLE -o $@ test_hal_halmac_bcn_abi.c \
		librust_hal_halmac.a

test-halmac-rust: test_hal_halmac_network_rust test_hal_halmac_bcn_rust \
	test_hal_halmac_bcn_abi_rust test_hal_halmac_rsvd_rust test_hal_halmac_trx_share_rust
	./test_hal_halmac_network_rust hal_halmac_network_vectors.json
	./test_hal_halmac_bcn_rust hal_halmac_bcn_vectors.json
	./test_hal_halmac_bcn_abi_rust
	./test_hal_halmac_rsvd_rust hal_halmac_rsvd_drv2halmac_vectors.json \
		hal_halmac_rsvd_halmac2drv_vectors.json
	./test_hal_halmac_trx_share_rust hal_halmac_trx_share_drv2halmac_vectors.json \
		hal_halmac_trx_share_dump_vectors.json

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

clean:
	rm -f test_hw_rate_map test_hw_rate_map_rust librust_hal_com.a \
		test_rsvd_page_cache test_rsvd_page_cache_rust librust_hal_com_rsvd.a \
		test_dump_chip_info test_dump_chip_info_rust librust_hal_com_chip.a \
		test_dump_trx_mode test_dump_trx_mode_rust librust_hal_intf_trx.a \
		test_dump_fw_info test_dump_fw_info_rust librust_hal_com_fw.a \
		test_m_rate_to_hw_rate test_m_rate_to_hw_rate_rust librust_hal_com_mrate.a \
		test_dump_rf_nss test_dump_rf_nss_rust librust_hal_intf_rf_nss.a \
		test_hal_halmac_network test_hal_halmac_bcn test_hal_halmac_bcn_abi \
		test_hal_halmac_rsvd test_hal_halmac_trx_share \
		test_hal_halmac_network_rust test_hal_halmac_bcn_rust \
		test_hal_halmac_bcn_abi_rust test_hal_halmac_rsvd_rust \
		test_hal_halmac_trx_share_rust librust_hal_halmac.a \
		test_hal_phy_bit_shift test_hal_phy_bit_shift_rust librust_hal_phy.a
