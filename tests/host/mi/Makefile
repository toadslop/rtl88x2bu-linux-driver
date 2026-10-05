ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_MI := $(ROOT)/rust/rtw_mi.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

CORE := $(ROOT)/core
MI_STATUS_SRC := $(CORE)/rtw_mi_status.c
MI_STATUS_CFLAGS := $(CFLAGS) -I$(CORE) -I$(ROOT)/include \
	-include $(HOST_INC)/host_autoconf.h \
	-DHOST_MI_STATUS_TEST -DCONFIG_IOCTL_CFG80211 -DCONFIG_TDLS -DCONFIG_RTW_MESH

.PHONY: all test test-c test-rust test-netif test-netif-c test-netif-rust \
	test-status test-status-c clean
all: test

test: test-c test-rust test-netif test-status

test-c: test_mi_ch_union
	./test_mi_ch_union mi_ch_union_vectors.json

test-rust: test_mi_ch_union_rust
	./test_mi_ch_union_rust mi_ch_union_vectors.json

test-netif: test-netif-c test-netif-rust

test-netif-c: test_mi_netif_buddy
	./test_mi_netif_buddy mi_netif_buddy_vectors.json

test-netif-rust: test_mi_netif_buddy_rust
	./test_mi_netif_buddy_rust mi_netif_buddy_vectors.json

test-status: test-status-c

test-status-c: test_mi_status
	./test_mi_status mi_status_vectors.json

rtw_mi_status_host.o: $(MI_STATUS_SRC)
	$(CC) $(MI_STATUS_CFLAGS) -c $(MI_STATUS_SRC) -o $@

test_mi_status: mi_status_l2.c rtw_mi_status_host.o $(HOST_JSON)
	$(CC) $(CFLAGS) -I$(CORE) -I$(ROOT)/include -o $@ mi_status_l2.c \
		rtw_mi_status_host.o $(HOST_JSON)

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
		test_mi_netif_buddy_rust test_mi_status rtw_mi_status_host.o \
		librust_mi.a librust_mi_netif.a
