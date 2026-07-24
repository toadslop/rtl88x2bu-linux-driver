# Host L2 differential test harness for core/crypto (T2/W1-03 aes-ctr, W2-01 aes-omac1).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
CRYPTO   := $(ROOT)/core/crypto
HOST_INC := ../include
RUST_CTR := $(ROOT)/rust/aes_ctr.rs
RUST_OMAC := $(ROOT)/rust/aes_omac1.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -O2 \
           -I$(HOST_INC) -I$(CRYPTO) \
           -DHOST_CRYPTO_TEST
RUSTFLAGS := -C opt-level=2

AES_LEAF_SRCS := \
	$(CRYPTO)/aes-internal.c \
	$(CRYPTO)/aes-internal-enc.c \
	../shim/host_crypto_shim.c

HOST_JSON := host_vector_json.c

C_ORACLE_CTR_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-ctr.c $(HOST_JSON)
C_ORACLE_OMAC_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-omac1.c $(HOST_JSON)

GCMP_COMPILE_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-gcm.c $(CRYPTO)/gcmp.c

.PHONY: all test test-c test-rust test-omac1 test-omac1-c test-omac1-rust test-gcmp-compile clean

all: test test-omac1

test: test-c test-rust

test-omac1: test-omac1-c test-omac1-rust

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
test-gcmp-compile:
	$(CC) $(CFLAGS) -c -o /tmp/gcmp_compile_probe.o $(CRYPTO)/gcmp.c
	$(CC) $(CFLAGS) -c -o /tmp/aes_gcm_compile_probe.o $(CRYPTO)/aes-gcm.c

clean:
	rm -f test_aes_ctr test_aes_ctr_rust librust_aes_ctr.a \
	      test_aes_omac1 test_aes_omac1_rust librust_aes_omac1.a
