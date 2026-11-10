---
title: "[FX-14] pwrctrl: restore dbg_ps_insuspend_cnt, LPS_LCLK guards, and dropped PS diagnostics"
labels: [rust-migration, phase-1, regression, size/~200]
type: child
id: FX-14
epic: E14
blocked_by: [FX-04]
estimate_loc: 140
---

## Priority

**P3 (low)** — observability parity; one counter is wrong in the default build.

## Problem

1. **`dbg_ps_insuspend_cnt` never increments** —
   [`rust/rtw_pwrctrl_ps.rs`](../../../rust/rtw_pwrctrl_ps.rs) (~239, b6f7939d/8bd6e309
   #1516/#1517) returns on `bInSuspend` without the C `pdbgpriv->dbg_ps_insuspend_cnt++`;
   proc shows 0.
2. **`rtw_set_fw_in_ips_mode`** ([`rust/rtw_pwrctrl_fw_ips.rs`](../../../rust/rtw_pwrctrl_fw_ips.rs)
   ~346–433, 9c087c4f #1611 / b9b8adec #1612) runs the rpwm 32K / CPWM-toggle polls
   unconditionally where C had `#ifdef CONFIG_LPS_LCLK`, and drops the per-iteration
   `rtw_read8(0x08)`/`rtw_read8(0x03)` inside the `RTW_INFO` (changes poll timing).
   Only with `CONFIG_FWLPS_IN_IPS`.
3. **Dropped log lines**: "Leave/Enter 802.11 power save - %s" (`rtw_set_ps_mode`
   ignores `msg`), `ps_deny=0x%08X, skip power save!`, "nolinked power save
   enter/leave" (IPS, RTW_PRINT), "WOW_LPS_mode=…", the `CONFIG_PNO_SUPPORT`
   leak printks in `rtw_free_pwrctrl_priv`, and all `rtw_set_fw_in_ips_mode` `RTW_INFO`s.

## Fix

Add C log/counter shims in `core/rtw_pwrctrl.c` (format strings copied from
`<port>^:core/rtw_pwrctrl.c`) and call them at the original positions; gate the
two fw_ips blocks with `#[cfg(config_lps_lclk)]` and restore the two reads.

## Acceptance

- L0; L1 allowlists updated only for new shim imports.
- `strings 88x2bu.ko` contains each restored format string.
- Review diff of each shim body vs the `commit^` C.

## Notes

- **Dependency rationale:** same C TU as FX-01 / FX-04 (`core/rtw_pwrctrl.c`).
