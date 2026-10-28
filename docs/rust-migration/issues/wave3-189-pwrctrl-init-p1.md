---
title: "[W3-189] Translate rtw_pwrctrl.c — init pwrctrl priv (part 1/2)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-189
epic: E05
blocked_by: []
estimate_loc: 220
---

## Goal

Port the first half of `rtw_init_pwrctrl_priv` from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_init.rs`](../../../rust/rtw_pwrctrl_init.rs) through `#ifdef CONFIG_RESUME_IN_WORKQUEUE`
(workqueue create), in **source order**:

- Lock init, counter defaults, registry-backed IPS/LPS fields
- `#ifdef CONFIG_LPS_LCLK` workitem / RPWM timer setup
- `#ifdef CONFIG_LPS_PG` name init
- `pwr_state_check_timer` registration and default `wowlan_*` mode flags
- `#ifdef CONFIG_RESUME_IN_WORKQUEUE` resume workitem + `rtw_workqueue`

Stop before `#ifdef CONFIG_HAS_EARLYSUSPEND` / `#ifdef CONFIG_GPIO_WAKEUP` / `#ifdef CONFIG_WOWLAN`.

## Notes

- `CONFIG_CONCURRENT_MODE` primary-adapter early return stays mirrored.
- Part 2 covers early suspend, GPIO wakeup, the large `#ifdef CONFIG_WOWLAN` block, and teardown exports.

## Acceptance

- L0 + L1 partial init symbol (split function or shared helper exported to C)
- L2: host snapshot of key `pwrctrl_priv` fields after init on stub adapter
