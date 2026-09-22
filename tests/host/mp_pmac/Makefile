ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
	-I$(HOST_INC) -DHOST_MP_PMAC_TEST

.PHONY: all test test-c clean
all: test
test: test-c
test-c: test_mp_pmac
	./test_mp_pmac mp_pmac_vectors.json

test_mp_pmac: test_mp_pmac.c mp_pmac_stubs.c mp_pmac_bit_crc_oracle.c mp_pmac_cck_oracle.c mp_pmac_param_oracle.c mp_pmac_sig_oracle.c mp_pmac_ht_sig_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mp_pmac.c mp_pmac_stubs.c mp_pmac_bit_crc_oracle.c mp_pmac_cck_oracle.c mp_pmac_param_oracle.c mp_pmac_sig_oracle.c mp_pmac_ht_sig_oracle.c $(HOST_JSON) -lm

clean:
	rm -f test_mp_pmac
