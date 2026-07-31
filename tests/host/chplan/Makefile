# Host L2 differential tests for core/rtw_chplan.c (W2-17b).
# Builds with gcc only — no kernel headers or KDIR.

include ../rust.mk

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
RUSTFLAGS := $(RUST_BASE_FLAGS) --cfg dfs

CHPLAN_SRCS := $(CORE)/rtw_chplan.c $(CORE)/rtw_chplan_rest.c ../shim/host_chplan_rf.c \
               ../shim/host_chplan_hal.c ../crypto/host_vector_json.c
CHPLAN_DATA_SRCS := $(CORE)/rtw_chplan_rest.c
HOST_JSON := ../crypto/host_vector_json.c
DATA_CFLAGS := $(CFLAGS) -DHOST_CHPLAN_DATA_ONLY

CHPLAN_REST_ORACLE := chplan_rest_c_oracle.c
RUST_CHPLAN_REST := $(ROOT)/rust/rtw_chplan_rest.rs

.PHONY: all test test-c test-rust test-rest-beacon-hint test-rest-beacon-hint-rust clean

all: test

test: test-c test-rust test-rest-beacon-hint test-rest-beacon-hint-rust

test-c: test_chplan
	./test_chplan chplan_vectors.json

test_chplan: test_chplan.c $(CHPLAN_SRCS)
	$(CC) $(CFLAGS) -o $@ test_chplan.c $(CHPLAN_SRCS)

librust_chplan.a: $(RUST_CHPLAN)
	$(RUSTC) $(RUSTFLAGS) --cfg host_chplan_test --cfg ieee80211_band_5ghz --crate-type staticlib \
		$(RUST_CHPLAN) -o $@

test-rust: test_chplan_rust
	./test_chplan_rust chplan_vectors.json

test_chplan_rust: test_chplan.c librust_chplan.a chplan_data.o $(HOST_JSON) ../shim/host_chplan_rf.c ../shim/host_chplan_hal.c
	$(CC) $(CFLAGS) -DRUST_CHPLAN_ORACLE -o $@ test_chplan.c $(HOST_JSON) \
		../shim/host_chplan_rf.c ../shim/host_chplan_hal.c chplan_data.o librust_chplan.a

chplan_data.o: $(CHPLAN_DATA_SRCS)
	$(CC) $(DATA_CFLAGS) -c -o $@ $(CORE)/rtw_chplan_rest.c

test-rest-beacon-hint: test_rest_beacon_hint
	./test_rest_beacon_hint chplan_rest_vectors.json

test_rest_beacon_hint: test_rest_beacon_hint.c $(CHPLAN_REST_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_CHPLAN_REST_ORACLE_BUILD -o $@ test_rest_beacon_hint.c \
		$(CHPLAN_REST_ORACLE) $(HOST_JSON) ../shim/host_chplan_rf.c

librust_chplan_rest.a: $(RUST_CHPLAN_REST)
	$(RUSTC) $(RUSTFLAGS) --cfg host_chplan_rest_test --crate-type staticlib \
		$(RUST_CHPLAN_REST) -o $@

test-rest-beacon-hint-rust: test_rest_beacon_hint_rust
	./test_rest_beacon_hint_rust chplan_rest_vectors.json

test_rest_beacon_hint_rust: test_rest_beacon_hint.c librust_chplan_rest.a $(HOST_JSON) ../shim/host_chplan_rf.c
	$(CC) $(CFLAGS) -DRUST_CHPLAN_REST_ORACLE -o $@ test_rest_beacon_hint.c \
		$(HOST_JSON) ../shim/host_chplan_rf.c librust_chplan_rest.a

clean:
	rm -f test_chplan test_chplan_rust librust_chplan.a chplan_data.o \
		test_rest_beacon_hint test_rest_beacon_hint_rust librust_chplan_rest.a
