ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
CMD_PRIV_SRC := $(CORE)/rtw_cmd_priv.c
HOST_JSON := ../crypto/host_vector_json.c

CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(CORE) -include $(HOST_INC)/host_autoconf.h \
          -DHOST_CMD_PRIV_TEST

.PHONY: all test clean
all: test
test: test_cmd_priv
	./test_cmd_priv cmd_priv_vectors.json
test_cmd_priv: test_cmd_priv.c host_cmd_priv_shim.c $(CMD_PRIV_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_cmd_priv.c host_cmd_priv_shim.c $(CMD_PRIV_SRC) $(HOST_JSON)
clean:
	rm -f test_cmd_priv
