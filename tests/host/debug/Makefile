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

# W3-203 proc HT registry get/set
HT_ORACLE := debug_proc_ht_c_oracle.c
HT_SHIM   := host_rtw_debug_proc_ht_shim.c
HT_RUST   := $(ROOT)/rust/rtw_debug_proc_ht.rs
HT_L2     := test_proc_ht_registry.c

# W3-208 proc vht_24g_enable get/set
VHT_ORACLE := debug_proc_vht_c_oracle.c
VHT_SHIM   := host_rtw_debug_proc_vht_shim.c
VHT_RUST   := $(ROOT)/rust/rtw_debug_proc_vht.rs
VHT_L2     := test_proc_vht_24g.c

# W3-209 proc en_fwps (check_fw_ps) get/set
FWPS_ORACLE := debug_proc_fwps_c_oracle.c
FWPS_L2     := test_proc_en_fwps.c

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
# W3-212 proc TDLS enable get/set (+ function info in PR3)
TDLS_ORACLE := debug_proc_tdls_c_oracle.c
TDLS_DISPLAY_ORACLE := debug_proc_tdls_display_oracle.c
TDLS_L2     := test_proc_tdls_enable.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(ROOT)/include

.PHONY: all test test-all test-c test-rust clean \
	test-hooks-c test-hooks-rust test-sec-cam-c test-sec-cam-rust \
	test-regsty-rx-ampdu-c test-regsty-rx-ampdu-rust \
	test-proc-ht-c test-proc-ht-rust \
	test-proc-vht-c test-proc-vht-rust \
	test-proc-fwps-c test-proc-tdls-c \
	test-proc-rx-ampdu-c test-proc-rx-ampdu-rust \
	test-proc-ampdu-tuning-c test-proc-ampdu-tuning-rust \
	test-proc-tx-ampdu-misc-c test-proc-tx-ampdu-misc-rust \
	test-proc-tx-amsdu-c test-proc-tx-amsdu-rust

all: test

test: test-hooks-c test-hooks-rust test-sec-cam-c test-sec-cam-rust \
	test-regsty-rx-ampdu-c test-regsty-rx-ampdu-rust \
	test-proc-ht-c test-proc-ht-rust \
	test-proc-vht-c test-proc-vht-rust \
	test-proc-fwps-c test-proc-tdls-c \
	test-proc-rx-ampdu-c test-proc-rx-ampdu-rust \
	test-proc-ampdu-tuning-c test-proc-ampdu-tuning-rust \
	test-proc-tx-ampdu-misc-c test-proc-tx-ampdu-misc-rust \
	test-proc-tx-amsdu-c test-proc-tx-amsdu-rust

test-all: test

test-c: test-hooks-c test-sec-cam-c test-regsty-rx-ampdu-c test-proc-ht-c \
	test-proc-vht-c test-proc-fwps-c test-proc-rx-ampdu-c test-proc-tdls-c \
	test-proc-ampdu-tuning-c test-proc-tx-amsdu-c

test-rust: test-hooks-rust test-sec-cam-rust test-regsty-rx-ampdu-rust \
	test-proc-ht-rust test-proc-vht-rust test-proc-rx-ampdu-rust test-proc-ampdu-tuning-rust \
	test-proc-tx-ampdu-misc-rust test-proc-tx-amsdu-rust

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

test-proc-vht-c: test_proc_vht_24g
	./test_proc_vht_24g debug_proc_vht_vectors.json

test_proc_vht_24g: $(VHT_L2) $(VHT_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(VHT_L2) $(VHT_ORACLE) $(HOST_JSON)

test-proc-fwps-c: test_proc_en_fwps
	./test_proc_en_fwps debug_proc_fwps_vectors.json

test_proc_en_fwps: $(FWPS_L2) $(FWPS_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(FWPS_L2) $(FWPS_ORACLE) $(HOST_JSON)

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

test_proc_tdls_enable: $(TDLS_L2) $(TDLS_ORACLE) $(TDLS_DISPLAY_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(TDLS_L2) $(TDLS_ORACLE) $(TDLS_DISPLAY_ORACLE) $(HOST_JSON)

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

clean:
	rm -f test_debug_test_hooks_c test_debug_test_hooks_rust librust_debug_test_hooks.a \
		test_dump_sec_cam_ent test_dump_sec_cam_ent_rust librust_rtw_debug_sec_cam.a \
		test_dump_regsty_rx_ampdu test_dump_regsty_rx_ampdu_rust librust_rtw_debug_regsty.a \
		test_proc_ht_registry test_proc_ht_registry_rust librust_rtw_debug_proc_ht.a \
		test_proc_vht_24g test_proc_vht_24g_rust librust_rtw_debug_proc_vht.a \
		test_proc_en_fwps test_proc_tdls_enable \
		test_proc_rx_ampdu test_proc_rx_ampdu_rust librust_rtw_debug_proc_ampdu.a \
		test_proc_ampdu_tuning test_proc_ampdu_tuning_rust \
		test_proc_tx_ampdu_misc test_proc_tx_ampdu_misc_rust \
		test_proc_tx_amsdu test_proc_tx_amsdu_rust librust_rtw_debug_proc_amsdu.a
