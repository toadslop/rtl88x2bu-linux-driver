# Host unit tests for rust/domain/types.rs (A1).
# No kernel headers — rustc only.

include ../rust.mk

ROOT     := ../../..
RUST_SRC := $(ROOT)/rust/domain_types.rs
RUSTC    := rustc

.PHONY: test clean

test: test_domain_types
	./test_domain_types

test_domain_types: $(RUST_SRC) $(ROOT)/rust/domain/types.rs
	$(RUSTC) --test $(RUST_BASE_FLAGS) $(RUST_SRC) -o $@

clean:
	rm -f test_domain_types
