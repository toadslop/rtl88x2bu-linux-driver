# Host L2 differential tests for core/rtw_chplan.c (W2-17b).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RUST_CHPLAN := $(ROOT)/rust/rtw_chplan.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
           -I$(HOST_INC) -I$(CORE) \
           -include $(HOST_INC)/host_autoconf.h \
           -DHOST_CHPLAN_TEST
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on

CHPLAN_SRCS := $(CORE)/rtw_chplan.c $(CORE)/rtw_chplan_rest.c ../shim/host_chplan_rf.c \
               ../crypto/host_vector_json.c
CHPLAN_DATA_SRCS := $(CORE)/rtw_chplan_rest.c
HOST_JSON := ../crypto/host_vector_json.c
DATA_CFLAGS := $(CFLAGS) -DHOST_CHPLAN_DATA_ONLY

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_chplan
	./test_chplan chplan_vectors.json

test_chplan: test_chplan.c $(CHPLAN_SRCS)
	$(CC) $(CFLAGS) -o $@ test_chplan.c $(CHPLAN_SRCS)

librust_chplan.a: $(RUST_CHPLAN)
	$(RUSTC) $(RUSTFLAGS) --cfg host_chplan_test --cfg ieee80211_band_5ghz --crate-type staticlib \
		$(RUST_CHPLAN) -o $@

test-rust: test_chplan_rust
	./test_chplan_rust chplan_vectors.json

test_chplan_rust: test_chplan.c librust_chplan.a chplan_data.o $(HOST_JSON) ../shim/host_chplan_rf.c
	$(CC) $(CFLAGS) -o $@ test_chplan.c $(HOST_JSON) \
		../shim/host_chplan_rf.c chplan_data.o librust_chplan.a

chplan_data.o: $(CHPLAN_DATA_SRCS)
	$(CC) $(DATA_CFLAGS) -c -o $@ $(CORE)/rtw_chplan_rest.c

clean:
	rm -f test_chplan test_chplan_rust librust_chplan.a chplan_data.o
