---
title: "[FX-08] debug procs: Rust cfg gates and C guards disagree (link safety)"
labels: [rust-migration, phase-1, regression, size/~200]
type: child
id: FX-08
epic: E14
blocked_by: []
estimate_loc: 140
---

## Priority

**P2 (medium)** — link failures in supported non-default configs; no default-build impact.

## Problem

1. **`CONFIG_PROC_DEBUG=n`, `CONFIG_RTW_DEBUG=y`.** Rust proc handlers are gated on
   `#[cfg(any(config_rtw_debug, config_proc_debug))]` (`export_proc!` in
   [`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs) ~line 94, and the
   `rtw_debug_proc_{ap,trx,rate,ps,ps_info,hw,signal,rx_cnt,dbg_logs,…}.rs` handlers,
   plus `dump_adapters_status`), but their `rtw_rust_debug_proc_*` helpers live
   inside `#ifdef CONFIG_PROC_DEBUG` in [`core/rtw_debug.c`](../../../core/rtw_debug.c)
   (lines ~585–6035). The original handlers were under `CONFIG_PROC_DEBUG` too.
   Review measured ~55 undefined `rtw_rust_debug_*` symbols with `nm` in that config.
   btcoex / smps gates use `any(feature, rtw_debug, proc_debug)` where `all` was meant.
2. **PCI.** The W3-231 guard in [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c)
   (~5300–5427, 08248250 #1861) also compiles out `proc_get_pci_bridge_conf_space`,
   which [`rust/rtw_debug_proc_pci.rs`](../../../rust/rtw_debug_proc_pci.rs) does not
   export; `os_dep/linux/rtw_proc.c:5435` still references it (verified with `cpp`).
3. **TDLS display prototypes.** f9d7fa7a (#1732) / b9689dc2 (#1733) declare
   `int proc_tdls_display_*(struct seq_file *)` in
   [`include/rtw_debug.h`](../../../include/rtw_debug.h) (~614–616) with external linkage
   while the fallbacks in `core/rtw_debug_rest.c` (~6153, 6235, 6357) stay `static` →
   "static declaration follows non-static declaration" with `CONFIG_TDLS=y` and no
   `CONFIG_RUST_RTW_DEBUG`.
4. **GPIO wakeup.** `wowlan_gpio_info` helpers sit inside `#ifdef CONFIG_WOWLAN`
   (`core/rtw_debug.c` ~4834) and the Rust gate is `all(config_wowlan, config_gpio_wakeup)`;
   C registers the proc with `CONFIG_GPIO_WAKEUP` alone (35cf0dc2 #1781).

## Fix

1. Gate proc handlers on `config_proc_debug` only; change the btcoex/smps `any` to `all`.
2. Split the PCI guard so only the two Rust-ported setters are compiled out.
3. Wrap the three TDLS prototypes in
   `#if defined(CONFIG_RUST) && defined(CONFIG_RUST_RTW_DEBUG)` (or drop `static`).
4. Move the `wowlan_gpio_info` helpers and the Rust gate to `CONFIG_GPIO_WAKEUP`
   alone (object-link condition is FX-07 row 7).

## Acceptance

- Default L0 + `verify-ko-probes.sh`; L1 allowlists unchanged for default build.
- Object-level link check (`nm -u` of all `rust/rtw_debug_proc*.o` vs `nm` of the C
  objects) clean for `CONFIG_PROC_DEBUG=n CONFIG_RTW_DEBUG=y` and for `CONFIG_TDLS=y`.
- `cpp -DCONFIG_PCI_HCI -DCONFIG_RUST -DCONFIG_RUST_RTW_DEBUG` on the PCI block keeps
  `proc_get_pci_bridge_conf_space`.

## Notes

- Debug-proc lane: FX-13 and FX-18 are chained behind this issue (same files).
