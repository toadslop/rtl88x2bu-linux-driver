# W4-127 PR2 — Rust L2 targets.

librust_hal_com_rcr.a: $(ROOT)/rust/hal_com_rcr_host.rs $(ROOT)/rust/hal_com_rcr.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(ROOT)/rust/hal_com_rcr_host.rs -o $@

test-rcr-leaf-rust: test_hal_com_rcr_rust
	./test_hal_com_rcr_rust

test_hal_com_rcr_rust: test_hal_com_rcr.c $(RCR_SHIM) $(RCR_ORACLE) librust_hal_com_rcr.a
	$(CC) $(CFLAGS) -I../include -DRUST_HAL_COM_RCR_ORACLE -o $@ \
		test_hal_com_rcr.c $(RCR_SHIM) $(RCR_ORACLE) librust_hal_com_rcr.a