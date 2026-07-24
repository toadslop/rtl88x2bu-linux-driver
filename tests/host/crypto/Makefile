# Host L2 differential test harness for core/crypto/aes-ctr.c (T2).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
CRYPTO   := $(ROOT)/core/crypto
HOST_INC := ../include

CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -O2 \
           -I$(HOST_INC) -I$(CRYPTO) \
           -DHOST_CRYPTO_TEST

ORACLE_SRCS := \
	$(CRYPTO)/aes-internal.c \
	$(CRYPTO)/aes-internal-enc.c \
	$(CRYPTO)/aes-ctr.c \
	../shim/host_crypto_shim.c

.PHONY: all test clean

all: test_aes_ctr

test_aes_ctr: test_aes_ctr.c $(ORACLE_SRCS)
	$(CC) $(CFLAGS) -o $@ test_aes_ctr.c $(ORACLE_SRCS)

test: test_aes_ctr
	./test_aes_ctr aes_ctr_vectors.json

clean:
	rm -f test_aes_ctr
