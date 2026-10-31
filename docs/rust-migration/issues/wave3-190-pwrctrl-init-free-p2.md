---
title: "[W3-190] Translate rtw_pwrctrl.c — init/free pwrctrl (part 2/2)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-190
epic: E05
blocked_by: [W3-189]
estimate_loc: 220
---

## Goal

Finish power-control lifecycle in [`rust/rtw_pwrctrl_init.rs`](../../../rust/rtw_pwrctrl_init.rs) and port teardown,
continuing `rtw_init_pwrctrl_priv` in **source order** after [W3-189](wave3-189-pwrctrl-init-p1.md):

- `#ifdef CONFIG_GPIO_WAKEUP` GPIO defaults and HAL pin setup
- Remainder of `#ifdef CONFIG_WOWLAN` init (patterns, PNO, keep-alive, etc.)
- Trailing init hooks after the WOWLAN block: `#ifdef CONFIG_LPS_POFF` (`HW_VAR_LPS_POFF_INIT`) and
  `#ifdef CONFIG_LPS_ACK` (`lps_ack_mutex` / status defaults) through **end of** `rtw_init_pwrctrl_priv`
- Init-time call to `rtw_register_early_suspend(pwrctrlpriv)` after GPIO setup (function bodies → [W3-194](wave3-194-pwrctrl-early-suspend-resume.md))
- `rtw_free_pwrctrl_priv` (including `rtw_unregister_early_suspend` / resume workqueue flush — coordinate ordering with W3-194)

## Notes

- Chains on [W3-189](wave3-189-pwrctrl-init-p1.md) same module/Makefile object (`rtw_pwrctrl_init.rs`).
- **`CONFIG_RESUME_IN_WORKQUEUE`:** resume workitem + `rtw_workqueue` create stay in [W3-189](wave3-189-pwrctrl-init-p1.md); runtime enqueue/callback → [W3-194](wave3-194-pwrctrl-early-suspend-resume.md) only.
- **`CONFIG_HAS_EARLYSUSPEND` / `CONFIG_ANDROID_POWER`:** register/unregister **implementations** and late-resume helpers → W3-194; W3-190 only wires the init-time register call and teardown unregister.
- Workitem cancel paths must mirror C ordering to avoid use-after-free in L3.

## Acceptance

- L0 + L1 on init + free symbols
- L2 optional: init→free round-trip field hygiene on stub priv
