ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
BITBANG := ../eeprom_bitbang/eeprom_bitbang_c_oracle.c
ORACLE := eeprom_rw_c_oracle.c
RUST_EEPROM := $(ROOT)/rust/rtw_eeprom.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -include $(HOST_INC)/host_autoconf.h

.PHONY: all test test-c test-rust clean
all: test
test: test-c test-rust

test-c: test_eeprom_rw
	./test_eeprom_rw eeprom_rw_vectors.json

test_eeprom_rw: test_eeprom_rw.c $(ORACLE) $(BITBANG) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_eeprom_rw.c $(ORACLE) $(BITBANG) $(HOST_JSON)

librust_eeprom_rw.a: $(RUST_EEPROM)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_eeprom_rw_test --crate-type staticlib $(RUST_EEPROM) -o $@

test-rust: test_eeprom_rw_rust
	./test_eeprom_rw_rust eeprom_rw_vectors.json

test_eeprom_rw_rust: test_eeprom_rw.c librust_eeprom_rw.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_eeprom_rw.c $(HOST_JSON) librust_eeprom_rw.a -lm -ldl -pthread

clean:
	rm -f test_eeprom_rw test_eeprom_rw_rust librust_eeprom_rw.a
