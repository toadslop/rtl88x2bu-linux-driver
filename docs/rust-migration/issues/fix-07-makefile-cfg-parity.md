---
title: "[FX-07] Makefile: rustc --cfg and Rust object-link parity with effective C config"
labels: [rust-migration, phase-1, regression, size/~200]
type: child
id: FX-07
epic: E14
blocked_by: []
estimate_loc: 160
---

## Priority

**P2 (medium)** — every item is a non-default config, but several are runtime bugs
(NULL call, double free, silent no-op), not just link failures.

## Problem

`rustflags-y += --cfg …` and conditional `$(MODULE_NAME)-y += rust/*.o` lines are
derived from Makefile variables or `grep include/autoconf.h`, not from the macro
set C actually sees (`ccflags-y`, `drv_conf.h`, `hal_ic_cfg.h`, kernel `.config`).
Mismatches found in the review (line numbers at `aa3a7b43`):

| # | C sees | Rust sees | Effect | Commit |
|---|---|---|---|---|
| 1 | `-DCONFIG_AP_MODE` forced by `CONFIG_AP_WOWLAN=y` (Makefile:1267) | `config_ap_mode` only when `CONFIG_AP_MODE=y` (:2787) | `process_pwrbit_data` / `process_wmmps_data` become **silent no-ops**; `proc_*bmc_tx_rate`, `proc_set_ap_linking_test` undefined | 754f1dac #1548, a7b6f319 #1473, 7fe5bf7c #1495 |
| 2 | `CONFIG_USB_INTERRUPT_IN_PIPE`, `CONFIG_PREALLOC_RECV_SKB`, `CONFIG_PREALLOC_RX_SKB_BUFFER`, `CONFIG_USE_USB_BUFFER_ALLOC_RX` (autoconf / platform ccflags) | never passed | `_read_interrupt` stays NULL → oops at hal init; prealloc skbs `kfree_skb`'d (double free); int_in urb leak | f13658a7 #1423, e8daf4ae #1383 |
| 3 | `-DCONFIG_RTL8192F` (:999) | no `--cfg CONFIG_RTL8192F` | 8192F "write CAM word 1 last" ordering lost in `rtw_sec_write_cam_ent` | b03e52b4 #1437 |
| 4 | `CONFIG_LPS_1T1R` from `hal_ic_cfg.h` (8822C + LPS) | grep / make var only (:2904) | `rtw_pm_set_wow_lps_1t1r` undefined | fbc2d495 #1646 |
| 5 | `CONFIG_HAS_EARLYSUSPEND` / `CONFIG_ANDROID_POWER` (kernel Kconfig), `CONFIG_RESUME_IN_WORKQUEUE` (platform ccflags) | grep of driver autoconf (:2910-2916) | resume / early-suspend symbols undefined | 3a0a1594 #1637 |
| 6 | `CONFIG_TX_AMSDU` via ccflags | object linked only on autoconf grep (:3287) | `proc_{get,set}_tx_amsdu*` undefined | 9d292a3e #1708 |
| 7 | `CONFIG_GPIO_WAKEUP` without WoWLAN registers `wowlan_gpio_info` | Rust gate `all(config_wowlan, config_gpio_wakeup)`; object under WoWLAN (:3285) | `proc_{get,set}_wowlan_gpio_info` undefined | 35cf0dc2 #1781 |
| 8 | `CONFIG_MCC_PHYDM_OFFLOAD` only for some chips | forced with `CONFIG_MCC_MODE` (:1341) | helper undefined on non-offload chips | d0ca621b #1801 |
| 9 | `CONFIG_POWER_SAVING` off (Makefile var or `drv_conf.h` customer configs) | `rtw_pwrctrl_{ips,rpwm,cpwm_int,alive}.o` linked unconditionally (:3331-3338); `config_power_saving` from make var (:3046) | undefined `rtw_rust_{ips,rpwm,cpwm,alive}_*` | 45be9b1b #1486, 3851c27a #1525, df5dfdd1 #1622, 80c41d2f #1543, 4b1636a9 #1785 |
| 10 | no `CONFIG_LPS_PG`, `CONFIG_FW_MULTI_PORT_SUPPORT` only with `CONFIG_CONCURRENT_MODE` | `--cfg config_lps_pg --cfg config_fw_multi_port_support` for 8822B (:2895) | Rust compiles LPS_PG/multiport branches C does not (benign today only because shims are `#ifdef` no-ops) | dd54cd9e #1616 |
| 11 | no `CONFIG_RF_SHADOW_RW` | `--cfg config_rf_shadow_rw` unconditional (:3202) | RF-shadow code + 4 KB `.bss` built with no C user | 8aae02bd #1279 |
| 12 | `CONFIG_RTW_CUSTOMER_STR`, `CONFIG_RTW_MAC_HIDDEN_RPT` | no cfg | needed by FX-06 | 62570053 #1445 |
| 13 | TDLS object grep matches `CONFIG_TDLS_DRIVER_SETUP` (:3291, no word boundary); `DBG_MEMORY_LEAK` only from `-D` (:3088); rf4ce cfg ignores `CONFIG_BT_COEXIST` | — | latent link breaks | debug-proc wiring (W3-212 TDLS and siblings) |

## Fix

For each row, derive the cfg / object condition from the same condition C uses:
`$(filter -DFOO,$(ccflags-y))` **or** an anchored `grep -Eq '^\s*#\s*define\s+FOO\b'`
over `include/autoconf.h`, **or** the kernel `CONFIG_*` (already visible to make
via `include/config/auto.conf`). Delete rows 10–11 cfgs. Follow the existing
`_rust_cfg_lps_chk_by_tp` pattern.

## Acceptance

- Default L0: `scripts/ci/verify-ko-probes.sh 88x2bu.ko` passes, and the rustc cfg
  set for the default build is unchanged except rows 10–11 removed (attach before/after
  `--cfg` lists from `.rust/*.o.cmd`).
- L0 links (no modpost undefined symbols) for: `CONFIG_AP_WOWLAN=y CONFIG_AP_MODE=n CONFIG_P2P=n`;
  `USER_EXTRA_CFLAGS=-DCONFIG_TX_AMSDU`; `CONFIG_POWER_SAVING=n`; `CONFIG_GPIO_WAKEUP=y CONFIG_WOWLAN=n`.
- `nm rust/hal_usb.o` shows `usb_read_interrupt` referenced when built with
  `-DCONFIG_USB_INTERRUPT_IN_PIPE -DCONFIG_SUPPORT_USB_INT`.

## Notes

- Short-term manual fix; T21 replaces hand derivation with a generated cfg list and a CI parity check.
- Makefile lane: FX-06, T21, and T24 are chained behind this issue.
