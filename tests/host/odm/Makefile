ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := odm_phydm_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) \
	-include $(HOST_INC)/host_autoconf.h

.PHONY: all test clean
all: test

test: test_odm_phydm_init
	./test_odm_phydm_init odm_phydm_init_vectors.json

test_odm_phydm_init: test_odm_phydm_init.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_odm_phydm_init
