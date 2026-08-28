# Host L2 differential tests for core/rtw_mlme_rest.c (W3-53).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
MLME_REST_SRC := $(CORE)/rtw_mlme_rest.c
MLME_SHIM := host_mlme_shim.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_MLME_REST := $(ROOT)/rust/rtw_mlme_rest.rs

CC     := gcc
RUSTC  := rustc
CFLAGS_COMMON := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h
CFLAGS := $(CFLAGS_COMMON) -DHOST_MLME_TEST -DCONFIG_P2P

.PHONY: all test test-c test-rust test-mlme-unassoc test-mlme-wmm-rsn clean

all: test

test: test-c test-rust test-mlme-unassoc test-mlme-wmm-rsn

test-c: test_mlme
	./test_mlme mlme_vectors.json

test_mlme: test_mlme.c $(MLME_REST_SRC) $(MLME_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mlme.c $(MLME_REST_SRC) $(MLME_SHIM) $(HOST_JSON)

librust_mlme_rest.a: $(RUST_MLME_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mlme_test --cfg config_p2p \
		--crate-type staticlib $(RUST_MLME_REST) -o $@

test-rust: test_mlme_rust
	./test_mlme_rust mlme_vectors.json

test_mlme_rust: test_mlme.c librust_mlme_rest.a $(MLME_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_MLME_ORACLE -o $@ test_mlme.c $(MLME_SHIM) $(HOST_JSON) librust_mlme_rest.a

CFLAGS_UNASSOC := $(CFLAGS_COMMON) -DCONFIG_RTW_MULTI_AP -DHOST_MLME_UNASSOC_TEST

test-mlme-unassoc: test_mlme_unassoc
	./test_mlme_unassoc mlme_unassoc_vectors.json

test_mlme_unassoc: test_mlme_unassoc.c $(MLME_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS_UNASSOC) -o $@ test_mlme_unassoc.c $(MLME_REST_SRC) $(HOST_JSON)

CFLAGS_WMM_RSN := $(CFLAGS_COMMON) -DHOST_MLME_WMM_RSN_TEST -DCONFIG_WMMPS_STA

test-mlme-wmm-rsn: test_mlme_wmm_rsn
	./test_mlme_wmm_rsn mlme_wmm_rsn_vectors.json

test_mlme_wmm_rsn: test_mlme_wmm_rsn.c $(MLME_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS_WMM_RSN) -o $@ test_mlme_wmm_rsn.c $(MLME_REST_SRC) $(HOST_JSON)

clean:
	rm -f test_mlme test_mlme_rust test_mlme_unassoc test_mlme_wmm_rsn librust_mlme_rest.a
