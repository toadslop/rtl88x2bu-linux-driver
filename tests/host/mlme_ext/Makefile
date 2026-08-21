# Host L2 differential tests for core/rtw_mlme_ext_rest.c (W3-54 chset).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
MLME_EXT_REST_SRC := $(CORE)/rtw_mlme_ext_rest.c
RF_REST_SRC := $(CORE)/rtw_rf_rest.c
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_mlme_ext_shim.c
CHPLAN_RF := ../shim/host_chplan_rf.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_TEST -DHOST_RF_TEST -DCONFIG_DFS_MASTER

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_chset
	./test_chset chset_vectors.json

test_chset: test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) \
		$(RF_REST_SRC) $(HOST_JSON)

clean:
	rm -f test_chset
