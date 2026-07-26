# Host L2 differential tests for core/rtw_security.c helpers (T5).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
HOST_INC := ../include
ORACLE   := security_c_oracle.c
RUST_SECURITY := $(ROOT)/rust/rtw_security.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I. \
           -DCONFIG_IEEE80211W
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust test-wep test-wep-rust test-tkip clean

all: test

test: test-c test-rust test-wep test-wep-rust test-tkip

test-c: test_security
	./test_security security_type_vectors.json

test-rust: test_security_rust
	./test_security_rust security_type_vectors.json

test-wep: test_wep_primitives
	./test_wep_primitives wep_primitive_vectors.json

test-wep-rust: test_wep_primitives_rust
	./test_wep_primitives_rust wep_primitive_vectors.json

test-tkip: test_tkip_mic
	./test_tkip_mic tkip_mic_vectors.json

test_security: test_security.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_security.c $(ORACLE) $(HOST_JSON)

librust_security.a: $(RUST_SECURITY) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_security_test --crate-type staticlib \
		$(RUST_SECURITY) -o $@

test_security_rust: test_security.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_security.c $(HOST_JSON) \
		librust_security.a

test_wep_primitives: test_wep_primitives.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_WEP_ORACLE_BUILD -o $@ test_wep_primitives.c $(ORACLE) $(HOST_JSON)

test_wep_primitives_rust: test_wep_primitives.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_wep_primitives.c $(HOST_JSON) \
		librust_security.a

test_tkip_mic: test_tkip_mic.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_tkip_mic.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_security test_security_rust test_wep_primitives test_wep_primitives_rust \
		test_tkip_mic librust_security.a
