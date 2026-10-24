---
title: "[W3-166] Translate rtw_debug_rest.c — proc rx_signal get/set"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-166
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port RX signal debug proc hooks from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_signal.rs`](../../../rust/rtw_debug_proc_signal.rs):

- `proc_get_rx_signal`
- `proc_set_rx_signal`

## Notes

- `CONFIG_MP_INCLUDED` MP-mode branch uses PHYDM `adapter_to_phydm` for antenna/rate display — gate like other debug proc ports.
- Optional `DBG_RX_SIGNAL_DISPLAY_RAW_DATA` helpers stay behind matching `cfg` flags.
- Setter clamps `signal_strength` to 100 and toggles `recvpriv.is_signal_dbg`.

## Acceptance

- L0 + L1 on both symbols for pinned STA profile
- MP and non-MP getter paths preserved when respective `cfg` symbols are enabled
