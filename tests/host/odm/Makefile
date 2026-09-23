ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SRC := $(CORE)/rtw_odm_phydm_init.c
SHIM := host_odm_phydm_shim.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(CORE) \
	-include $(HOST_INC)/host_autoconf.h -DHOST_ODM_PHYDM_INIT_TEST

ADAPT_ORACLE := odm_adaptivity_c_oracle.c
ADAPT_SHIM := host_odm_adaptivity_shim.c
ADAPT_CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) \
	-include $(HOST_INC)/host_autoconf.h

.PHONY: all test test-adaptivity clean
all: test

test: test_odm_phydm_init
	./test_odm_phydm_init odm_phydm_init_vectors.json

test-adaptivity: test_odm_adaptivity_leaf
	./test_odm_adaptivity_leaf

test_odm_phydm_init: test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)

test_odm_adaptivity_leaf: test_odm_adaptivity_leaf.c $(ADAPT_ORACLE) $(ADAPT_SHIM)
	$(CC) $(ADAPT_CFLAGS) -o $@ test_odm_adaptivity_leaf.c $(ADAPT_ORACLE) $(ADAPT_SHIM)

clean:
	rm -f test_odm_phydm_init test_odm_adaptivity_leaf
