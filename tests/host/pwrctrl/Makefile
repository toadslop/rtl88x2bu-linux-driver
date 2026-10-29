# Host L2 differential tests for W3-94 ps deny gate helpers.

ROOT     := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
SHIM     := host_pwrctrl_ps_deny_shim.c
LPS_SHIM := host_pwrctrl_lps_shim.c
LEAVE_ALL_SHIM := host_pwrctrl_leave_all_ps_shim.c
LPS_ENTER_SHIM := host_pwrctrl_lps_enter_shim.c
IPS_SHIM := host_pwrctrl_ips_shim.c
PS_PROC_SHIM := host_pwrctrl_ps_processor_shim.c
LPS_TP_SHIM := host_pwrctrl_lps_tp_shim.c
LPS_EXEC_SHIM := host_pwrctrl_lps_exec_shim.c
RPWM_SHIM := host_pwrctrl_rpwm_shim.c
ALIVE_SHIM := host_pwrctrl_alive_shim.c
WOW_LPS_SHIM := host_pwrctrl_wow_lps_shim.c
PS_RDY_SHIM := host_pwrctrl_ps_rdy_shim.c
SET_PS_SHIM := host_pwrctrl_set_ps_shim.c

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -O2 \
          -I$(HOST_INC) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h
ALIVE_CFLAGS := $(CFLAGS) -DCONFIG_DETECT_CPWM_BY_POLLING
LPS_CFLAGS := $(CFLAGS)
LPS_ENTER_CFLAGS := $(CFLAGS)

RUST_PWRCTRL := $(ROOT)/rust/rtw_pwrctrl.rs
RUST_PWRCTRL_IPS_HOST := $(ROOT)/rust/rtw_pwrctrl_ips_host.rs
RUST_PWRCTRL_PS := $(ROOT)/rust/rtw_pwrctrl_ps.rs
RUST_PWRCTRL_LPS_TP := $(ROOT)/rust/rtw_pwrctrl_lps_tp.rs
RUST_PWRCTRL_LPS_EXEC := $(ROOT)/rust/rtw_pwrctrl_lps_exec.rs
RUST_PWRCTRL_RPWM := $(ROOT)/rust/rtw_pwrctrl_rpwm.rs
RUST_PWRCTRL_ALIVE := $(ROOT)/rust/rtw_pwrctrl_alive.rs
RUST_PWRCTRL_WOW := $(ROOT)/rust/rtw_pwrctrl_wow.rs
RUST_PWRCTRL_PS_RDY := $(ROOT)/rust/rtw_pwrctrl_ps_rdy.rs
RUST_PWRCTRL_SET_PS := $(ROOT)/rust/rtw_pwrctrl_set_ps.rs
RUSTC := rustc
PS_RDY_CFLAGS := $(CFLAGS) -DCONFIG_WOWLAN
SET_PS_CFLAGS := $(CFLAGS) -DCONFIG_BT_COEXIST -DCONFIG_FW_MULTI_PORT_SUPPORT -DCONFIG_LPS -DCONFIG_LPS_LCLK
SET_PS_PG_WOW_CFLAGS := $(SET_PS_CFLAGS) -DCONFIG_LPS_PG -DCONFIG_WOWLAN

.PHONY: all test test-c test-rust test-lps test-lps-rust test-lps-enter test-lps-enter-rust \
	test-leave-all test-leave-all-rust test-ips test-ips-rust test-ps-processor test-ps-processor-rust \
	test-lps-tp test-lps-tp-rust test-lps-exec test-lps-exec-rust test-rpwm test-rpwm-rust test-register-alive test-register-alive-rust \
	test-unregister-alive test-unregister-alive-rust test-wow-lps test-wow-lps-rust \
	test-ps-rdy test-ps-rdy-rust test-set-ps test-set-ps-pg-wow test-set-ps-rust test-set-ps-rust-pg-wow clean

all: test test-lps test-lps-enter test-leave-all test-leave-all-rust test-ips test-ips-rust \
	test-ps-processor test-ps-processor-rust test-lps-tp test-lps-tp-rust test-rpwm test-rpwm-rust \
	test-register-alive test-register-alive-rust test-unregister-alive test-unregister-alive-rust \
	test-wow-lps test-wow-lps-rust test-ps-rdy test-ps-rdy-rust test-set-ps test-set-ps-pg-wow test-set-ps-rust

test: test-c test-rust test-lps test-lps-rust test-lps-enter test-lps-enter-rust test-leave-all \
	test-leave-all-rust test-ips test-ips-rust test-ps-processor test-ps-processor-rust test-lps-tp \
	test-lps-tp-rust test-lps-exec test-rpwm test-rpwm-rust test-register-alive test-register-alive-rust \
	test-unregister-alive test-unregister-alive-rust test-wow-lps test-wow-lps-rust test-ps-rdy test-ps-rdy-rust \
	test-set-ps test-set-ps-pg-wow test-set-ps-rust test-set-ps-rust-pg-wow

test-c: test_pwrctrl_ps_deny
	./test_pwrctrl_ps_deny ps_deny_vectors.json

test-rust: test_pwrctrl_ps_deny_rust
	./test_pwrctrl_ps_deny_rust ps_deny_vectors.json

test-lps: test_pwrctrl_lps
	./test_pwrctrl_lps lps_vectors.json

test-lps-rust: test_pwrctrl_lps_rust
	./test_pwrctrl_lps_rust lps_vectors.json

test-lps-enter: test_pwrctrl_lps_enter
	./test_pwrctrl_lps_enter lps_enter_vectors.json

test-lps-enter-rust: test_pwrctrl_lps_enter_rust
	./test_pwrctrl_lps_enter_rust lps_enter_vectors.json

test-leave-all: test_pwrctrl_leave_all_ps
	./test_pwrctrl_leave_all_ps leave_all_ps_vectors.json

test-leave-all-rust: test_pwrctrl_leave_all_ps_rust
	./test_pwrctrl_leave_all_ps_rust leave_all_ps_vectors.json

test-ips: test_pwrctrl_ips
	./test_pwrctrl_ips ips_vectors.json

test-ips-rust: test_pwrctrl_ips_rust
	./test_pwrctrl_ips_rust ips_vectors.json

test-ps-processor: test_pwrctrl_ps_processor
	./test_pwrctrl_ps_processor ps_processor_vectors.json

test-ps-processor-rust: test_pwrctrl_ps_processor_rust
	./test_pwrctrl_ps_processor_rust ps_processor_vectors.json

test_pwrctrl_ps_processor: test_pwrctrl_ps_processor.c $(PS_PROC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_processor.c $(PS_PROC_SHIM) $(HOST_JSON)

librust_pwrctrl_ps.a: $(RUST_PWRCTRL_PS)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_ps_processor_test \
		--crate-type staticlib $(RUST_PWRCTRL_PS) -o $@

test_pwrctrl_ps_processor_rust: test_pwrctrl_ps_processor.c librust_pwrctrl_ps.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_processor.c $(HOST_JSON) librust_pwrctrl_ps.a

test-lps-tp: test_pwrctrl_lps_tp
	./test_pwrctrl_lps_tp lps_tp_vectors.json

test-lps-tp-rust: test_pwrctrl_lps_tp_rust
	./test_pwrctrl_lps_tp_rust lps_tp_vectors.json

test_pwrctrl_lps_tp: test_pwrctrl_lps_tp.c $(LPS_TP_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_lps_tp.c $(LPS_TP_SHIM) $(HOST_JSON)

librust_pwrctrl_lps_tp.a: $(RUST_PWRCTRL_LPS_TP)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_lps_tp_test \
		--crate-type staticlib $(RUST_PWRCTRL_LPS_TP) -o $@

test_pwrctrl_lps_tp_rust: test_pwrctrl_lps_tp.c librust_pwrctrl_lps_tp.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_lps_tp.c $(HOST_JSON) librust_pwrctrl_lps_tp.a

test-lps-exec: test_pwrctrl_lps_exec
	./test_pwrctrl_lps_exec lps_exec_vectors.json

test-lps-exec-rust: test_pwrctrl_lps_exec_rust
	./test_pwrctrl_lps_exec_rust lps_exec_vectors.json

test_pwrctrl_lps_exec: test_pwrctrl_lps_exec.c $(LPS_EXEC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_lps_exec.c $(LPS_EXEC_SHIM) $(HOST_JSON)

librust_pwrctrl_lps_exec.a: $(RUST_PWRCTRL_LPS_EXEC)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_lps_exec_test \
		--crate-type staticlib $(RUST_PWRCTRL_LPS_EXEC) -o $@

test_pwrctrl_lps_exec_rust: test_pwrctrl_lps_exec.c librust_pwrctrl_lps_exec.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_lps_exec.c $(HOST_JSON) librust_pwrctrl_lps_exec.a

test-rpwm: test_pwrctrl_rpwm
	./test_pwrctrl_rpwm rpwm_vectors.json

test-rpwm-rust: test_pwrctrl_rpwm_rust
	./test_pwrctrl_rpwm_rust rpwm_vectors.json

test_pwrctrl_rpwm: test_pwrctrl_rpwm.c $(RPWM_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_rpwm.c $(RPWM_SHIM) $(HOST_JSON)

librust_pwrctrl_rpwm.a: $(RUST_PWRCTRL_RPWM)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_rpwm_test \
		--crate-type staticlib $(RUST_PWRCTRL_RPWM) -o $@

test_pwrctrl_rpwm_rust: test_pwrctrl_rpwm.c librust_pwrctrl_rpwm.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_rpwm.c $(HOST_JSON) librust_pwrctrl_rpwm.a

test-register-alive: test_pwrctrl_register_alive
	./test_pwrctrl_register_alive register_alive_vectors.json

test_pwrctrl_register_alive: test_pwrctrl_register_alive.c $(ALIVE_SHIM) $(HOST_JSON)
	$(CC) $(ALIVE_CFLAGS) -o $@ test_pwrctrl_register_alive.c $(ALIVE_SHIM) $(HOST_JSON)

test-register-alive-rust: test_pwrctrl_register_alive_rust
	./test_pwrctrl_register_alive_rust register_alive_vectors.json

librust_pwrctrl_alive.a: $(RUST_PWRCTRL_ALIVE)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_alive_test \
		--cfg detect_cpwm_by_polling --crate-type staticlib $(RUST_PWRCTRL_ALIVE) -o $@

test_pwrctrl_register_alive_rust: test_pwrctrl_register_alive.c librust_pwrctrl_alive.a $(HOST_JSON)
	$(CC) $(ALIVE_CFLAGS) -o $@ test_pwrctrl_register_alive.c $(HOST_JSON) librust_pwrctrl_alive.a

test-unregister-alive: test_pwrctrl_unregister_alive
	./test_pwrctrl_unregister_alive unregister_alive_vectors.json

test_pwrctrl_unregister_alive: test_pwrctrl_unregister_alive.c $(ALIVE_SHIM) $(HOST_JSON)
	$(CC) $(ALIVE_CFLAGS) -o $@ test_pwrctrl_unregister_alive.c $(ALIVE_SHIM) $(HOST_JSON)

test-unregister-alive-rust: test_pwrctrl_unregister_alive_rust
	./test_pwrctrl_unregister_alive_rust unregister_alive_vectors.json

test_pwrctrl_unregister_alive_rust: test_pwrctrl_unregister_alive.c librust_pwrctrl_alive.a $(HOST_JSON)
	$(CC) $(ALIVE_CFLAGS) -o $@ test_pwrctrl_unregister_alive.c $(HOST_JSON) librust_pwrctrl_alive.a

test_pwrctrl_lps_enter: test_pwrctrl_lps_enter.c $(LPS_ENTER_SHIM) $(HOST_JSON)
	$(CC) $(LPS_ENTER_CFLAGS) -o $@ test_pwrctrl_lps_enter.c $(LPS_ENTER_SHIM) $(HOST_JSON)

test_pwrctrl_leave_all_ps_rust: test_pwrctrl_leave_all_ps.c librust_pwrctrl.a $(HOST_JSON)
	$(CC) $(LPS_CFLAGS) -o $@ test_pwrctrl_leave_all_ps.c $(HOST_JSON) librust_pwrctrl.a

test_pwrctrl_leave_all_ps: test_pwrctrl_leave_all_ps.c $(LEAVE_ALL_SHIM) $(HOST_JSON)
	$(CC) $(LPS_CFLAGS) -o $@ test_pwrctrl_leave_all_ps.c $(LEAVE_ALL_SHIM) $(HOST_JSON)

test_pwrctrl_lps: test_pwrctrl_lps.c $(LPS_SHIM) $(HOST_JSON)
	$(CC) $(LPS_CFLAGS) -o $@ test_pwrctrl_lps.c $(LPS_SHIM) $(HOST_JSON)

test-wow-lps: test_pwrctrl_wow_lps_level
	./test_pwrctrl_wow_lps_level wow_lps_level_vectors.json

test-wow-lps-rust: test_pwrctrl_wow_lps_level_rust
	./test_pwrctrl_wow_lps_level_rust wow_lps_level_vectors.json

test_pwrctrl_wow_lps_level: test_pwrctrl_wow_lps_level.c $(WOW_LPS_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_wow_lps_level.c $(WOW_LPS_SHIM) $(HOST_JSON)

librust_pwrctrl_wow.a: $(RUST_PWRCTRL_WOW)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_wow_lps_test \
		--crate-type staticlib $(RUST_PWRCTRL_WOW) -o $@

test_pwrctrl_wow_lps_level_rust: test_pwrctrl_wow_lps_level.c librust_pwrctrl_wow.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_wow_lps_level.c $(HOST_JSON) librust_pwrctrl_wow.a

test_pwrctrl_ps_deny: test_pwrctrl_ps_deny.c $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_deny.c $(SHIM) $(HOST_JSON)

test_pwrctrl_ips: test_pwrctrl_ips.c $(IPS_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ips.c $(IPS_SHIM) $(HOST_JSON)

librust_pwrctrl_ips.a: $(RUST_PWRCTRL_IPS_HOST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--crate-type staticlib $(RUST_PWRCTRL_IPS_HOST) -o $@

test_pwrctrl_ips_rust: test_pwrctrl_ips.c librust_pwrctrl_ips.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ips.c $(HOST_JSON) librust_pwrctrl_ips.a

librust_pwrctrl.a: $(RUST_PWRCTRL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_test \
		--crate-type staticlib $(RUST_PWRCTRL) -o $@

test_pwrctrl_ps_deny_rust: test_pwrctrl_ps_deny.c librust_pwrctrl.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_ps_deny.c $(HOST_JSON) librust_pwrctrl.a

test_pwrctrl_lps_rust: test_pwrctrl_lps.c librust_pwrctrl.a $(HOST_JSON)
	$(CC) $(LPS_CFLAGS) -o $@ test_pwrctrl_lps.c $(HOST_JSON) librust_pwrctrl.a

test_pwrctrl_lps_enter_rust: test_pwrctrl_lps_enter.c librust_pwrctrl.a $(HOST_JSON)
	$(CC) $(LPS_ENTER_CFLAGS) -o $@ test_pwrctrl_lps_enter.c $(HOST_JSON) librust_pwrctrl.a

test-ps-rdy: test_pwrctrl_ps_rdy
	./test_pwrctrl_ps_rdy ps_rdy_vectors.json

test-ps-rdy-rust: test_pwrctrl_ps_rdy_rust
	./test_pwrctrl_ps_rdy_rust ps_rdy_vectors.json

test_pwrctrl_ps_rdy: test_pwrctrl_ps_rdy.c $(PS_RDY_SHIM) $(HOST_JSON)
	$(CC) $(PS_RDY_CFLAGS) -o $@ test_pwrctrl_ps_rdy.c $(PS_RDY_SHIM) $(HOST_JSON)

librust_pwrctrl_ps_rdy.a: $(RUST_PWRCTRL_PS_RDY)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_pwrctrl_ps_rdy_test \
		--crate-type staticlib $(RUST_PWRCTRL_PS_RDY) -o $@

test_pwrctrl_ps_rdy_rust: test_pwrctrl_ps_rdy.c librust_pwrctrl_ps_rdy.a $(HOST_JSON)
	$(CC) $(PS_RDY_CFLAGS) -o $@ test_pwrctrl_ps_rdy.c $(HOST_JSON) librust_pwrctrl_ps_rdy.a

test-set-ps: test_pwrctrl_set_ps
	./test_pwrctrl_set_ps set_ps_vectors.json

test-set-ps-pg-wow: test_pwrctrl_set_ps_pg_wow
	./test_pwrctrl_set_ps_pg_wow set_ps_vectors_pg_wow.json

test_pwrctrl_set_ps: test_pwrctrl_set_ps.c $(SET_PS_SHIM) $(HOST_JSON)
	$(CC) $(SET_PS_CFLAGS) -o $@ test_pwrctrl_set_ps.c $(SET_PS_SHIM) $(HOST_JSON)

SET_PS_RUSTCFGS := --cfg host_pwrctrl_set_ps_test --cfg config_bt_coexist \
	--cfg config_fw_multi_port_support --cfg config_lps --cfg config_lps_lclk
SET_PS_PG_WOW_RUSTCFGS := $(SET_PS_RUSTCFGS) --cfg config_lps_pg --cfg config_wowlan

test_pwrctrl_set_ps_pg_wow: test_pwrctrl_set_ps.c $(SET_PS_SHIM) $(HOST_JSON)
	$(CC) $(SET_PS_PG_WOW_CFLAGS) -o $@ test_pwrctrl_set_ps.c $(SET_PS_SHIM) $(HOST_JSON)

test-set-ps-rust: test_pwrctrl_set_ps_rust
	./test_pwrctrl_set_ps_rust set_ps_vectors.json

test-set-ps-rust-pg-wow: test_pwrctrl_set_ps_rust_pg_wow
	./test_pwrctrl_set_ps_rust_pg_wow set_ps_vectors_pg_wow.json

librust_pwrctrl_set_ps.a: $(RUST_PWRCTRL_SET_PS)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on $(SET_PS_RUSTCFGS) \
		--crate-type staticlib $(RUST_PWRCTRL_SET_PS) -o $@

librust_pwrctrl_set_ps_pg_wow.a: $(RUST_PWRCTRL_SET_PS)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on $(SET_PS_PG_WOW_RUSTCFGS) \
		--crate-type staticlib $(RUST_PWRCTRL_SET_PS) -o $@

test_pwrctrl_set_ps_rust: test_pwrctrl_set_ps.c librust_pwrctrl_set_ps.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_set_ps.c $(HOST_JSON) librust_pwrctrl_set_ps.a

test_pwrctrl_set_ps_rust_pg_wow: test_pwrctrl_set_ps.c librust_pwrctrl_set_ps_pg_wow.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_pwrctrl_set_ps.c $(HOST_JSON) librust_pwrctrl_set_ps_pg_wow.a

clean:
	rm -f test_pwrctrl_ps_deny test_pwrctrl_ps_deny_rust test_pwrctrl_lps \
		test_pwrctrl_lps_rust test_pwrctrl_lps_enter test_pwrctrl_lps_enter_rust \
		test_pwrctrl_leave_all_ps test_pwrctrl_leave_all_ps_rust \
		test_pwrctrl_ips test_pwrctrl_ips_rust test_pwrctrl_ps_processor \
		test_pwrctrl_ps_processor_rust test_pwrctrl_lps_tp test_pwrctrl_lps_tp_rust \
		test_pwrctrl_lps_exec test_pwrctrl_lps_exec_rust \
		test_pwrctrl_rpwm test_pwrctrl_rpwm_rust \
		test_pwrctrl_register_alive test_pwrctrl_register_alive_rust \
		test_pwrctrl_unregister_alive test_pwrctrl_unregister_alive_rust \
		test_pwrctrl_wow_lps_level test_pwrctrl_wow_lps_level_rust \
		librust_pwrctrl_ps.a librust_pwrctrl_ips.a librust_pwrctrl_lps_tp.a librust_pwrctrl_lps_exec.a \
		librust_pwrctrl_rpwm.a \
		librust_pwrctrl_alive.a librust_pwrctrl_wow.a librust_pwrctrl_ps_rdy.a \
		test_pwrctrl_set_ps test_pwrctrl_set_ps_pg_wow test_pwrctrl_set_ps_rust \
		test_pwrctrl_set_ps_rust_pg_wow librust_pwrctrl_set_ps.a librust_pwrctrl_set_ps_pg_wow.a \
		librust_pwrctrl.a test_pwrctrl_ps_rdy test_pwrctrl_ps_rdy_rust
