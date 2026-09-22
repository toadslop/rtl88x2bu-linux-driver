ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM := ft_ie_shim.c
STUBS := ft_ie_stubs.c
ORACLE := ft_ie_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include \
	-DCONFIG_RTW_BTM_ROAM

.PHONY: all test clean

all: test

test: test_ft_ie
	./test_ft_ie ft_ie_vectors.json

test_ft_ie: test_ft_ie.c $(SHIM) $(STUBS) $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ft_ie.c $(SHIM) $(STUBS) $(ORACLE) $(HOST_JSON)

clean:
	rm -f test_ft_ie
