ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := sdio_cmd_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -include $(HOST_INC)/host_autoconf.h

.PHONY: all test clean
all: test

test: test_sdio_cmd
	./test_sdio_cmd sdio_cmd_vectors.json

test_sdio_cmd: test_sdio_cmd.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_sdio_cmd.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_sdio_cmd
