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
| W3-21 | `wave3-21-rf-lookup-tables.md` | `rtw_rf.c` lookup/format tables |
| W3-22 | `wave3-22-rf-op-class.md` | `rtw_rf.c` global op-class lookup |
| W3-23 | `wave3-23-rf-trx-path.md` | `rtw_rf.c` RF type / trx path helpers |
| W3-24 | `wave3-24-rf-txpwr-cac.md` | `rtw_rf.c` txpwr format + DFS CAC |
| W3-25 | `wave3-25-rf-tx-path-nss.md` | `rtw_rf.c` tx path NSS + bb gain sel |
| W3-26 | `wave3-26-ieee80211-rates.md` | `rtw_ieee80211.c` rate/network type |
| W3-27 | `wave3-27-ieee80211-wpa-rsn-p1.md` | `rtw_ieee80211.c` WPA/RSN cipher getters |
| W3-28 | `wave3-28-ieee80211-wpa-rsn-p2.md` | `rtw_ieee80211.c` WPA/RSN IE parse |
| W3-29 | `wave3-29-ieee80211-wapi-wps.md` | `rtw_ieee80211.c` WAPI/WPS/sec-IE |
| W3-30 | `wave3-30-ieee80211-mac-str.md` | `rtw_ieee80211.c` string/MAC helpers |
| W3-31 | `wave3-31-ieee80211-chbw.md` | `rtw_ieee80211.c` chbw grouping/sync |
| W3-32 | `wave3-32-ieee80211-frame-ht.md` | `rtw_ieee80211.c` frame header + HT MCS |
| W3-33 | `wave3-33-rm-util-pure.md` | `rtw_rm_util.c` RM pure helpers |
| W3-34 | `wave3-34-rm-util-tokens.md` | `rtw_rm_util.c` RM token generation |
| W3-35 | `wave3-35-vht-mcs-nss.md` | `rtw_vht.c` VHT MCS/NSS helpers |
| W3-36 | `wave3-36-vht-ie-restructure.md` | `rtw_vht.c` VHT IE restructure |
| W3-37 | `wave3-37-sta-mgt-acl.md` | `rtw_sta_mgt.c` match rule + ACL |
| W3-38 | `wave3-38-sta-mgt-aid.md` | `rtw_sta_mgt.c` AID + pre-link sta |
| W3-39 | `wave3-39-recv-leaf.md` | `rtw_recv.c` recv leaf helpers |
| W3-40 | `wave3-40-xmit-rate-bmp.md` | `rtw_xmit.c` tx rate bitmap helpers |

Further ~200 LOC slices from larger protocol TUs; file new `wave3-*.md` issues as needed:

- `rtw_rf.c` remainder (regd_exc, txpwr_lmt, op_class_pref, dump_*)
- `rtw_pwrctrl.c`, `rtw_iol.c`, `rtw_sreset.c`, `rtw_roch.c`
- `rtw_recv.c`, `rtw_xmit.c`, `rtw_cmd.c`
- `rtw_mlme.c`, `rtw_mlme_ext.c`, `rtw_ap.c`, remaining `rtw_ieee80211.c`
- `rtw_sta_mgt.c`, `rtw_vht.c`, smaller `rtw_*.c` (sreset, iol, roch, …)

Wave 3 milestone (L4): WPA2 STA associate + encrypted ping when hardware available (same bar as Wave 2).
