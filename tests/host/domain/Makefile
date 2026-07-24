# Host unit tests for rust/domain/types.rs (A1).
# No kernel headers — rustc only.

ROOT     := ../../..
RUST_SRC := $(ROOT)/rust/domain_types.rs

.PHONY: test clean

test: test_domain_types
	./test_domain_types

test_domain_types: $(RUST_SRC) $(ROOT)/rust/domain/types.rs
	rustc --test $(RUST_SRC) -o $@

clean:
	rm -f test_domain_types
