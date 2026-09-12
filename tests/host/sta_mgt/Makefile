# Host L2 differential tests for core/rtw_sta_mgt_rest.c (W3-37).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
STA_REST_SRC := $(CORE)/rtw_sta_mgt_rest.c
STCTL_SRC := $(CORE)/rtw_sta_mgt_stctl.c
LOOKUP_SRC := $(CORE)/rtw_sta_mgt_lookup.c
ALLOC_SRC := $(CORE)/rtw_sta_mgt_alloc.c
FREE_SRC := $(CORE)/rtw_sta_mgt_free.c
HOST_JSON := ../crypto/host_vector_json.c
HOST_SHIM := host_sta_mgt_shim.c
STCTL_SHIM := host_sta_mgt_stctl_shim.c
LOOKUP_SHIM := host_sta_mgt_lookup_shim.c
ALLOC_SHIM := host_sta_mgt_alloc_shim.c
FREE_SHIM := host_sta_mgt_free_shim.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-sign-compare -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_STA_MGT_TEST -DCONFIG_RTW_MACADDR_ACL -DCONFIG_RTW_PRE_LINK_STA

.PHONY: all test test-c test-rust sta_mgt-compile clean

all: test

# PR4: W3-38 AID/pre-link L2 vectors (W3-37 ACL vectors retained).
sta_mgt-compile: $(STA_REST_SRC) $(HOST_SHIM)
	$(CC) $(CFLAGS) -c $(STA_REST_SRC) -o sta_mgt_rest_host.o
	$(CC) $(CFLAGS) -c $(HOST_SHIM) -o sta_mgt_shim_host.o

test: test-c test-rust

test-c: test_sta_mgt test_sta_mgt_aid test_sta_mgt_stctl test_sta_mgt_lookup test_sta_mgt_alloc test_sta_mgt_free
	./test_sta_mgt sta_mgt_vectors.json
	./test_sta_mgt_aid sta_mgt_aid_vectors.json
	./test_sta_mgt_stctl sta_mgt_stctl_vectors.json
	./test_sta_mgt_lookup sta_mgt_lookup_vectors.json
	./test_sta_mgt_alloc sta_mgt_alloc_vectors.json
	./test_sta_mgt_free sta_mgt_free_vectors.json

test_sta_mgt: test_sta_mgt.c $(STA_REST_SRC) $(HOST_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sta_mgt.c $(HOST_SHIM) $(STA_REST_SRC) $(HOST_JSON)

test_sta_mgt_aid: test_sta_mgt_aid.c $(STA_REST_SRC) $(HOST_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sta_mgt_aid.c $(HOST_SHIM) $(STA_REST_SRC) $(HOST_JSON)

test_sta_mgt_stctl: test_sta_mgt_stctl.c $(STCTL_SRC) $(STA_REST_SRC) $(HOST_SHIM) $(STCTL_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sta_mgt_stctl.c $(HOST_SHIM) $(STCTL_SHIM) $(STCTL_SRC) $(STA_REST_SRC) $(HOST_JSON)

test_sta_mgt_lookup: test_sta_mgt_lookup.c $(LOOKUP_SRC) $(STCTL_SRC) $(LOOKUP_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sta_mgt_lookup.c $(LOOKUP_SHIM) $(STCTL_SRC) $(LOOKUP_SRC) $(HOST_JSON)

test_sta_mgt_alloc: test_sta_mgt_alloc.c $(ALLOC_SRC) $(LOOKUP_SRC) $(STCTL_SRC) $(LOOKUP_SHIM) $(ALLOC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sta_mgt_alloc.c $(ALLOC_SHIM) $(LOOKUP_SHIM) $(STCTL_SRC) $(LOOKUP_SRC) $(ALLOC_SRC) $(HOST_JSON)

test_sta_mgt_free: test_sta_mgt_free.c $(FREE_SRC) $(ALLOC_SRC) $(LOOKUP_SRC) $(STCTL_SRC) $(FREE_SHIM) $(ALLOC_SHIM) $(LOOKUP_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sta_mgt_free.c $(FREE_SHIM) $(ALLOC_SHIM) $(LOOKUP_SHIM) $(STCTL_SRC) $(LOOKUP_SRC) $(ALLOC_SRC) $(FREE_SRC) $(HOST_JSON)

RUST_STA_MGT := $(ROOT)/rust/rtw_sta_mgt.rs

librust_sta_mgt.a: $(RUST_STA_MGT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--crate-type staticlib $(RUST_STA_MGT) -o $@

test-rust: test_sta_mgt_rust test_sta_mgt_aid_rust test_sta_mgt_stctl_rust test_sta_mgt_alloc_rust
	./test_sta_mgt_rust sta_mgt_vectors.json
	./test_sta_mgt_aid_rust sta_mgt_aid_vectors.json
	./test_sta_mgt_stctl_rust sta_mgt_stctl_vectors.json
	./test_sta_mgt_alloc_rust sta_mgt_alloc_vectors.json

RUST_STA_MGT_STCTL := $(ROOT)/rust/rtw_sta_mgt_stctl.rs
RUST_STA_MGT_AID := $(ROOT)/rust/rtw_sta_mgt_aid.rs
RUST_STA_MGT_ALLOC := $(ROOT)/rust/rtw_sta_mgt_alloc.rs

librust_sta_mgt_stctl.a: $(RUST_STA_MGT_STCTL)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--crate-type staticlib $(RUST_STA_MGT_STCTL) -o $@

librust_sta_mgt_aid.a: $(RUST_STA_MGT_AID)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--crate-type staticlib $(RUST_STA_MGT_AID) -o $@

test_sta_mgt_aid_rust: test_sta_mgt_aid.c librust_sta_mgt_aid.a $(HOST_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_STA_MGT_ORACLE -o $@ test_sta_mgt_aid.c $(HOST_SHIM) $(HOST_JSON) librust_sta_mgt_aid.a

test_sta_mgt_rust: test_sta_mgt.c librust_sta_mgt.a $(HOST_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_STA_MGT_ORACLE -o $@ test_sta_mgt.c $(HOST_SHIM) $(HOST_JSON) librust_sta_mgt.a

test_sta_mgt_stctl_rust: test_sta_mgt_stctl.c librust_sta_mgt_stctl.a $(HOST_SHIM) $(STCTL_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_STA_MGT_STCTL_ORACLE -o $@ test_sta_mgt_stctl.c $(HOST_SHIM) $(STCTL_SHIM) $(HOST_JSON) librust_sta_mgt_stctl.a

librust_sta_mgt_alloc.a: $(RUST_STA_MGT_ALLOC)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_sta_mgt_test \
		--crate-type staticlib $(RUST_STA_MGT_ALLOC) -o $@

test_sta_mgt_alloc_rust: test_sta_mgt_alloc.c librust_sta_mgt_alloc.a $(ALLOC_SHIM) $(LOOKUP_SHIM) $(STCTL_SRC) $(LOOKUP_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_STA_MGT_ALLOC_ORACLE -o $@ test_sta_mgt_alloc.c $(ALLOC_SHIM) $(LOOKUP_SHIM) $(STCTL_SRC) $(LOOKUP_SRC) $(HOST_JSON) librust_sta_mgt_alloc.a

clean:
	rm -f test_sta_mgt test_sta_mgt_aid test_sta_mgt_stctl test_sta_mgt_lookup test_sta_mgt_alloc test_sta_mgt_free \
		test_sta_mgt_rust test_sta_mgt_aid_rust test_sta_mgt_stctl_rust test_sta_mgt_alloc_rust \
		librust_sta_mgt.a librust_sta_mgt_aid.a librust_sta_mgt_stctl.a librust_sta_mgt_alloc.a \
		sta_mgt_rest_host.o sta_mgt_shim_host.o
