# Host L2 differential tests for core/rtw_security.c helpers (T5).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
HOST_INC := ../include
SHIM     := ../shim/host_security_shim.c
ORACLE   := security_c_oracle.c
REST_ORACLE := security_rest_c_oracle.c
RUST_SECURITY := $(ROOT)/rust/rtw_security.rs
RUST_SECURITY_REST := $(ROOT)/rust/rtw_security_rest.rs
RUST_SWCRYPTO := $(ROOT)/rust/rtw_swcrypto.rs
SWCRYPTO_SHIM := ../shim/host_swcrypto_shim.c
CRYPTO_DIR := $(ROOT)/core/crypto
AES_LEAF_SRCS := \
	$(CRYPTO_DIR)/aes-internal.c \
	$(CRYPTO_DIR)/aes-internal-enc.c \
	../shim/host_crypto_shim.c
GCMP_CRYPTO_SRCS := $(CRYPTO_DIR)/aes-gcm.c $(CRYPTO_DIR)/gcmp.c
SWCRYPTO_CRYPTO_SRCS := $(AES_LEAF_SRCS) $(GCMP_CRYPTO_SRCS) \
	$(CRYPTO_DIR)/aes-ccm.c $(CRYPTO_DIR)/ccmp.c \
	$(CRYPTO_DIR)/aes-omac1.c $(CRYPTO_DIR)/aes-ctr.c \
	$(CRYPTO_DIR)/aes-siv.c

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I. \
           -DCONFIG_IEEE80211W
SWCRYPTO_CFLAGS := $(CFLAGS) -I$(CRYPTO_DIR) -include ../include/host_autoconf.h \
	-DHOST_CRYPTO_TEST -DHOST_SWCRYPTO_TEST
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust test-wep test-wep-rust test-wep-frames test-wep-frames-rust test-tkip test-tkip-rust test-tkip-phase test-tkip-phase-rust test-tkip-frames test-tkip-frames-rust test-ccmp-primitives test-ccmp-primitives-rust test-ccmp-construct test-ccmp-construct-rust test-ccmp-frames test-ccmp-frames-rust test-gcmp-frames test-gcmp-frames-rust test-rest-misc test-rest-misc-rust clean

all: test

test: test-c test-rust test-wep test-wep-rust test-wep-frames test-wep-frames-rust test-tkip test-tkip-rust test-tkip-phase test-tkip-phase-rust test-tkip-frames test-tkip-frames-rust test-ccmp-primitives test-ccmp-primitives-rust test-ccmp-construct test-ccmp-construct-rust test-ccmp-frames test-ccmp-frames-rust test-gcmp-frames test-gcmp-frames-rust test-rest-misc

test-c: test_security
	./test_security security_type_vectors.json

test-rust: test_security_rust
	./test_security_rust security_type_vectors.json

test-wep: test_wep_primitives
	./test_wep_primitives wep_primitive_vectors.json

test-wep-rust: test_wep_primitives_rust
	./test_wep_primitives_rust wep_primitive_vectors.json

test-wep-frames: test_wep_frames
	./test_wep_frames wep_frame_vectors.json

test-wep-frames-rust: test_wep_frames_rust
	./test_wep_frames_rust wep_frame_vectors.json

test-tkip: test_tkip_mic
	./test_tkip_mic tkip_mic_vectors.json

test-tkip-rust: test_tkip_mic_rust
	./test_tkip_mic_rust tkip_mic_vectors.json

test-tkip-phase: test_tkip_phase
	./test_tkip_phase tkip_phase_vectors.json

test-tkip-phase-rust: test_tkip_phase_rust
	./test_tkip_phase_rust tkip_phase_vectors.json

test-tkip-frames: test_tkip_frames
	./test_tkip_frames tkip_frame_vectors.json

test-tkip-frames-rust: test_tkip_frames_rust
	./test_tkip_frames_rust tkip_frame_vectors.json

test-ccmp-primitives: test_ccmp_primitives
	./test_ccmp_primitives ccmp_primitive_vectors.json

test-ccmp-primitives-rust: test_ccmp_primitives_rust
	./test_ccmp_primitives_rust ccmp_primitive_vectors.json

test-ccmp-construct: test_ccmp_construct
	./test_ccmp_construct ccmp_construct_vectors.json

test-ccmp-construct-rust: test_ccmp_construct_rust
	./test_ccmp_construct_rust ccmp_construct_vectors.json

test-ccmp-frames: test_ccmp_frames
	./test_ccmp_frames ccmp_frame_vectors.json

test-ccmp-frames-rust: test_ccmp_frames_rust
	./test_ccmp_frames_rust ccmp_frame_vectors.json

test-gcmp-frames: test_gcmp_frames
	./test_gcmp_frames gcmp_frame_vectors.json

test-gcmp-frames-rust: test_gcmp_frames_rust
	./test_gcmp_frames_rust gcmp_frame_vectors.json

test-rest-misc: test_rest_misc
	./test_rest_misc rest_misc_vectors.json

# test-rest-misc-rust is added to aggregate `test` in W3-15 PR2 (stacked Rust port).
test-rest-misc-rust: test_rest_misc_rust
	./test_rest_misc_rust rest_misc_vectors.json

MISC_CRYPTO_SRCS := $(CRYPTO_DIR)/aes-internal.c $(CRYPTO_DIR)/aes-internal-enc.c \
	$(CRYPTO_DIR)/aes-omac1.c $(CRYPTO_DIR)/aes-ctr.c $(CRYPTO_DIR)/aes-siv.c \
	../shim/host_crypto_shim.c

test_rest_misc: test_rest_misc.c $(REST_ORACLE) $(ORACLE) $(MISC_CRYPTO_SRCS) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_REST_MISC_ORACLE_BUILD -DHOST_WEP_ORACLE_BUILD \
		-I$(CRYPTO_DIR) -include ../include/host_autoconf.h -DHOST_CRYPTO_TEST \
		-o $@ test_rest_misc.c $(REST_ORACLE) $(ORACLE) $(MISC_CRYPTO_SRCS) $(HOST_JSON)

librust_security_rest_misc.a: $(RUST_SECURITY_REST)
	$(RUSTC) $(RUSTFLAGS) --cfg host_security_rest_test --cfg host_rest_misc_test \
		--crate-type staticlib $(RUST_SECURITY_REST) -o $@

test_rest_misc_rust: test_rest_misc.c librust_security_rest_misc.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_REST_ORACLE -o $@ test_rest_misc.c $(HOST_JSON) \
		librust_security_rest_misc.a

test_security: test_security.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_security.c $(ORACLE) $(HOST_JSON)

librust_security.a: $(RUST_SECURITY) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_security_test --crate-type staticlib \
		$(RUST_SECURITY) -o $@

librust_security_rest.a: $(RUST_SECURITY_REST)
	$(RUSTC) $(RUSTFLAGS) --cfg host_security_rest_test --crate-type staticlib \
		$(RUST_SECURITY_REST) -o $@

librust_security_rest_gcmp.a: $(RUST_SECURITY_REST)
	$(RUSTC) $(RUSTFLAGS) --cfg host_security_rest_test --cfg host_gcmp_frame_test \
		--crate-type staticlib $(RUST_SECURITY_REST) -o $@

test_security_rust: test_security.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_security.c $(HOST_JSON) \
		librust_security.a

test_wep_primitives: test_wep_primitives.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_WEP_ORACLE_BUILD -o $@ test_wep_primitives.c $(ORACLE) $(HOST_JSON)

test_wep_primitives_rust: test_wep_primitives.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_wep_primitives.c $(HOST_JSON) \
		librust_security.a

test_wep_frames: test_wep_frames.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_WEP_FRAME_ORACLE_BUILD -o $@ test_wep_frames.c $(SHIM) $(HOST_JSON)

test_wep_frames_rust: test_wep_frames.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_wep_frames.c $(HOST_JSON) \
		librust_security.a

test_tkip_mic: test_tkip_mic.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_TKIP_ORACLE_BUILD -o $@ test_tkip_mic.c $(ORACLE) $(HOST_JSON)

test_tkip_mic_rust: test_tkip_mic.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_tkip_mic.c $(HOST_JSON) \
		librust_security.a

test_tkip_phase: test_tkip_phase.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_TKIP_PHASE_ORACLE_BUILD -o $@ test_tkip_phase.c $(ORACLE) $(HOST_JSON)

test_tkip_phase_rust: test_tkip_phase.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_tkip_phase.c $(HOST_JSON) \
		librust_security.a

test_tkip_frames: test_tkip_frames.c $(SHIM) $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_TKIP_FRAME_ORACLE_BUILD -o $@ test_tkip_frames.c $(SHIM) $(ORACLE) $(HOST_JSON)

test_tkip_frames_rust: test_tkip_frames.c librust_security.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_ORACLE -o $@ test_tkip_frames.c $(HOST_JSON) \
		librust_security.a

test_ccmp_primitives: test_ccmp_primitives.c $(REST_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_CCMP_PRIMITIVE_ORACLE_BUILD -o $@ test_ccmp_primitives.c $(REST_ORACLE) $(HOST_JSON)

# librust_security_rest.a is added in W3-11 PR2 (stacked Rust port).
test_ccmp_primitives_rust: test_ccmp_primitives.c librust_security_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_REST_ORACLE -o $@ test_ccmp_primitives.c $(HOST_JSON) \
		librust_security_rest.a

test_ccmp_construct: test_ccmp_construct.c $(REST_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_CCMP_CONSTRUCT_ORACLE_BUILD -o $@ test_ccmp_construct.c $(REST_ORACLE) $(HOST_JSON)

test_ccmp_construct_rust: test_ccmp_construct.c librust_security_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_REST_ORACLE -o $@ test_ccmp_construct.c $(HOST_JSON) \
		librust_security_rest.a

CCMP_ORACLE_FLAGS := -DHOST_CCMP_PRIMITIVE_ORACLE_BUILD -DHOST_CCMP_CONSTRUCT_ORACLE_BUILD -DHOST_CCMP_FRAME_ORACLE_BUILD

test_ccmp_frames: test_ccmp_frames.c $(REST_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) $(CCMP_ORACLE_FLAGS) -o $@ test_ccmp_frames.c $(REST_ORACLE) $(HOST_JSON)

# test-ccmp-frames-rust is added to aggregate `test` in W3-12 PR2 (stacked Rust port).
test_ccmp_frames_rust: test_ccmp_frames.c librust_security_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_REST_ORACLE -o $@ test_ccmp_frames.c $(HOST_JSON) \
		librust_security_rest.a

librust_swcrypto.a: $(RUST_SWCRYPTO)
	$(RUSTC) $(RUSTFLAGS) --cfg host_swcrypto_test --crate-type staticlib \
		$(RUST_SWCRYPTO) -o $@

test_gcmp_frames: test_gcmp_frames.c $(SHIM) librust_swcrypto.a $(SWCRYPTO_SHIM) \
		$(SWCRYPTO_CRYPTO_SRCS) $(HOST_JSON)
	$(CC) $(SWCRYPTO_CFLAGS) -DHOST_GCMP_FRAME_ORACLE_BUILD -o $@ test_gcmp_frames.c $(SHIM) \
		$(SWCRYPTO_SHIM) $(SWCRYPTO_CRYPTO_SRCS) $(HOST_JSON) librust_swcrypto.a

# test-gcmp-frames-rust: full Rust encrypt/decrypt (W3-14 PR3).
test_gcmp_frames_rust: test_gcmp_frames.c librust_security_rest_gcmp.a librust_swcrypto.a \
		$(SWCRYPTO_SHIM) $(SWCRYPTO_CRYPTO_SRCS) $(HOST_JSON)
	$(CC) $(SWCRYPTO_CFLAGS) -DRUST_SECURITY_REST_ORACLE -o $@ test_gcmp_frames.c $(HOST_JSON) \
		$(SWCRYPTO_SHIM) $(SWCRYPTO_CRYPTO_SRCS) librust_security_rest_gcmp.a librust_swcrypto.a

clean:
	rm -f test_security test_security_rust test_wep_primitives test_wep_primitives_rust \
		test_wep_frames test_wep_frames_rust test_tkip_mic test_tkip_mic_rust \
		test_tkip_phase test_tkip_phase_rust test_tkip_frames test_tkip_frames_rust \
		test_ccmp_primitives test_ccmp_primitives_rust \
		test_ccmp_construct test_ccmp_construct_rust \
		test_ccmp_frames test_ccmp_frames_rust \
		test_gcmp_frames test_gcmp_frames_rust \
		test_rest_misc test_rest_misc_rust \
		librust_security.a librust_security_rest.a librust_security_rest_gcmp.a \
		librust_security_rest_misc.a librust_swcrypto.a
