# Host L2 differential tests for core/rtw_security.c helpers (T5).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
HOST_INC := ../include
SHIM     := ../shim/host_security_shim.c
ORACLE   := security_c_oracle.c
REST_ORACLE := security_rest_c_oracle.c
RUST_SECURITY := $(ROOT)/rust/rtw_security.rs
RUST_SECURITY_REST := $(ROOT)/rust/rtw_security_rest.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I. \
           -DCONFIG_IEEE80211W
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust test-wep test-wep-rust test-wep-frames test-wep-frames-rust test-tkip test-tkip-rust test-tkip-phase test-tkip-phase-rust test-tkip-frames test-tkip-frames-rust test-ccmp-primitives test-ccmp-primitives-rust test-ccmp-construct test-ccmp-construct-rust clean

all: test

test: test-c test-rust test-wep test-wep-rust test-wep-frames test-wep-frames-rust test-tkip test-tkip-rust test-tkip-phase test-tkip-phase-rust test-tkip-frames test-ccmp-primitives test-ccmp-primitives-rust test-ccmp-construct test-ccmp-construct-rust

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

test_security: test_security.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_security.c $(ORACLE) $(HOST_JSON)

librust_security.a: $(RUST_SECURITY) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_security_test --crate-type staticlib \
		$(RUST_SECURITY) -o $@

librust_security_rest.a: $(RUST_SECURITY_REST)
	$(RUSTC) $(RUSTFLAGS) --cfg host_security_rest_test --crate-type staticlib \
		$(RUST_SECURITY_REST) -o $@

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

test_ccmp_primitives_rust: test_ccmp_primitives.c librust_security_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_REST_ORACLE -o $@ test_ccmp_primitives.c $(HOST_JSON) \
		librust_security_rest.a

test_ccmp_construct: test_ccmp_construct.c $(REST_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_CCMP_CONSTRUCT_ORACLE_BUILD -o $@ test_ccmp_construct.c $(REST_ORACLE) $(HOST_JSON)

test_ccmp_construct_rust: test_ccmp_construct.c librust_security_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SECURITY_REST_ORACLE -o $@ test_ccmp_construct.c $(HOST_JSON) \
		librust_security_rest.a

clean:
	rm -f test_security test_security_rust test_wep_primitives test_wep_primitives_rust \
		test_wep_frames test_wep_frames_rust test_tkip_mic test_tkip_mic_rust \
		test_tkip_phase test_tkip_phase_rust test_tkip_frames test_tkip_frames_rust \
		test_ccmp_primitives test_ccmp_primitives_rust \
		test_ccmp_construct test_ccmp_construct_rust \
		librust_security.a librust_security_rest.a
