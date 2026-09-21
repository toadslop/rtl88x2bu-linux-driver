# Host L2 differential tests for core/rtw_rson.c leaf helpers (W3-106).

ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DCONFIG_RTW_REPEATER_SON

CORE_ORACLE := rson_core_c_oracle.c host_rson_util.c
IE_ORACLE := rson_score_c_oracle.c rson_core_c_oracle.c rson_ie_c_oracle.c host_rson_util.c

.PHONY: all test test-score test-core test-ie test-choose clean

all: test

test: test-score test-core test-ie test-choose

test-score: test_rson_score
	./test_rson_score rson_score_vectors.json

test-core: test_rson_core
	./test_rson_core rson_core_vectors.json

test_rson_score: test_rson_score.c rson_score_c_oracle.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_score.c rson_score_c_oracle.c $(HOST_JSON)

test_rson_core: test_rson_core.c $(CORE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_core.c $(CORE_ORACLE) $(HOST_JSON)

test-ie: test_rson_ie
	./test_rson_ie rson_ie_vectors.json

test_rson_ie: test_rson_ie.c $(IE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_ie.c $(IE_ORACLE) $(HOST_JSON)

test-choose: test_rson_choose
	./test_rson_choose rson_choose_vectors.json

test_rson_choose: test_rson_choose.c $(IE_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rson_choose.c $(IE_ORACLE) $(HOST_JSON)

clean:
	rm -f test_rson_score test_rson_core test_rson_ie test_rson_choose
