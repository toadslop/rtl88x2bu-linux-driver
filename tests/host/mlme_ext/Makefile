# Host L2 differential tests for core/rtw_mlme_ext_rest.c (W3-54 chset, W3-68 mgnt attrib).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
MLME_EXT_REST_SRC := $(CORE)/rtw_mlme_ext_rest.c
RF_REST_SRC := $(CORE)/rtw_rf_rest.c
RUST_MLME_EXT_REST := $(ROOT)/rust/rtw_mlme_ext_rest.rs
RUST_MLME_EXT_MGNT_ATTRIB := $(ROOT)/rust/rtw_mlme_ext_mgnt_attrib.rs
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_mlme_ext_shim.c
MGNT_SHIM := host_mlme_ext_mgnt_attrib_shim.c

MGNT_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_MGNT_ATTRIB_TEST -DCONFIG_RTW_MGMT_QUEUE -DCONFIG_BEAMFORMING

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_TEST -DHOST_RF_TEST -DCONFIG_DFS_MASTER

.PHONY: all test test-c test-rust test-mgnt-attrib test-mgnt-attrib-rust clean

all: test

test: test-c test-rust test-mgnt-attrib
ifneq ($(wildcard $(RUST_MLME_EXT_MGNT_ATTRIB)),)
test: test-mgnt-attrib-rust
endif

test-c: test_chset
	./test_chset chset_vectors.json

test_chset: test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) \
		$(RF_REST_SRC) $(HOST_JSON)

librust_mlme_ext_rest.a: $(RUST_MLME_EXT_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_test --cfg dfs_master \
		--crate-type staticlib $(RUST_MLME_EXT_REST) -o $@

test-rust: test_chset_rust
	./test_chset_rust chset_vectors.json

test_chset_rust: test_chset.c librust_mlme_ext_rest.a $(SHIM) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_MLME_EXT_REST_ORACLE -o $@ test_chset.c $(SHIM) \
		$(RF_REST_SRC) $(HOST_JSON) librust_mlme_ext_rest.a

test-mgnt-attrib: test_mgnt_attrib
	./test_mgnt_attrib mgnt_attrib_vectors.json

test_mgnt_attrib: test_mgnt_attrib.c $(MLME_EXT_REST_SRC) $(MGNT_SHIM) $(HOST_JSON)
	$(CC) $(MGNT_CFLAGS) -o $@ test_mgnt_attrib.c $(MLME_EXT_REST_SRC) $(MGNT_SHIM) \
		$(HOST_JSON)

librust_mlme_ext_mgnt_attrib.a: $(RUST_MLME_EXT_MGNT_ATTRIB)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_mgnt_attrib_test --cfg config_rtw_mgmt_queue \
		--cfg config_beamforming --cfg config_80211ac_vht \
		--crate-type staticlib $(RUST_MLME_EXT_MGNT_ATTRIB) -o $@

test-mgnt-attrib-rust: test_mgnt_attrib_rust
	./test_mgnt_attrib_rust mgnt_attrib_vectors.json

test_mgnt_attrib_rust: test_mgnt_attrib.c librust_mlme_ext_mgnt_attrib.a $(MGNT_SHIM) $(HOST_JSON)
	$(CC) $(MGNT_CFLAGS) -DRUST_MLME_EXT_MGNT_ATTRIB_ORACLE -o $@ test_mgnt_attrib.c \
		$(MGNT_SHIM) $(HOST_JSON) librust_mlme_ext_mgnt_attrib.a

clean:
	rm -f test_chset test_chset_rust librust_mlme_ext_rest.a \
		test_mgnt_attrib test_mgnt_attrib_rust librust_mlme_ext_mgnt_attrib.a
