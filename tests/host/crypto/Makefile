# Host L2 differential test harness for core/crypto (T2/W1-03 aes-ctr, W2-01
# aes-omac1, W2-02 gcmp, W2-03 aes-siv, W2-04 aes-ccm, W2-05 sha256-internal,
# W2-06 sha256-prf, W2-06d rtw-crypto-wrap, W2-07 aes-gcm part 1).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
CRYPTO   := $(ROOT)/core/crypto
HOST_INC := ../include
RUST_CTR := $(ROOT)/rust/aes_ctr.rs
RUST_OMAC := $(ROOT)/rust/aes_omac1.rs
RUST_GCMP := $(ROOT)/rust/gcmp.rs
RUST_SIV := $(ROOT)/rust/aes_siv.rs
RUST_CCM := $(ROOT)/rust/aes_ccm.rs
RUST_GCM := $(ROOT)/rust/aes_gcm.rs
RUST_SHA256 := $(ROOT)/rust/sha256_internal.rs
RUST_SHA256_PRF := $(ROOT)/rust/sha256_prf.rs
RUST_CRYPTO_WRAP := $(ROOT)/rust/rtw_crypto_wrap.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I$(CRYPTO) \
           -DHOST_CRYPTO_TEST
# C oracle for rtw_crypto_wrap.c: compile the real TU without HOST_CRYPTO_TEST.
WRAP_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
               -I$(HOST_INC) -I$(CRYPTO)
# Match kernel CONFIG_RUST_OVERFLOW_CHECKS so host L2 catches wrap panics
# that would abort in extern "C" under /opt/linux.
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on

AES_LEAF_SRCS := \
	$(CRYPTO)/aes-internal.c \
	$(CRYPTO)/aes-internal-enc.c \
	../shim/host_crypto_shim.c

GCM_SRCS := $(CRYPTO)/aes-gcm.c
GCM_REST_SRCS := $(CRYPTO)/aes-gcm_rest.c
HOST_JSON := host_vector_json.c
HOST_GCMP := host_gcmp_vector.c

C_ORACLE_CTR_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-ctr.c $(HOST_JSON)
C_ORACLE_OMAC_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-omac1.c $(HOST_JSON)
C_ORACLE_GCM_SRCS := $(AES_LEAF_SRCS) $(GCM_SRCS) $(HOST_JSON)
GCM_AE_COMPILE_SRCS := $(AES_LEAF_SRCS) $(GCM_SRCS)
GCM_SPLIT_COMPILE_SRCS := $(AES_LEAF_SRCS) $(GCM_REST_SRCS)

C_ORACLE_GCMP_SRCS := $(AES_LEAF_SRCS) $(GCM_SRCS) $(CRYPTO)/gcmp.c \
		      $(HOST_JSON) $(HOST_GCMP)

GCMP_COMPILE_SRCS := $(AES_LEAF_SRCS) $(GCM_REST_SRCS) $(CRYPTO)/gcmp.c \
		     $(HOST_GCMP)

C_ORACLE_SIV_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-omac1.c $(CRYPTO)/aes-ctr.c \
		     $(CRYPTO)/aes-siv.c $(HOST_JSON)

SIV_COMPILE_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-omac1.c $(CRYPTO)/aes-ctr.c \
		    $(CRYPTO)/aes-siv.c

C_ORACLE_CCM_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-ccm.c $(HOST_JSON)
CCM_COMPILE_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-ccm.c

C_ORACLE_SHA256_SRCS := $(CRYPTO)/sha256-internal.c $(HOST_JSON)
SHA256_COMPILE_SRCS := $(CRYPTO)/sha256-internal.c

C_ORACLE_SHA256_PRF_SRCS := $(CRYPTO)/sha256-internal.c $(CRYPTO)/sha256.c \
			    $(CRYPTO)/sha256-prf.c ../shim/host_crypto_shim.c \
			    $(HOST_JSON)
SHA256_PRF_COMPILE_SRCS := $(CRYPTO)/sha256-internal.c $(CRYPTO)/sha256.c \
			   $(CRYPTO)/sha256-prf.c ../shim/host_crypto_shim.c

C_ORACLE_WRAP_SRCS := $(CRYPTO)/rtw_crypto_wrap.c ../shim/host_rtw_wrap_support.c \
		      $(HOST_JSON)
WRAP_COMPILE_SRCS := $(CRYPTO)/rtw_crypto_wrap.c ../shim/host_rtw_wrap_support.c

.PHONY: all test test-c test-rust test-omac1 test-omac1-c test-omac1-rust \
        test-gcmp test-gcmp-c test-gcmp-rust test-gcmp-compile \
	test-aes-gcm test-aes-gcm-c test-aes-gcm-rust test-aes-gcm-compile \
	test-aes-siv test-aes-siv-c test-aes-siv-rust test-aes-siv-compile \
	test-aes-ccm test-aes-ccm-c test-aes-ccm-rust test-aes-ccm-compile \
	test-sha256-internal test-sha256-internal-c test-sha256-internal-rust \
	test-sha256-internal-compile \
	test-sha256-prf test-sha256-prf-c test-sha256-prf-rust test-sha256-prf-compile \
	test-rtw-crypto-wrap test-rtw-crypto-wrap-c test-rtw-crypto-wrap-rust \
	test-rtw-crypto-wrap-compile \
	clean

all: test test-omac1 test-gcmp test-aes-gcm test-aes-siv test-aes-ccm test-sha256-internal \
     test-sha256-prf test-rtw-crypto-wrap

test: test-c test-rust

test-omac1: test-omac1-c test-omac1-rust

test-gcmp: test-gcmp-c test-gcmp-rust

test-aes-gcm: test-aes-gcm-c test-aes-gcm-rust

test-aes-gcm-compile: $(GCM_AE_COMPILE_SRCS) $(GCM_SPLIT_COMPILE_SRCS)
	@set -e; \
	for src in $(GCM_AE_COMPILE_SRCS) $(GCM_SPLIT_COMPILE_SRCS); do \
		obj="/tmp/$$(basename $$src .c)_gcm_compile_probe.o"; \
		$(CC) $(CFLAGS) -c -o $$obj $$src; \
	done

test-aes-gcm-c: test_aes_gcm
	./test_aes_gcm aes_gcm_vectors.json

test_aes_gcm: test_aes_gcm.c $(C_ORACLE_GCM_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_gcm.c $(C_ORACLE_GCM_SRCS)

librust_aes_gcm.a: $(RUST_GCM) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_GCM) -o $@

test-aes-gcm-rust: test_aes_gcm_rust
	./test_aes_gcm_rust aes_gcm_vectors.json

test_aes_gcm_rust: test_aes_gcm.c librust_aes_gcm.a $(AES_LEAF_SRCS) $(GCM_REST_SRCS) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_AES_GCM_ORACLE -o $@ test_aes_gcm.c $(AES_LEAF_SRCS) \
		$(GCM_REST_SRCS) $(HOST_JSON) librust_aes_gcm.a

test-c: test_aes_ctr
	./test_aes_ctr aes_ctr_vectors.json

test_aes_ctr: test_aes_ctr.c $(C_ORACLE_CTR_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_ctr.c $(C_ORACLE_CTR_SRCS)

librust_aes_ctr.a: $(RUST_CTR) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_CTR) -o $@

test-rust: test_aes_ctr_rust
	./test_aes_ctr_rust aes_ctr_vectors.json

test_aes_ctr_rust: test_aes_ctr.c librust_aes_ctr.a $(AES_LEAF_SRCS) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_aes_ctr.c $(AES_LEAF_SRCS) $(HOST_JSON) librust_aes_ctr.a

test-omac1-c: test_aes_omac1
	./test_aes_omac1 aes_omac1_vectors.json

test_aes_omac1: test_aes_omac1.c $(C_ORACLE_OMAC_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_omac1.c $(C_ORACLE_OMAC_SRCS)

librust_aes_omac1.a: $(RUST_OMAC) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_OMAC) -o $@

test-omac1-rust: test_aes_omac1_rust
	./test_aes_omac1_rust aes_omac1_vectors.json

test_aes_omac1_rust: test_aes_omac1.c librust_aes_omac1.a $(AES_LEAF_SRCS) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_OMAC1_ORACLE -o $@ test_aes_omac1.c $(AES_LEAF_SRCS) $(HOST_JSON) librust_aes_omac1.a

# Compile-only gate: shims must let gcmp.c / aes-gcm.c build under HOST_CRYPTO_TEST.
test-gcmp-compile: $(GCMP_COMPILE_SRCS)
	@set -e; \
	for src in $(GCMP_COMPILE_SRCS); do \
		obj="/tmp/$$(basename $$src .c)_compile_probe.o"; \
		$(CC) $(CFLAGS) -c -o $$obj $$src; \
	done

test-gcmp-c: test_gcmp
	./test_gcmp gcmp_vectors.json

test_gcmp: test_gcmp.c $(C_ORACLE_GCMP_SRCS)
	$(CC) $(CFLAGS) -o $@ test_gcmp.c $(C_ORACLE_GCMP_SRCS)

librust_gcmp.a: $(RUST_GCMP) $(ROOT)/rust/gcmp_support.rs $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_GCMP) -o $@

test-gcmp-rust: test_gcmp_rust
	./test_gcmp_rust gcmp_vectors.json

test_gcmp_rust: test_gcmp.c librust_gcmp.a librust_aes_gcm.a $(AES_LEAF_SRCS) \
		$(GCM_REST_SRCS) $(HOST_JSON) $(HOST_GCMP)
	$(CC) $(CFLAGS) -DRUST_GCMP_ORACLE -o $@ test_gcmp.c $(AES_LEAF_SRCS) \
		$(GCM_REST_SRCS) $(HOST_JSON) $(HOST_GCMP) librust_gcmp.a librust_aes_gcm.a

test-aes-siv: test-aes-siv-c test-aes-siv-rust

test-aes-siv-compile: $(SIV_COMPILE_SRCS)
	@set -e; \
	for src in $(SIV_COMPILE_SRCS); do \
		obj="/tmp/$$(basename $$src .c)_siv_compile_probe.o"; \
		$(CC) $(CFLAGS) -c -o $$obj $$src; \
	done

test-aes-siv-c: test_aes_siv
	./test_aes_siv aes_siv_vectors.json

test_aes_siv: test_aes_siv.c $(C_ORACLE_SIV_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_siv.c $(C_ORACLE_SIV_SRCS)

librust_aes_siv.a: $(RUST_SIV) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_SIV) -o $@

test-aes-siv-rust: test_aes_siv_rust
	./test_aes_siv_rust aes_siv_vectors.json

test_aes_siv_rust: test_aes_siv.c librust_aes_siv.a librust_aes_omac1.a librust_aes_ctr.a \
		   $(AES_LEAF_SRCS) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_AES_SIV_ORACLE -o $@ test_aes_siv.c $(AES_LEAF_SRCS) \
		$(HOST_JSON) librust_aes_siv.a librust_aes_omac1.a librust_aes_ctr.a

test-aes-ccm: test-aes-ccm-c test-aes-ccm-rust

test-aes-ccm-compile: $(CCM_COMPILE_SRCS)
	@set -e; \
	for src in $(CCM_COMPILE_SRCS); do \
		obj="/tmp/$$(basename $$src .c)_ccm_compile_probe.o"; \
		$(CC) $(CFLAGS) -c -o $$obj $$src; \
	done

test-aes-ccm-c: test_aes_ccm
	./test_aes_ccm aes_ccm_vectors.json

test_aes_ccm: test_aes_ccm.c $(C_ORACLE_CCM_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_ccm.c $(C_ORACLE_CCM_SRCS)

librust_aes_ccm.a: $(RUST_CCM) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_CCM) -o $@

test-aes-ccm-rust: test_aes_ccm_rust
	./test_aes_ccm_rust aes_ccm_vectors.json

test_aes_ccm_rust: test_aes_ccm.c librust_aes_ccm.a $(AES_LEAF_SRCS) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_AES_CCM_ORACLE -o $@ test_aes_ccm.c $(AES_LEAF_SRCS) \
		$(HOST_JSON) librust_aes_ccm.a

test-sha256-internal: test-sha256-internal-c test-sha256-internal-rust

test-sha256-internal-compile: $(SHA256_COMPILE_SRCS)
	@set -e; \
	for src in $(SHA256_COMPILE_SRCS); do \
		obj="/tmp/$$(basename $$src .c)_sha256_compile_probe.o"; \
		$(CC) $(CFLAGS) -c -o $$obj $$src; \
	done

test-sha256-internal-c: test_sha256_internal
	./test_sha256_internal sha256_internal_vectors.json

test_sha256_internal: test_sha256_internal.c $(C_ORACLE_SHA256_SRCS)
	$(CC) $(CFLAGS) -o $@ test_sha256_internal.c $(C_ORACLE_SHA256_SRCS)

librust_sha256_internal.a: $(RUST_SHA256)
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_SHA256) -o $@

test-sha256-internal-rust: test_sha256_internal_rust
	./test_sha256_internal_rust sha256_internal_vectors.json

test_sha256_internal_rust: test_sha256_internal.c librust_sha256_internal.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SHA256_INTERNAL_ORACLE -o $@ test_sha256_internal.c \
		$(HOST_JSON) librust_sha256_internal.a

test-sha256-prf: test-sha256-prf-c test-sha256-prf-rust

test-sha256-prf-compile: $(SHA256_PRF_COMPILE_SRCS)
	@set -e; \
	for src in $(SHA256_PRF_COMPILE_SRCS); do \
		obj="/tmp/$$(basename $$src .c)_sha256_prf_compile_probe.o"; \
		$(CC) $(CFLAGS) -c -o $$obj $$src; \
	done

test-sha256-prf-c: test_sha256_prf
	./test_sha256_prf sha256_prf_vectors.json

test_sha256_prf: test_sha256_prf.c $(C_ORACLE_SHA256_PRF_SRCS)
	$(CC) $(CFLAGS) -o $@ test_sha256_prf.c $(C_ORACLE_SHA256_PRF_SRCS)

librust_sha256_prf.a: $(RUST_SHA256_PRF)
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_SHA256_PRF) -o $@

test-sha256-prf-rust: test_sha256_prf_rust
	./test_sha256_prf_rust sha256_prf_vectors.json

test_sha256_prf_rust: test_sha256_prf.c librust_sha256_prf.a librust_sha256_internal.a \
		       $(CRYPTO)/sha256.c $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_SHA256_PRF_ORACLE -o $@ test_sha256_prf.c \
		$(CRYPTO)/sha256.c $(HOST_JSON) librust_sha256_prf.a librust_sha256_internal.a

test-rtw-crypto-wrap: test-rtw-crypto-wrap-c test-rtw-crypto-wrap-rust

test-rtw-crypto-wrap-compile: $(WRAP_COMPILE_SRCS)
	@set -e; \
	for src in $(WRAP_COMPILE_SRCS); do \
		obj="/tmp/$$(basename $$src .c)_rtw_wrap_compile_probe.o"; \
		$(CC) $(WRAP_CFLAGS) -c -o $$obj $$src; \
	done

test-rtw-crypto-wrap-c: test_rtw_crypto_wrap
	./test_rtw_crypto_wrap rtw_crypto_wrap_vectors.json

test_rtw_crypto_wrap: test_rtw_crypto_wrap.c $(C_ORACLE_WRAP_SRCS)
	$(CC) $(WRAP_CFLAGS) -o $@ test_rtw_crypto_wrap.c $(C_ORACLE_WRAP_SRCS)

librust_rtw_crypto_wrap.a: $(RUST_CRYPTO_WRAP)
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_CRYPTO_WRAP) -o $@

test-rtw-crypto-wrap-rust: test_rtw_crypto_wrap_rust
	./test_rtw_crypto_wrap_rust rtw_crypto_wrap_vectors.json

test_rtw_crypto_wrap_rust: test_rtw_crypto_wrap.c librust_rtw_crypto_wrap.a \
			   ../shim/host_rtw_wrap_support.c $(HOST_JSON)
	$(CC) $(WRAP_CFLAGS) -DRUST_RTW_CRYPTO_WRAP_ORACLE -o $@ test_rtw_crypto_wrap.c \
		../shim/host_rtw_wrap_support.c $(HOST_JSON) librust_rtw_crypto_wrap.a

clean:
	rm -f test_aes_ctr test_aes_ctr_rust librust_aes_ctr.a \
	      test_aes_omac1 test_aes_omac1_rust librust_aes_omac1.a \
	      test_gcmp test_gcmp_rust librust_gcmp.a \
	      test_aes_gcm test_aes_gcm_rust librust_aes_gcm.a \
	      test_aes_siv test_aes_siv_rust librust_aes_siv.a \
	      test_aes_ccm test_aes_ccm_rust librust_aes_ccm.a \
	      test_sha256_internal test_sha256_internal_rust librust_sha256_internal.a \
	      test_sha256_prf test_sha256_prf_rust librust_sha256_prf.a \
	      test_rtw_crypto_wrap test_rtw_crypto_wrap_rust librust_rtw_crypto_wrap.a
