ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
RM_PARSE_SRC := $(CORE)/rtw_rm_parse_rest.c
STUBS := rm_parse_stubs.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
	-I$(HOST_INC) -I$(CORE) -DHOST_RM_PARSE_TEST -DCONFIG_RTW_80211K

.PHONY: all test clean
all: test
test: test_rm_parse
	./test_rm_parse
test_rm_parse: test_rm_parse.c $(RM_PARSE_SRC) $(STUBS)
	$(CC) $(CFLAGS) -o $@ test_rm_parse.c $(RM_PARSE_SRC) $(STUBS)
clean:
	rm -f test_rm_parse
