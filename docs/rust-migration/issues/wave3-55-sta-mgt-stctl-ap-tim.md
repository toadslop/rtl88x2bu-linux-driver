---
title: "[W3-55] Translate sta_mgt st_ctl and AP TIM/VAPID helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-55
epic: E05
blocked_by: [W3-54]
estimate_loc: 200
---

## Goal

Port session-tracker ctl and AP TIM/VAPID helpers to Rust:

From [`core/rtw_sta_mgt.c`](../../../core/rtw_sta_mgt.c) → [`rust/rtw_sta_mgt.rs`](../../../rust/rtw_sta_mgt.rs):

- `rtw_st_ctl_init`, `rtw_st_ctl_clear_tracker_q`, `rtw_st_ctl_deinit`
- `rtw_st_ctl_register`, `rtw_st_ctl_unregister`, `rtw_st_ctl_chk_reg_s_proto`, `rtw_st_ctl_chk_reg_rule`
- `rtw_stainfo_offset`

From [`core/rtw_ap.c`](../../../core/rtw_ap.c) → [`rust/rtw_ap_rest.rs`](../../../rust/rtw_ap_rest.rs):

- `rtw_set_tim_ie`, `rtw_ap_allocate_vapid`, `rtw_ap_release_vapid`

## Notes

- W3-37/38 covered ACL and AID/pre-link; sta init/free/hash stay in C.
- **Combined ~200 LOC estimate (two sub-slices, one issue):**
  - ~120 LOC — `st_ctl` lifecycle/register/check helpers (`rtw_sta_mgt.rs`; queue/mutex
    coupling; adapter fixtures for register/unregister rules).
  - ~80 LOC — AP TIM/VAPID helpers (`rtw_ap_rest.rs`; mostly pure IE bytes + bitmap).
- AP lifecycle/beacon core stays in C.
- L2: extend `tests/host/sta_mgt/` for st_ctl rules; new `tests/host/ap/` for TIM/VAPID.
  Acceptance is per sub-slice — both harnesses must pass before closing.

## Acceptance

- L0 build + L2 host unit tests for st_ctl helpers (`tests/host/sta_mgt/`)
- L0 build + L2 host unit tests for AP TIM/VAPID helpers (`tests/host/ap/`)
