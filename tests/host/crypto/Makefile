# Host L2 differential test harness for core/crypto/aes-ctr.c (T2) and Rust port (W1-03).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
CRYPTO   := $(ROOT)/core/crypto
HOST_INC := ../include
RUST_SRC := $(ROOT)/rust/aes_ctr.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -O2 \
           -I$(HOST_INC) -I$(CRYPTO) \
           -DHOST_CRYPTO_TEST
RUSTFLAGS := -C opt-level=2

C_ORACLE_SRCS := \
	$(CRYPTO)/aes-internal.c \
	$(CRYPTO)/aes-internal-enc.c \
	$(CRYPTO)/aes-ctr.c \
	../shim/host_crypto_shim.c

AES_LEAF_SRCS := \
	$(CRYPTO)/aes-internal.c \
	$(CRYPTO)/aes-internal-enc.c \
	../shim/host_crypto_shim.c

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_aes_ctr
	./test_aes_ctr aes_ctr_vectors.json

test_aes_ctr: test_aes_ctr.c $(C_ORACLE_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_ctr.c $(C_ORACLE_SRCS)

librust_aes_ctr.a: $(RUST_SRC) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_crypto_test --crate-type staticlib $(RUST_SRC) -o $@

test-rust: test_aes_ctr_rust
	./test_aes_ctr_rust aes_ctr_vectors.json

test_aes_ctr_rust: test_aes_ctr.c librust_aes_ctr.a $(AES_LEAF_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_ctr.c $(AES_LEAF_SRCS) librust_aes_ctr.a

clean:
	rm -f test_aes_ctr test_aes_ctr_rust librust_aes_ctr.a
