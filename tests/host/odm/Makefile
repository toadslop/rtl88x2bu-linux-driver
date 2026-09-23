ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SRC := $(CORE)/rtw_odm_phydm_init.c
SHIM := host_odm_phydm_shim.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(CORE) \
	-include $(HOST_INC)/host_autoconf.h -DHOST_ODM_PHYDM_INIT_TEST

.PHONY: all test clean
all: test

test: test_odm_phydm_init
	./test_odm_phydm_init odm_phydm_init_vectors.json

test_odm_phydm_init: test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)

clean:
	rm -f test_odm_phydm_init
