# Host L2 differential tests for core/rtw_rson.c leaf helpers (W3-106).

ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DCONFIG_RTW_REPEATER_SON

.PHONY: all test test-score clean

all: test

test: test-score

test-score: test_rson_score
	./test_rson_score rson_score_vectors.json

test_rson_score: test_rson_score.c rson_score_c_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_score.c rson_score_c_oracle.c $(HOST_JSON)

clean:
	rm -f test_rson_score
