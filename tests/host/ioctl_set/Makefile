ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c clean
all: test

test: test-c

test-c: test_ioctl_validate
	./test_ioctl_validate ioctl_validate_vectors.json

test_ioctl_validate: ioctl_validate_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ ioctl_validate_l2.c $(HOST_JSON)

clean:
	rm -f test_ioctl_validate test_ioctl_validate_rust librust_ioctl_validate.a
