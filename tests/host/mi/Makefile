ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_MI := $(ROOT)/rust/rtw_mi.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

CORE := $(ROOT)/core
MI_STATUS_SRC := $(CORE)/rtw_mi_status.c
MI_STATUS_AUX_SRC := $(CORE)/rtw_mi_status_aux.c
MI_STATUS_CHECK_SRC := $(CORE)/rtw_mi_status_check.c
MI_STATUS_CFLAGS := $(CFLAGS) -I$(CORE) -I$(ROOT)/include \
	-include $(HOST_INC)/host_autoconf.h \
	-DHOST_MI_STATUS_TEST -DCONFIG_IOCTL_CFG80211 -DCONFIG_TDLS -DCONFIG_RTW_MESH
MI_STATUS_HOST_OBJS := rtw_mi_status_host.o rtw_mi_status_aux_host.o rtw_mi_status_check_host.o

.PHONY: all test test-c test-rust test-netif test-netif-c test-netif-rust \
	test-status test-status-c test-status-rust clean
all: test

test: test-c test-rust test-netif test-status test-status-rust

test-c: test_mi_ch_union
	./test_mi_ch_union mi_ch_union_vectors.json

test-rust: test_mi_ch_union_rust
	./test_mi_ch_union_rust mi_ch_union_vectors.json

test-netif: test-netif-c test-netif-rust

test-netif-c: test_mi_netif_buddy
	./test_mi_netif_buddy mi_netif_buddy_vectors.json

test-netif-rust: test_mi_netif_buddy_rust
	./test_mi_netif_buddy_rust mi_netif_buddy_vectors.json

test-status: test-status-c test-status-rust

test-status-c: test_mi_status
	./test_mi_status mi_status_vectors.json

test-status-rust: test_mi_status_rust
	./test_mi_status_rust mi_status_vectors.json

rtw_mi_status_host.o: $(MI_STATUS_SRC)
	$(CC) $(MI_STATUS_CFLAGS) -c $(MI_STATUS_SRC) -o $@

rtw_mi_status_aux_host.o: $(MI_STATUS_AUX_SRC)
	$(CC) $(MI_STATUS_CFLAGS) -c $(MI_STATUS_AUX_SRC) -o $@

rtw_mi_status_check_host.o: $(MI_STATUS_CHECK_SRC)
	$(CC) $(MI_STATUS_CFLAGS) -c $(MI_STATUS_CHECK_SRC) -o $@

test_mi_status: mi_status_l2.c $(MI_STATUS_HOST_OBJS) $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(CORE) -I$(ROOT)/include -o $@ mi_status_l2.c \
		$(MI_STATUS_HOST_OBJS) $(HOST_JSON)

librust_mi_status.a: $(RUST_MI) $(ROOT)/rust/rtw_mi_status_host.rs
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mi_status_test \
		--crate-type staticlib $(RUST_MI) -o $@

# Link aux/check C objects only to satisfy symbols in unused op branches; Rust provides by_ifbmp.
test_mi_status_rust: mi_status_l2.c librust_mi_status.a rtw_mi_status_aux_host.o \
		rtw_mi_status_check_host.o $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_MI_STATUS_RUST -I$(CORE) -I$(ROOT)/include -o $@ mi_status_l2.c \
		rtw_mi_status_aux_host.o rtw_mi_status_check_host.o $(HOST_JSON) librust_mi_status.a

test_mi_ch_union: mi_ch_union_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ mi_ch_union_l2.c $(HOST_JSON)

test_mi_netif_buddy: mi_netif_buddy_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ mi_netif_buddy_l2.c $(HOST_JSON)

test_mi_netif_buddy_rust: mi_netif_buddy_l2.c librust_mi_netif.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_MI_RUST -o $@ mi_netif_buddy_l2.c $(HOST_JSON) librust_mi_netif.a

librust_mi.a: $(RUST_MI)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mi_ch_union_test \
		--crate-type staticlib $(RUST_MI) -o $@

librust_mi_netif.a: $(RUST_MI)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mi_netif_buddy_test \
		--crate-type staticlib $(RUST_MI) -o $@

test_mi_ch_union_rust: mi_ch_union_l2.c librust_mi.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_MI_RUST -o $@ mi_ch_union_l2.c $(HOST_JSON) librust_mi.a

clean:
	rm -f test_mi_ch_union test_mi_ch_union_rust test_mi_netif_buddy \
		test_mi_netif_buddy_rust test_mi_status test_mi_status_rust \
		$(MI_STATUS_HOST_OBJS) librust_mi.a librust_mi_netif.a librust_mi_status.a
