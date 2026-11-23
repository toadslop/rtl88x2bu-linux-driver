---
title: "[W4-126] Translate hal_com_rest.c — MBSSID CAM HW push and enable"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-126
epic: E06
blocked_by: [W4-125]
estimate_loc: 160
---

## Goal

Under `CONFIG_MBSSID_CAM`, extend [`rust/hal_com_mbid_cam.rs`](../../../rust/hal_com_mbid_cam.rs) with
sync helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c):

- `rtw_ap_set_mbid_num`
- `rtw_mbid_cam_enable`
- `rtw_mi_set_mbid_cam`
- static `write_mbssid_cam` (via `HW_VAR_MBSSID_CAM_WRITE`)

## Notes

- **C callers remain:** `write_mbssid_cam` is still invoked from `rtw_hal_change_macaddr_mbid`, `HW_VAR_MAC_ADDR` / MBSSID mac paths this slice does not move — export the Rust symbol (or a thin C shim) so those call sites keep linking until their slices land.
- **Same lane as W4-125** — shares submodule and Makefile swap.
- `rtw_mbid_cam_enable` calls `rtw_hal_rcr_add(RCR_ENMBID)`; RCR helpers may stay C stubs until [W4-127](wave4-127-hal-com-rcr-leaf.md) lands (thin `extern "C"` delegate OK).
- `DBG_MBID_CAM_DUMP` path may call W4-125 dump — keep cfg parity.

## Acceptance

- L0/L1 with same MBSSID CAM profile as W4-119
- L2: host fixture — bitmap with two slots → `rtw_mi_set_mbid_cam` records `HW_VAR_MBSSID_CAM_WRITE` payloads; include `rtw_ap_set_mbid_num` `REG_MBID_NUM` RMW oracle `((read & 0xF8) | ((ap_num - 1) & 0x07))`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-125