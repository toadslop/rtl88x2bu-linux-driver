# Host L2 differential tests for core/rtw_ieee80211.c IE helpers (W3-03)
# and core/rtw_ieee80211_rest.c rate helpers (W3-26).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
WLAN_UTIL := ../wlan_util
RUST_IE  := $(ROOT)/rust/rtw_ieee80211.rs
IE_REST_SRC := $(CORE)/rtw_ieee80211_rest.c

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I$(CORE) -I$(WLAN_UTIL) \
           -DHOST_IE_TEST
REST_CFLAGS := $(CFLAGS) -DHOST_IEEE80211_REST_TEST
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on

IE_SRCS := ie_c_oracle.c ../crypto/host_vector_json.c
IE_REST_SRCS := $(IE_REST_SRC) ie_c_oracle.c $(WLAN_UTIL)/wlan_util_c_oracle.c \
                ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust test-rest clean

all: test

test: test-c test-rust test-rest

test-c: test_ie
	./test_ie ie_vectors.json

test_ie: test_ie.c $(IE_SRCS)
	$(CC) $(CFLAGS) -o $@ test_ie.c $(IE_SRCS)

librust_ie.a: $(RUST_IE)
	$(RUSTC) $(RUSTFLAGS) --cfg host_ie_test --crate-type staticlib \
		$(RUST_IE) -o $@

test-rust: test_ie_rust
	./test_ie_rust ie_vectors.json

test_ie_rust: test_ie.c librust_ie.a ../crypto/host_vector_json.c
	$(CC) $(CFLAGS) -DRUST_IE_ORACLE -o $@ test_ie.c ../crypto/host_vector_json.c \
		librust_ie.a

test-rest: test_ie_rest
	./test_ie_rest ie_rest_vectors.json

test_ie_rest: test_ie_rest.c $(IE_REST_SRCS)
	$(CC) $(REST_CFLAGS) -o $@ test_ie_rest.c $(IE_REST_SRCS)

clean:
	rm -f test_ie test_ie_rust test_ie_rest librust_ie.a
