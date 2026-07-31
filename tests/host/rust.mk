# Shared rustc flags for host L2 oracles — edition must match kbuild rustflags-y.
RUST_EDITION := 2024
RUST_BASE_FLAGS := --edition=$(RUST_EDITION) -C opt-level=2 -C overflow-checks=on
