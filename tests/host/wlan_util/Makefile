# Host L2 differential tests for core/rtw_wlan_util.c helpers (T5).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
HOST_INC := ../include
ORACLE   := wlan_util_c_oracle.c

CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I.
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_wlan_util
	./test_wlan_util rate_class_vectors.json

test_wlan_util: test_wlan_util.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wlan_util.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_wlan_util
