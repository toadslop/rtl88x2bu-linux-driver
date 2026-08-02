---
title: "[Epic] Wave 3 — Core protocol translation"
labels: [rust-migration, phase-1, wave-3]
type: epic
id: E05
blocked_by: [E04]
---

## Goal

Translate `core/` protocol files (cmd/io/security/xmit/recv/mlme/…) behind stable `extern "C"` boundaries, sliced into ~200 LOC issues.

**Status:** see open/closed children on GitHub (`label:rust-migration` + `wave-3`).
This file is planning structure only — not a second tracker.

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

## Children (tranche 3 — filed)

| ID | File | Focus |
|----|------|--------|
| W3-41 | `wave3-41-ieee80211-rate-section.md` | `rtw_ieee80211.c` rate-section + ch-offset mapping |
| W3-42 | `wave3-42-ieee80211-ht-mcs-amsdu.md` | `rtw_ieee80211.c` HT MCS bitmap + AMSDU mode |
| W3-43 | `wave3-43-ieee80211-p2p-ie.md` | `rtw_ieee80211.c` P2P IE merge/delete |
| W3-44 | `wave3-44-ieee80211-wfd-multiap.md` | `rtw_ieee80211.c` WFD + multi-AP IE helpers |
| W3-45 | `wave3-45-vht-mcs-rate.md` | `rtw_vht.c` VHT MCS/rate pure helpers |
| W3-46 | `wave3-46-recv-llc-ethhdr.md` | `rtw_recv.c` LLC parse + ethhdr + BMC gate |
| W3-47 | `wave3-47-recv-pn-decache.md` | `rtw_recv.c` PN replay decache |
| W3-48 | `wave3-48-xmit-qos-submit-ctx.md` | `rtw_xmit.c` QoS/SNAP/submit context |
| W3-49 | `wave3-49-xmit-rate-bmp-aggregate.md` | `rtw_xmit.c` aggregate rate bitmap + RA short GI |
| W3-50 | `wave3-50-iol-append-cmds.md` | `rtw_iol.c` IOL command-buffer append encoders |
| W3-51 | `wave3-51-rf-regd-exc.md` | `rtw_rf.c` regd_exc list CRUD + search |
| W3-52 | `wave3-52-rf-txpwr-lmt.md` | `rtw_rf.c` txpwr_lmt list CRUD |
| W3-53 | `wave3-53-mlme-bssid-compare.md` | `rtw_mlme.c` BSSID getters + same-network compare |
| W3-54 | `wave3-54-mlmeext-chset-nonocp.md` | `rtw_mlme_ext.c` chset non-OCP + search/valid |
| W3-55 | `wave3-55-sta-mgt-stctl-ap-tim.md` | `rtw_sta_mgt.c` st_ctl + `rtw_ap.c` TIM/VAPID |

## Children (tranche 4 — filed)

| ID | File | Focus |
|----|------|--------|
| W3-56 | `wave3-56-rf-op-class-pref.md` | `rtw_rf.c` op_class_pref lifecycle + regulatory apply |
| W3-57 | `wave3-57-rf-op-class-dump.md` | `rtw_rf.c` op-class debug dump helpers |
| W3-58 | `wave3-58-rf-dump-txpwr-lmt.md` | `rtw_rf.c` dump_txpwr_lmt formatter |
| W3-59 | `wave3-59-rf-kfree-tx-gain.md` | `rtw_rf.c` kfree TX gain offset apply |
| W3-60 | `wave3-60-cmd-priv-init.md` | `rtw_cmd.c` cmd/evt priv init/teardown |
| W3-61 | `wave3-61-cmd-queue-filter.md` | `rtw_cmd.c` cmd/evt queue enqueue/filter |
| W3-62 | `wave3-62-mlme-unassoc-sta.md` | `rtw_mlme.c` unassoc STA queue helpers |
| W3-63 | `wave3-63-mlme-wmm-rsn-ie.md` | `rtw_mlme.c` WMM/RSN IE restructure |
| W3-64 | `wave3-64-mlme-roaming-candidate.md` | `rtw_mlme.c` roaming candidate check/select |
| W3-65 | `wave3-65-mlme-network-update.md` | `rtw_mlme.c` scanned network update merge |
| W3-66 | `wave3-66-mlme-80211d.md` | `rtw_mlme.c` 802.11d country IE processing |
| W3-67 | `wave3-67-mlme-ht-ie-restructure.md` | `rtw_mlme.c` HT IE restructure |
| W3-68 | `wave3-68-mlmeext-mgnt-attrib.md` | `rtw_mlme_ext.c` mgnt frame attribute builders |
| W3-69 | `wave3-69-mlmeext-peer-alive.md` | `rtw_mlme_ext.c` delba/peer-alive checks |
| W3-70 | `wave3-70-mlmeext-scan-decision.md` | `rtw_mlme_ext.c` scan sparse/channel decision |
| W3-71 | `wave3-71-mlmeext-pick-ch.md` | `rtw_mlme_ext.c` sitesurvey channel pick |
| W3-72 | `wave3-72-mlmeext-band-ie-format.md` | `rtw_mlme_ext.c` band-change beacon IE update (`CONFIG_AP_MODE`) |
| W3-73 | `wave3-73-ap-sta-ie-parse.md` | `rtw_ap.c` STA assoc IE parse (cap/rates/HT/VHT) |
| W3-74 | `wave3-74-ap-security-ie-parse.md` | `rtw_ap.c` STA security IE parse |
| W3-75 | `wave3-75-ap-beacon-ie-update.md` | `rtw_ap.c` beacon TIM/generic IE update |

## Children (tranche 5 — filed)

| ID | File | Focus |
|----|------|--------|
| W3-76 | `wave3-76-ieee80211-parse-elems.md` | `rtw_ieee80211.c` element parse (`parse_elems`) |
| W3-77 | `wave3-77-sta-mgt-lookup-init.md` | `rtw_sta_mgt.c` stainfo init + hash lookup |
| W3-78 | `wave3-78-sta-mgt-alloc.md` | `rtw_sta_mgt.c` stainfo alloc |
| W3-79 | `wave3-79-sta-mgt-free-priv.md` | `rtw_sta_mgt.c` stainfo free + sta priv lifecycle |
| W3-80 | `wave3-80-ap-bmc-rate.md` | `rtw_ap.c` BMC multicast tx rate |
| W3-81 | `wave3-81-ap-beacon-ht-wps-update.md` | `rtw_ap.c` beacon HT/WPS/ERP refresh |
| W3-82 | `wave3-82-ap-sta-expire-timeout.md` | `rtw_ap.c` sta keepalive + expire timeout |
| W3-83 | `wave3-83-ap-update-sta-info.md` | `rtw_ap.c` assoc sta info update |
| W3-84 | `wave3-84-vht-build-handlers.md` | `rtw_vht.c` VHT IE build + assoc handlers |
| W3-85 | `wave3-85-recv-sta-validate-hdr.md` | `rtw_recv.c` sta rx validate + stats |
| W3-86 | `wave3-86-xmit-update-attrib.md` | `rtw_xmit.c` update_attrib builders |
| W3-87 | `wave3-87-mlme-join-select.md` | `rtw_mlme.c` join candidate select |
| W3-88 | `wave3-88-mlmeext-join-cmd-hdl.md` | `rtw_mlme_ext.c` join_cmd_hdl |
| W3-89 | `wave3-89-mlmeext-sitesurvey-hdl-p1.md` | `rtw_mlme_ext.c` sitesurvey cmd handler (enter/process) |
| W3-90 | `wave3-90-mlmeext-sitesurvey-hdl-p2.md` | `rtw_mlme_ext.c` sitesurvey cmd handler (backop/complete) |
| W3-91 | `wave3-91-cmd-thread.md` | `rtw_cmd.c` cmd thread loop |
| W3-92 | `wave3-92-cmd-join-drvextra.md` | `rtw_cmd.c` joinbss + drvextra dispatch |
| W3-93 | `wave3-93-cmd-traffic-lps-watchdog.md` | `rtw_cmd.c` traffic/LPS dynamic watchdog |
| W3-94 | `wave3-94-pwrctrl-ps-gate-lps.md` | `rtw_pwrctrl.c` ps deny gate + LPS enter/leave |
| W3-95 | `wave3-95-sreset-lifecycle.md` | `rtw_sreset.c` silent reset lifecycle |
| W3-96 | `wave3-96-roch-handlers.md` | `rtw_roch.c` remain-on-channel handlers |
| W3-97 | `wave3-97-roch-concurrent-init.md` | `rtw_roch.c` concurrent roch + init |

## Children (tranche 6 — filed)

| ID | File | Focus |
|----|------|--------|
| W3-98 | `wave3-98-p2p-channel-leaf.md` | `rtw_p2p.c` channel/negotiation leaf |
| W3-99 | `wave3-99-p2p-ie-builders.md` | `rtw_p2p.c` P2P IE frame builders |
| W3-100 | `wave3-100-p2p-wfd-beacon-probe.md` | `rtw_p2p.c` WFD IE builders (beacon/probe) |
| W3-101 | `wave3-101-p2p-wfd-probe-assoc.md` | `rtw_p2p.c` WFD IE builders (probe-resp/assoc) |
| W3-102 | `wave3-102-tdls-lifecycle.md` | `rtw_tdls.c` lifecycle + prohibited checks |
| W3-103 | `wave3-103-tdls-ht-vht-cap.md` | `rtw_tdls.c` HT/VHT cap process |
| W3-104 | `wave3-104-nat25-addr-hash.md` | `rtw_br_ext.c` NAT25 addr/hash leaf |
| W3-105 | `wave3-105-nat25-db.md` | `rtw_br_ext.c` NAT25 DB lifecycle |
| W3-106 | `wave3-106-rson-leaf.md` | `rtw_rson.c` score/IE/choose leaf |
| W3-107 | `wave3-107-mbo-ie-leaf.md` | `rtw_mbo.c` IE parse/build leaf |
| W3-108 | `wave3-108-wnm-btm-parse.md` | `rtw_wnm.c` BTM parse/reset leaf |
| W3-109 | `wave3-109-wapi-pn-cam.md` | `rtw_wapi.c` PN/IE/CAM table leaf |
| W3-110 | `wave3-110-ft-ie-build.md` | `rtw_ft.c` IE update/build leaf |
| W3-111 | `wave3-111-rm-parse-subelem.md` | `rtw_rm.c` meas subelem parse |
| W3-112 | `wave3-112-rm-fsm-queue.md` | `rtw_rm_fsm.c` obj/queue/clock |
| W3-113 | `wave3-113-mp-pmac-generators.md` | `rtw_mp.c` PMAC sig generators |

## Children (tranche 7 — filed)

| ID | File | Focus |
|----|------|--------|
| W3-114 | `wave3-114-mem-premem-buffers.md` | `rtw_mem.c` premem buffer helpers |
| W3-115 | `wave3-115-sdio-cmd-io.md` | `rtw_sdio.c` cmd52/53 I/O wrappers |
| W3-116 | `wave3-116-eeprom-bitbang.md` | `rtw_eeprom.c` bit-bang primitives |
| W3-117 | `wave3-117-eeprom-read-write.md` | `rtw_eeprom.c` read/write API |
| W3-118 | `wave3-118-odm-phydm-init.md` | `rtw_odm.c` phydm ability + IC init |
| W3-119 | `wave3-119-odm-adaptivity-leaf.md` | `rtw_odm.c` adaptivity msg/parm leaf |
| W3-120 | `wave3-120-odm-radar-txpwr.md` | `rtw_odm.c` radar detect + tx power leaf |
| W3-121 | `wave3-121-mi-ch-union.md` | `rtw_mi.c` channel union + stay-in checks |
| W3-122 | `wave3-122-mi-status-leaf.md` | `rtw_mi.c` status + check_status leaf |
| W3-123 | `wave3-123-mi-netif-buddy.md` | `rtw_mi.c` netif buddy queue/carrier leaf |
| W3-124 | `wave3-124-ioctl-connect-leaf.md` | `rtw_ioctl_set.c` validate + connect/disassociate |
| W3-125 | `wave3-125-ioctl-scan-channel.md` | `rtw_ioctl_set.c` scan/auth/channel setters |
| W3-126 | `wave3-126-btcoex-init-notify.md` | `rtw_btcoex.c` init + notify leaf |
| W3-127 | `wave3-127-btcoex-handler-policy.md` | `rtw_btcoex.c` handler + AMPDU policy leaf |
| W3-128 | `wave3-128-bf-entry-packet-leaf.md` | `rtw_beamforming.c` entry lookup + packet leaf |
| W3-129 | `wave3-129-bf-init-cmd-leaf.md` | `rtw_beamforming.c` init + cmd_hdl leaf |

Further ~200 LOC slices **not yet covered by W3-01…W3-129**; file new `wave3-*.md` issues as needed when tranche 7 closes:

- `core/` files not yet sliced: remaining `rtw_beamforming.c` sounding state machine, `rtw_btcoex*.c` remainder, `rtw_debug.c`, `rtw_ioctl_query.c`, `rtw_wapi_sms4.c`, …
- Remaining helpers in partially translated files after W3-129 lands (audit with migration progress / `grep`)

Wave 3 milestone (L4): WPA2 STA associate + encrypted ping when hardware available (same bar as Wave 2).
