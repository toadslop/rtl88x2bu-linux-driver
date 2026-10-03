# Host L2 differential tests for core/rtw_wapi.c PN/IE leaf (W3-109 PR1).

ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := wapi_pn_cam_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_wapi_pn_cam
	./test_wapi_pn_cam wapi_pn_cam_vectors.json

test_wapi_pn_cam: test_wapi_pn_cam.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wapi_pn_cam.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_wapi_pn_cam
