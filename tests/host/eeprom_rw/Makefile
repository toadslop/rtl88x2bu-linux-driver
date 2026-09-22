ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
BITBANG := ../eeprom_bitbang/eeprom_bitbang_c_oracle.c
ORACLE := eeprom_rw_c_oracle.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -include $(HOST_INC)/host_autoconf.h

.PHONY: all test test-c clean
all: test
test: test-c

test-c: test_eeprom_rw
	./test_eeprom_rw eeprom_rw_vectors.json

test_eeprom_rw: test_eeprom_rw.c $(ORACLE) $(BITBANG) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_eeprom_rw.c $(ORACLE) $(BITBANG) $(HOST_JSON)

clean:
	rm -f test_eeprom_rw
