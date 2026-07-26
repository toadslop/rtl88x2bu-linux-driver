# Host L2 differential tests for core/rtw_security.c helpers (T5).
# Builds with gcc only — no kernel headers or KDIR.

ROOT     := ../../..
HOST_INC := ../include
ORACLE   := security_c_oracle.c

CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I. \
           -DCONFIG_IEEE80211W
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-wep test-tkip clean

all: test

test: test-c test-wep test-tkip

test-c: test_security
	./test_security security_type_vectors.json

test-wep: test_wep_primitives
	./test_wep_primitives wep_primitive_vectors.json

test-tkip: test_tkip_mic
	./test_tkip_mic tkip_mic_vectors.json

test_security: test_security.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_security.c $(ORACLE) $(HOST_JSON)

test_wep_primitives: test_wep_primitives.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wep_primitives.c $(ORACLE) $(HOST_JSON)

test_tkip_mic: test_tkip_mic.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_tkip_mic.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_security test_wep_primitives test_tkip_mic
