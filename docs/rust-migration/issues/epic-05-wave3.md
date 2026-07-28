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

Bridge from Wave 2 + leaf core units (low HAL coupling). **Issue IDs match in-flight implementation branches** (`cursor/w3-0*-3dd4`):

| ID | File | Focus |
|----|------|--------|
| W3-01 | `wave3-01-swcrypto-ccmp-gcmp.md` | `rtw_swcrypto.c` CCMP/GCMP wrappers |
| W3-02 | `wave3-02-swcrypto-bip-tdls.md` | `rtw_swcrypto.c` BIP/SIV wrappers; TDLS → `rtw_swcrypto_rest.c` |
| W3-03 | `wave3-03-ie-parse.md` | `rtw_ieee80211.c` IE parse helpers |
| W3-04 … W3-07 | `wave3-04-security-*.md` | `rtw_security.c` start (type str → WEP → TKIP MIC) |
| W3-08 … W3-09 | `wave3-08-wlan-util-*.md` | `rtw_wlan_util.c` rate helpers |

Chplan work is **Wave 2** (W2-17…W2-20), not Wave 3 — see `epic-04-wave2.md`.

Supporting: **A2** (channel/rate domain types), **A3** (security domain types), **T5** (security/wlan_util L2 harness).

## Children (tranche 2 — filed)

| ID | File | Focus |
|----|------|--------|
| W3-10 … W3-16 | (GitHub only) | `rtw_security_rest.c` remainder |
| W3-17 | (GitHub only) | `rtw_chplan_rest.c` beacon hint + debug dumps |
| W3-18 | (GitHub only) | `rtw_io_rest.c` I/O error + reg sniff matchers |
| W3-19 | `wave3-19-rf-ch-layout.md` | `rtw_rf.c` channel layout helpers |
| W3-20 | `wave3-20-rf-freq.md` | `rtw_rf.c` ch2freq / freq2ch / freq range |

Further ~200 LOC slices from larger protocol TUs; file new `wave3-*.md` issues as needed:

- `rtw_rf.c` remainder (op-class, txpwr, regd_exc, …)
- `rtw_pwrctrl.c`
- `rtw_recv.c`, `rtw_xmit.c`, `rtw_cmd.c`
- `rtw_mlme.c`, `rtw_mlme_ext.c`, `rtw_ap.c`, remaining `rtw_ieee80211.c`
- `rtw_sta_mgt.c`, `rtw_vht.c`, smaller `rtw_*.c` (sreset, iol, roch, …)

Wave 3 milestone (L4): WPA2 STA associate + encrypted ping when hardware available (same bar as Wave 2).
