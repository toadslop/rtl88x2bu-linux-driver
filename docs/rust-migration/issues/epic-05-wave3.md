---
title: "[Epic] Wave 3 — Core protocol translation"
labels: [rust-migration, phase-1, wave-3]
type: epic
id: E05
blocked_by: [E04]
---

## Goal

Translate `core/` protocol files (cmd/io/security/xmit/recv/mlme/…) behind stable `extern "C"` boundaries, sliced into ~200 LOC issues.

## Children (tranche 1 — filed)

Bridge from Wave 2 + leaf core units (low HAL coupling):

| ID | File | Focus |
|----|------|--------|
| W3-01 | `wave3-01-swcrypto.md` | `rtw_swcrypto.c` (Wave 2 tail) |
| W3-02 … W3-05 | `wave3-02-chplan-part*.md` | `rtw_chplan.c` in four slices |
| W3-06 … W3-09 | `wave3-06-security-*.md` | `rtw_security.c` start (type str → WEP → TKIP MIC) |
| W3-10 … W3-11 | `wave3-10-wlan-util-*.md` | `rtw_wlan_util.c` rate helpers |

Supporting: **A2** (channel/rate domain types), **T4** (host chplan harness).

## Children (tranche 2 — open when tranche 1 is underway)

Slice ~200 LOC function groups from larger protocol TUs; file new `wave3-*.md` issues as needed:

- `rtw_security.c` remainder (TKIP/CCMP frame encrypt/decrypt paths)
- `rtw_io.c`, `rtw_rf.c`, `rtw_pwrctrl.c`
- `rtw_recv.c`, `rtw_xmit.c`, `rtw_cmd.c`
- `rtw_mlme.c`, `rtw_mlme_ext.c`, `rtw_ap.c`, `rtw_ieee80211.c`
- `rtw_sta_mgt.c`, `rtw_vht.c`, smaller `rtw_*.c` (sreset, iol, roch, …)

Wave 3 milestone (L4): WPA2 STA associate + encrypted ping when hardware available (same bar as Wave 2).
