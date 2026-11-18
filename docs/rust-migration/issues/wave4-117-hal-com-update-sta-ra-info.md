---
title: "[W4-117] Translate hal_com_rest.c — STA RA info orchestrator"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-117
epic: E06
blocked_by: []
estimate_loc: 60
---

## Goal

Port `rtw_hal_update_sta_ra_info` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs) (same module as
[W4-97](wave4-97-hal-com-update-sta-wset-mimo.md) / [W4-99](wave4-99-hal-com-sta-smps-rate-mask.md)):

- Calls `rtw_hal_update_sta_mimo_type`, `rtw_hal_update_sta_smps_cap`, and
  `rtw_hal_update_sta_rate_mask` in the same order as C.

## Notes

- **Parallel lane:** `hal_com` / `rust/hal_com.rs` — disjoint from in-flight `hal_dm` PRs (W4-111–113).
- Thin orchestrator; behavior is entirely delegated to Rust children already on `master`.

## Acceptance

- L0 + L1 for `rtw_hal_update_sta_ra_info`
- L2: host vectors asserting call order and one-shot invocation per child (stub trace)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none — W4-97/W4-99 landed on `master`)