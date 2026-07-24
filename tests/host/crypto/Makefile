# Host L2 differential test harness for core/crypto (T2/W1-03 aes-ctr, W2-01 aes-omac1, W2-02 gcmp, W2-03 aes-siv).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
CRYPTO   := $(ROOT)/core/crypto
HOST_INC := ../include
RUST_CTR := $(ROOT)/rust/aes_ctr.rs
RUST_OMAC := $(ROOT)/rust/aes_omac1.rs
RUST_GCMP := $(ROOT)/rust/gcmp.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I$(CRYPTO) \
           -DHOST_CRYPTO_TEST
RUSTFLAGS := -C opt-level=2

AES_LEAF_SRCS := \
	$(CRYPTO)/aes-internal.c \
	$(CRYPTO)/aes-internal-enc.c \
	../shim/host_crypto_shim.c

GCM_SRCS := $(CRYPTO)/aes-gcm.c
HOST_JSON := host_vector_json.c
HOST_GCMP := host_gcmp_vector.c

C_ORACLE_CTR_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-ctr.c $(HOST_JSON)
C_ORACLE_OMAC_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-omac1.c $(HOST_JSON)
C_ORACLE_GCMP_SRCS := $(AES_LEAF_SRCS) $(GCM_SRCS) $(CRYPTO)/gcmp.c \
		      $(HOST_JSON) $(HOST_GCMP)

GCMP_COMPILE_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-gcm.c $(CRYPTO)/gcmp.c \
		     $(HOST_GCMP)

C_ORACLE_SIV_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-omac1.c $(CRYPTO)/aes-ctr.c \
		     $(CRYPTO)/aes-siv.c $(HOST_JSON)

SIV_COMPILE_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-omac1.c $(CRYPTO)/aes-ctr.c \
		    $(CRYPTO)/aes-siv.c

.PHONY: all test test-c test-rust test-omac1 test-omac1-c test-omac1-rust \
        test-gcmp test-gcmp-c test-gcmp-rust test-gcmp-compile \
	test-aes-siv test-aes-siv-c test-aes-siv-compile clean

all: test test-omac1 test-gcmp test-aes-siv

test: test-c test-rust

test-omac1: test-omac1-c test-omac1-rust

test-gcmp: test-gcmp-c test-gcmp-rust

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

test_gcmp_rust: test_gcmp.c librust_gcmp.a $(AES_LEAF_SRCS) $(GCM_SRCS) $(HOST_JSON) $(HOST_GCMP)
	$(CC) $(CFLAGS) -DRUST_GCMP_ORACLE -o $@ test_gcmp.c $(AES_LEAF_SRCS) $(GCM_SRCS) $(HOST_JSON) $(HOST_GCMP) librust_gcmp.a

test-aes-siv: test-aes-siv-c

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

clean:
	rm -f test_aes_ctr test_aes_ctr_rust librust_aes_ctr.a \
	      test_aes_omac1 test_aes_omac1_rust librust_aes_omac1.a \
	      test_gcmp test_gcmp_rust librust_gcmp.a \
	      test_aes_siv
