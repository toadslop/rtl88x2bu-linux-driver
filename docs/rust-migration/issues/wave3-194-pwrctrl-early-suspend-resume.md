---
title: "[W3-194] Translate rtw_pwrctrl.c — early suspend and resume workqueue"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-194
epic: E05
blocked_by: [W3-190]
estimate_loc: 200
---

## Goal

Port Android/legacy early-suspend registration and workqueue resume helpers from
[`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_resume.rs`](../../../rust/rtw_pwrctrl_resume.rs):

- `rtw_register_early_suspend` / `rtw_unregister_early_suspend` (both `CONFIG_HAS_EARLYSUSPEND` and `CONFIG_ANDROID_POWER` paths)
- `rtw_is_earlysuspend_registered`, `rtw_is_do_late_resume`, `rtw_set_do_late_resume`
- `rtw_resume_in_workqueue` and `resume_workitem_callback` when `CONFIG_RESUME_IN_WORKQUEUE`

**Out of scope (owned by [W3-189](wave3-189-pwrctrl-init-p1.md) / [W3-190](wave3-190-pwrctrl-init-free-p2.md)):** `_init_workitem` for `resume_work`, `create_singlethread_workqueue`, and the init-time `rtw_register_early_suspend()` call at the end of `rtw_init_pwrctrl_priv` — do not duplicate those in this issue.

## Notes

- Epic tranche 20 follow-up after [W3-190](wave3-190-pwrctrl-init-free-p2.md); teardown in W3-190 must call into these exports before freeing priv.
- `rtw_resume_process` / `rtw_resume_lock_suspend` stay C exports — call via existing FFI.
- **Makefile:** dedicated `rtw_pwrctrl_resume.o` → `rtw_pwrctrl_resume.rs` (parallel object lane; do not expand `rtw_pwrctrl_init.o` swap).

## Acceptance

- L0 + L1 on exported resume/early-suspend symbols wired from C
- L2 optional: stub `pwrctrl_priv` — register/unregister idempotency and `do_late_resume` flag transitions
