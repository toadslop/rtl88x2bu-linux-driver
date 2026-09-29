ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c clean
all: test

test: test-c

test-c: test_btcoex_init_notify
	./test_btcoex_init_notify btcoex_init_notify_vectors.json

test_btcoex_init_notify: btcoex_init_notify_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ btcoex_init_notify_l2.c $(HOST_JSON)

clean:
	rm -f test_btcoex_init_notify test_btcoex_init_notify_rust librust_btcoex_init_notify.a
