# Host L2 differential tests for core/rtw_swcrypto.c wrappers (W3-01/W3-02).
# Builds with gcc only — no kernel headers or KDIR.

include ../rust.mk

ROOT     := ../../..
CORE     := $(ROOT)/core
CRYPTO   := $(ROOT)/core/crypto
HOST_INC := ../include
RUST_SWCRYPTO := $(ROOT)/rust/rtw_swcrypto.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
           -I$(HOST_INC) -I$(CRYPTO) \
           -include $(HOST_INC)/host_autoconf.h \
           -DHOST_CRYPTO_TEST -DHOST_SWCRYPTO_TEST
RUSTFLAGS := $(RUST_BASE_FLAGS)

AES_LEAF_SRCS := \
	$(CRYPTO)/aes-internal.c \
	$(CRYPTO)/aes-internal-enc.c \
	../shim/host_crypto_shim.c

CCMP_SRCS := $(AES_LEAF_SRCS) $(CRYPTO)/aes-ccm.c $(CRYPTO)/ccmp.c
GCMP_SRCS := $(CRYPTO)/aes-gcm.c $(CRYPTO)/gcmp.c
CRYPTO_SRCS := $(CCMP_SRCS) $(GCMP_SRCS) $(CRYPTO)/aes-omac1.c $(CRYPTO)/aes-ctr.c \
		$(CRYPTO)/aes-siv.c
SWCRYPTO_SRCS := $(CORE)/rtw_swcrypto.c $(CORE)/rtw_swcrypto_rest.c
SWCRYPTO_SHIM := ../shim/host_swcrypto_shim.c
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_swcrypto
	./test_swcrypto swcrypto_vectors.json

test_swcrypto: test_swcrypto.c host_swcrypto_vector.c $(SWCRYPTO_SRCS) \
	       $(CRYPTO_SRCS) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_swcrypto.c host_swcrypto_vector.c \
		$(SWCRYPTO_SRCS) $(CRYPTO_SRCS) $(HOST_JSON)

librust_swcrypto.a: $(RUST_SWCRYPTO)
	$(RUSTC) $(RUSTFLAGS) --cfg host_swcrypto_test --crate-type staticlib \
		$(RUST_SWCRYPTO) -o $@

test-rust: test_swcrypto_rust
	./test_swcrypto_rust swcrypto_vectors.json

test_swcrypto_rust: test_swcrypto.c host_swcrypto_vector.c librust_swcrypto.a \
		    $(CRYPTO_SRCS) $(HOST_JSON) $(SWCRYPTO_SHIM)
	$(CC) $(CFLAGS) -DRUST_SWCRYPTO_ORACLE -o $@ test_swcrypto.c \
		host_swcrypto_vector.c $(CRYPTO_SRCS) $(HOST_JSON) $(SWCRYPTO_SHIM) \
		librust_swcrypto.a

clean:
	rm -f test_swcrypto test_swcrypto_rust librust_swcrypto.a
