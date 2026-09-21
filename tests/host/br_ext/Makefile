ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
LEAF := host_br_ext_nat25_leaf.c
REST := host_br_ext_nat25_rest.c
TLV := host_br_ext_tlv_skb.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-rest test-tlv clean
all: test test-rest test-tlv

test: test_nat25_leaf
	./test_nat25_leaf nat25_leaf_vectors.json

test-rest: test_nat25_rest
	./test_nat25_rest nat25_rest_vectors.json

test-tlv: test_nat25_tlv
	./test_nat25_tlv nat25_tlv_vectors.json

test_nat25_leaf: test_nat25_leaf.c $(LEAF) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_leaf.c $(LEAF) $(HOST_JSON)

test_nat25_rest: test_nat25_rest.c $(LEAF) $(REST) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_rest.c $(LEAF) $(REST) $(HOST_JSON)

test_nat25_tlv: test_nat25_tlv.c $(TLV) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_nat25_tlv.c $(TLV) $(HOST_JSON)

clean:
	rm -f test_nat25_leaf test_nat25_rest test_nat25_tlv
