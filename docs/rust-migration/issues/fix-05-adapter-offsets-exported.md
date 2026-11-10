---
title: "[FX-05] Replace hard-coded _ADAPTER byte offsets with C-exported offsetof"
labels: [rust-migration, phase-1, regression, size/~200]
type: child
id: FX-05
epic: E14
blocked_by: []
estimate_loc: 140
---

## Priority

**P2 (medium)** — default build is correct today, but the module no longer builds
on lock-debug / `PREEMPT_RT` kernels or with several driver options, and two
offsets can go silently wrong.

## Problem

Rust reads `_ADAPTER` fields at literal byte offsets:

| Constant | Value | Field | File |
|---|---|---|---|
| `MLMEEXT_BASICRATE_OFFSET` | 0x848 | `mlmeextpriv.basicrate` | [`rust/rtw_wlan_util.rs`](../../../rust/rtw_wlan_util.rs) `kernel_layout` |
| `MLMEEXT_DATARATE_OFFSET` | 0x855 | `mlmeextpriv.datarate` | same |
| `CUR_CHANNEL_OFFSET` | 0x844 | `mlmeextpriv.cur_channel` | same, `network_layout` |
| `HT_ENABLE_OFFSET` | 0xc58 | `mlmeextpriv.mlmext_info.HT_enable` | same |
| `VHT_ENABLE_OFFSET` | 0xc66 | `mlmeextpriv.mlmext_info.VHT_enable` | same |
| `AMSDU_MODE_OFFSET` | 0x3859 | `registrypriv.amsdu_mode` | [`rust/rtw_crypto_wrap.rs`](../../../rust/rtw_crypto_wrap.rs) |

`basicrate`/`datarate` feed `get_rate_set` (join/assoc path); `amsdu_mode` feeds
the TX crypto wrapper.

acdc4dec (#1803) added file-scope `static_assert`s in
[`core/rtw_wlan_util.c`](../../../core/rtw_wlan_util.c) for four of the six. Review
findings (verified with `offsetof` / `-fdump-record-layouts` under the real module flags):

- `mlmeextpriv` sits after 4 `spinlock_t` + 3 `timer_list`; `amsdu_mode` after 34
  spinlocks, 7 timers, 5 semaphores, 2 mutexes, 6 tasklets, 1 completion. All grow
  with `CONFIG_DEBUG_SPINLOCK`, lockdep, `DEBUG_MUTEXES`, or `PREEMPT_RT` → the
  asserts fire and the module fails to build on those kernels.
- `-DCONFIG_RTW_80211K` (documented toggle), `CONFIG_BCN_CNT_CONFIRM_HDL`, and
  `CONFIG_RTW_MULTI_AP` move the asserted offsets → build failure.
- The asserts are not under `#ifdef CONFIG_RUST`, so they also break C-only builds.
- `HT_enable`/`VHT_enable` are not asserted; `CONFIG_RTW_ROAM_QUICKSCAN` or
  `CONFIG_MI_WITH_MBSSID_CAM` (auto-on when `CONFIG_IFACE_NUMBER > 2`) moves them
  to 0xc5c/0xc6a silently. Only consumer: `judge_network_type` (no C callers today).

## Fix

1. In `core/rtw_wlan_util.c`, export
   `const size_t rtw_rust_off_adapter_{basicrate,datarate,cur_channel,ht_enable,vht_enable,amsdu_mode} = offsetof(...)`,
   following the existing pattern in
   [`core/rtw_wapi_sms4_frame_rust_acc.c`](../../../core/rtw_wapi_sms4_frame_rust_acc.c).
2. Rust: replace the literals with `extern "C" { static rtw_rust_off_adapter_…: usize; }`
   (host test builds keep their own struct layout).
3. Delete the four `static_assert`s (no longer needed) and the `kernel_layout` /
   `network_layout` literal modules.

## Acceptance

- L0 default build; L0 with `USER_EXTRA_CFLAGS=-DCONFIG_RTW_80211K` and with
  `-DCONFIG_MI_WITH_MBSSID_CAM` both link.
- L1: `rust/rtw_wlan_util.o` / `rust/rtw_crypto_wrap.o` exports unchanged.
- `rg '0x(844|848|855|c58|c66|3859)' rust/` finds no kernel-path literal.
- L2 wlan_util / crypto_wrap suites green.

## Notes

- Mainmatter *C to Rust Migration Book*, ch. 2 (types across the boundary): layouts
  must come from the C definition (bindgen / `offsetof`), never be retyped.
