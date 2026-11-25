# W4-137 PR2 — Rust L2 targets.

librust_hal_com_rcr_am.a: $(ROOT)/rust/hal_com_rcr_am_host.rs
	rustc --edition 2021 -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(ROOT)/rust/hal_com_rcr_am_host.rs -o $@

test-rcr-am-hwvar-rust: test_hal_com_rcr_am_rust
	./test_hal_com_rcr_am_rust

test_hal_com_rcr_am_rust: test_hal_com_rcr_am.c $(RCR_AM_SHIM) \
		hal_com_rcr_am_rust_set_hwreg.c librust_hal_com_rcr_am.a
	$(CC) $(CFLAGS) -I../include -o $@ test_hal_com_rcr_am.c \
		$(RCR_AM_SHIM) hal_com_rcr_am_rust_set_hwreg.c librust_hal_com_rcr_am.a