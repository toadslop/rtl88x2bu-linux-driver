ROOT := ../../..
CORE := $(ROOT)/core
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SRC := $(CORE)/rtw_odm_phydm_init.c
SHIM := host_odm_phydm_shim.c
ADAPT_SRC := $(CORE)/rtw_odm_adaptivity_leaf.c
ADAPT_SHIM := host_odm_adaptivity_shim.c
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(CORE) \
	-include $(HOST_INC)/host_autoconf.h -DHOST_ODM_PHYDM_INIT_TEST
ADAPT_CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(CORE) \
	-include $(HOST_INC)/host_autoconf.h -DHOST_ODM_ADAPTIVITY_TEST
RADAR_CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(CORE) \
	-include $(HOST_INC)/host_autoconf.h -DHOST_ODM_RADAR_TXPWR_TEST \
	-DCONFIG_DFS_MASTER
RUST_RADAR := $(ROOT)/rust/rtw_odm_radar_txpwr_host.rs
RUSTC := rustc

.PHONY: all test test-adaptivity test-radar test-radar-rust clean
all: test

test: test_odm_phydm_init test_odm_adaptivity_leaf test-radar

test-radar: test_odm_radar_txpwr_leaf test_odm_radar_txpwr_rust
	./test_odm_radar_txpwr_leaf odm_radar_txpwr_leaf_vectors.json
	./test_odm_radar_txpwr_rust odm_radar_txpwr_leaf_vectors.json

test-adaptivity: test_odm_adaptivity_leaf
	./test_odm_adaptivity_leaf odm_adaptivity_leaf_vectors.json

test_odm_phydm_init: test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_odm_phydm_init.c $(SRC) $(SHIM) $(HOST_JSON)

test_odm_adaptivity_leaf: test_odm_adaptivity_leaf.c $(ADAPT_SRC) $(ADAPT_SHIM) $(HOST_JSON)
	$(CC) $(ADAPT_CFLAGS) -o $@ test_odm_adaptivity_leaf.c $(ADAPT_SRC) $(ADAPT_SHIM) $(HOST_JSON)

test_odm_radar_txpwr_leaf: test_odm_radar_txpwr_leaf.c $(HOST_JSON)
	$(CC) $(RADAR_CFLAGS) -o $@ test_odm_radar_txpwr_leaf.c $(HOST_JSON)

librust_odm_radar.a: $(RUST_RADAR)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --crate-type staticlib $(RUST_RADAR) -o $@

test_odm_radar_txpwr_rust: test_odm_radar_txpwr_leaf.c librust_odm_radar.a $(HOST_JSON)
	$(CC) $(RADAR_CFLAGS) -DHOST_ODM_RADAR_RUST -o $@ test_odm_radar_txpwr_leaf.c $(HOST_JSON) librust_odm_radar.a

clean:
	rm -f test_odm_phydm_init test_odm_adaptivity_leaf test_odm_radar_txpwr_leaf \
		test_odm_radar_txpwr_rust librust_odm_radar.a
