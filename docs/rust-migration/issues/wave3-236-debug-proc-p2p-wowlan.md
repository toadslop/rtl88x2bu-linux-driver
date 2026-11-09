---
title: "[W3-236] Translate rtw_debug_rest.c — P2P WoWLAN trigger info proc"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-236
epic: E05
blocked_by: []
estimate_loc: 110
---

## Goal

Port P2P WoWLAN info proc from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_wow_p2p.rs`](../../../rust/rtw_debug_proc_wow_p2p.rs):

- `proc_get_p2p_wowlan_info` (`CONFIG_P2P_WOWLAN`)

## Notes

- Prints `p2p_wowlan_info` trigger flag, frame type enum strings, peer MAC/WPS fields.
- Getter-only; depends on `wifidirect_info` layout via domain/FFI accessors.
- Parallel with W3-235 (different Rust module).

## Acceptance

- L0 with `CONFIG_P2P_WOWLAN`
- L1 on handler
- L2: `is_trigger: False` banner; triggered path with Nego Request label
