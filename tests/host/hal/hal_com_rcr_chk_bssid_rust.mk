# W4-128 PR4 — Rust L2 target for chk_bssid policy.

CHK_BSSID_RUSTFLAGS := --cfg host_hal_com_rcr_chk_bssid_test --cfg config_ap_mode \
	--cfg config_rtw_mesh --cfg config_tdls --cfg config_find_best_channel \
	--cfg config_mi_with_mbssid_cam --cfg config_client_port_cfg \
	--cfg config_rtw_multi_ap

librust_hal_com_rcr_chk.a: $(ROOT)/rust/hal_com_rcr_host.rs $(ROOT)/rust/hal_com_rcr.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on $(CHK_BSSID_RUSTFLAGS) \
		--crate-type staticlib $(ROOT)/rust/hal_com_rcr_host.rs -o $@

test-rcr-chk-bssid-rust: test_hal_com_rcr_chk_bssid_rust
	./test_hal_com_rcr_chk_bssid_rust

test_hal_com_rcr_chk_bssid_rust: test_hal_com_rcr_chk_bssid.c \
		$(RCR_CHK_BSSID_SHIM) $(RCR_CHK_BSSID_ORACLE) librust_hal_com_rcr_chk.a
	$(CC) $(RCR_CHK_BSSID_CFLAGS) -I../include -DRUST_HAL_COM_RCR_CHK_BSSID_ORACLE -o $@ \
		test_hal_com_rcr_chk_bssid.c $(RCR_CHK_BSSID_SHIM) $(RCR_CHK_BSSID_ORACLE) \
		librust_hal_com_rcr_chk.a