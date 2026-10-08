# Host L2 differential harness for core/rtw_wapi_sms4.c SMS4 block helpers (T17).

ROOT := ../../..
HOST_INC := ../include
ORACLE := wapi_sms4_c_oracle.c

CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DWAPI_SMS4_FULL_VECTORS

.PHONY: all test test-c clean

all: test

test: test-c

test-c: test_wapi_sms4
	./test_wapi_sms4

test_wapi_sms4: test_wapi_sms4.c $(ORACLE)
	$(CC) $(CFLAGS) -o $@ test_wapi_sms4.c $(ORACLE)

clean:
	rm -f test_wapi_sms4
