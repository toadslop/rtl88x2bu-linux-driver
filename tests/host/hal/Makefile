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
RUST_HAL := $(ROOT)/rust/hal_com.rs
RUST_HAL_INTF := $(ROOT)/rust/hal_intf.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC)

.PHONY: all test test-c test-rust test-rsvd test-rsvd-rust test-chip test-chip-rust \
	test-trx test-trx-rust test-fw test-fw-rust test-mrate test-mrate-rust \
	test-rf-nss test-rf-nss-rust clean

all: test

test: test-c test-rust test-rsvd test-chip test-chip-rust test-trx test-trx-rust \
	test-fw test-mrate test-mrate-rust test-rf-nss test-rf-nss-rust

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

clean:
	rm -f test_hw_rate_map test_hw_rate_map_rust librust_hal_com.a \
		test_rsvd_page_cache test_rsvd_page_cache_rust librust_hal_com_rsvd.a \
		test_dump_chip_info test_dump_chip_info_rust librust_hal_com_chip.a \
		test_dump_trx_mode test_dump_trx_mode_rust librust_hal_intf_trx.a \
		test_dump_fw_info test_dump_fw_info_rust librust_hal_com_fw.a \
		test_m_rate_to_hw_rate test_m_rate_to_hw_rate_rust librust_hal_com_mrate.a \
		test_dump_rf_nss test_dump_rf_nss_rust librust_hal_intf_rf_nss.a
