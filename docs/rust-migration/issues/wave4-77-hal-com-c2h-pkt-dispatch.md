---
title: "[W4-77] Translate hal_com_rest.c — C2H packet pre/handle"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-77
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `CONFIG_FW_C2H_PKT` orchestration from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com_c2h_pkt.rs`](../../../rust/hal_com_c2h_pkt.rs) (new module + Makefile hook):

- `rtw_hal_c2h_pkt_pre_hdl`
- `rtw_hal_c2h_pkt_hdl`

## Notes

- **Parallel lane:** independent of hal_intf tranche — disjoint Rust module from [W4-73](wave4-73-hal-intf-hwreg-odm.md).
- Non-`RTW_HALMAC` body only in this slice (`hal_mac` branch stays in C until a follow-up).
- Reuse existing `rtw_hal_c2h_pkt_hdr_parse`, `rtw_hal_c2h_handler`, and `rtw_c2h_packet_wk_cmd` FFI.

## Acceptance

- L0 with `CONFIG_FW_C2H_PKT=y` on pinned profile
- L1 on both handler symbols
- Debug print paths match C when `DBG_C2H_PKT_*` enabled

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none — ready after W4-72 merges or stacks on `master`)