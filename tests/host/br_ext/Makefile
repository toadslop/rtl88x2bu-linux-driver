ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
LEAF := host_br_ext_nat25_leaf.c
REST := host_br_ext_nat25_rest.c
TLV := host_br_ext_tlv_skb.c
DB := host_br_ext_nat25_db.c
RUST_BR := $(ROOT)/rust/rtw_br_ext.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-rest test-tlv test-db test-rust clean
all: test test-rest test-tlv test-db

test: test_nat25_leaf
	./test_nat25_leaf nat25_leaf_vectors.json

test-rest: test_nat25_rest
	./test_nat25_rest nat25_rest_vectors.json

test-tlv: test_nat25_tlv
	./test_nat25_tlv nat25_tlv_vectors.json

test-db: test_nat25_db
	./test_nat25_db nat25_db_vectors.json

test-rust: test_nat25_leaf_rust test_nat25_rest_rust test_nat25_tlv_rust test_nat25_db_rust
	./test_nat25_leaf_rust nat25_leaf_vectors.json
	./test_nat25_rest_rust nat25_rest_vectors.json
	./test_nat25_tlv_rust nat25_tlv_vectors.json
	./test_nat25_db_rust nat25_db_vectors.json

test_nat25_leaf: test_nat25_leaf.c $(LEAF) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_leaf.c $(LEAF) $(HOST_JSON)

test_nat25_rest: test_nat25_rest.c $(LEAF) $(REST) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_rest.c $(LEAF) $(REST) $(HOST_JSON)

test_nat25_tlv: test_nat25_tlv.c $(TLV) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_tlv.c $(TLV) $(HOST_JSON)

test_nat25_db: test_nat25_db.c $(LEAF) $(REST) $(DB) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_db.c $(LEAF) $(REST) $(DB) $(HOST_JSON)

librust_br_ext.a: $(RUST_BR)
	$(RUSTC) -C opt-level=2 --cfg host_br_ext_test --crate-type staticlib $(RUST_BR) -o $@

test_nat25_leaf_rust: test_nat25_leaf.c librust_br_ext.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_leaf.c $(HOST_JSON) librust_br_ext.a

test_nat25_rest_rust: test_nat25_rest.c librust_br_ext.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_rest.c $(HOST_JSON) librust_br_ext.a

test_nat25_tlv_rust: test_nat25_tlv.c librust_br_ext.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_tlv.c $(HOST_JSON) librust_br_ext.a

test_nat25_db_rust: test_nat25_db.c librust_br_ext.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_db.c $(HOST_JSON) librust_br_ext.a

clean:
	rm -f test_nat25_leaf test_nat25_rest test_nat25_tlv test_nat25_db \
		test_nat25_leaf_rust test_nat25_rest_rust test_nat25_tlv_rust \
		test_nat25_db_rust librust_br_ext.a
