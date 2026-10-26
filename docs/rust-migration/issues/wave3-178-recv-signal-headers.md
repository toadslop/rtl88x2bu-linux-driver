---
title: "[W3-178] Translate rtw_recv.c — signal-stat profile and SNAP headers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-178
epic: E05
blocked_by: []
estimate_loc: 150
---

## Goal

Port recv signal-stat and LLC SNAP constants from [`core/rtw_recv.c`](../../../core/rtw_recv.c) to
[`rust/rtw_recv_signal.rs`](../../../rust/rtw_recv_signal.rs):

- `signal_stat_calc_profile` (under `CONFIG_NEW_SIGNAL_STAT_PROCESS`)
- `rtw_bridge_tunnel_header`
- `rtw_rfc1042_header`
- `SNAP_ETH_TYPE_IPX`, `SNAP_ETH_TYPE_APPLETALK_AARP`, and `SNAP_ETH_TYPE_TDLS` when the matching `CONFIG_*` gates are on

## Notes

- Mostly static data + trivial accessors; keep symbol names/addresses visible to C linkers.
- Does not port `_rtw_init_recv_priv` / frame pool init (separate, larger recv tranche).
- Parallel lane — no overlap with [W3-177](wave3-177-recv-ap-pwrbit-wmmps.md) Rust module.

## Acceptance

- L0 + L1 on exported symbols
- L2: byte-for-byte compare of header tables and profile rows against C oracles in `tests/host/recv/`
