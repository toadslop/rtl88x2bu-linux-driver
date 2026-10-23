# Issue map (filing registry)

**Not a status tracker.** Open/closed state and dependencies live on GitHub Issues
only. This file records which draft IDs have been filed and their GitHub numbers.

Generated/updated by `docs/rust-migration/issues/file-issues.sh` — do not edit by
hand except to fix filing mistakes, then prefer re-running the script.

| Draft ID | GitHub | Title |
|----------|--------|-------|
| E01 | #64 | [Epic] Phase 1 — Exact C→Rust translation |
| E02 | #65 | [Epic] Wave 0 — RfL out-of-tree build scaffold |
| E03 | #66 | [Epic] Wave 1 — Bindgen / FFI seam + pilot |
| E04 | #67 | [Epic] Wave 2 — Leaf / pure unit translation |
| E05 | #68 | [Epic] Wave 3 — Core protocol translation |
| E06 | #69 | [Epic] Wave 4 — HAL / HALMAC / PHYDM / USB HCI |
| E07 | #70 | [Epic] Wave 5 — os_dep/linux glue |
| E08 | #71 | [Epic] Wave 6 — RfL module! entry |
| E09 | #72 | [Epic] Phase 2 — Idiomatic Rust / reduce unsafe |
| E10 | #73 | [Epic] Offline test infrastructure for Rust migration |
| E11 | #74 | [Epic] Domain-typed architecture + characterization tests |
| E12 | #150 | [Epic] Distribution — DKMS packages and GitHub Releases |
| T0 | #75 | [T0] Land offline test plan + PR verification checklist |
| T1 | #76 | [T1] Add check-symbols.sh for C→Rust object ABI gate (L1) |
| T2 | #77 | [T2] Host differential test harness for crypto (L2) + aes-ctr vectors |
| T3 | #78 | [T3] CI: run host L2 tests on every PR |
| T4 | #79 | [T4] Host chplan differential harness + vectors |
| T5 | #80 | [T5] Host security + wlan_util differential harness |
| T6 | #151 | [T6] CI: L0 module build against pinned Rust kernel |
| T7 | #152 | [T7] CI: L1 symbol/ABI checks on C→Rust swaps |
| T8 | #153 | [T8] CI: L3 insmod/rmmod in QEMU on master merges |
| T9 | #154 | [T9] Enforce merge gates: branch protection, PR template, required checks |
| R1 | #155 | [R1] GitHub Releases: versioning, DKMS tarball, merge pipeline |
| A0 | #81 | [A0] Land architecture.md and PR architecture checklist |
| A1 | #82 | [A1] Seed rust/domain/types (MacAddr + validated buffer pattern) |
| A2 | #83 | [A2] Domain types for channel plan + rates (Wave 3) |
| A3 | #84 | [A3] Domain types for security (Wave 3) |
| W0-01 | #85 | [W0-01] Document Rust migration + smoke-test checklist |
| W0-02 | #86 | [W0-02] Kbuild: allow linking a Rust object into 88x2bu |
| W0-03 | #87 | [W0-03] Add trivial rust/scaffold.rs and call from C init |
| W1-01 | #88 | [W1-01] Bindgen script + allowlisted bindings for pilot headers |
| W1-02 | #89 | [W1-02] Add rust/ffi module documenting C vs Rust ownership |
| W1-03 | #90 | [W1-03] Pilot: exact-translate core/crypto/aes-ctr.c to Rust |
| W1-04 | #91 | [W1-04] Pilot Makefile object swap + smoke verification |
| W2-01 | #92 | [W2-01] Translate core/crypto/aes-omac1.c |
| W2-02 | #93 | [W2-02] Translate core/crypto/gcmp.c |
| W2-03 | #94 | [W2-03] Translate core/crypto/aes-siv.c |
| W2-04 | #95 | [W2-04] Translate core/crypto/aes-ccm.c |
| W2-05 | #96 | [W2-05] Translate core/crypto/sha256-internal.c |
| W2-06 | #97 | [W2-06] Translate sha256-prf.c + rtw_crypto_wrap.c |
| W2-07 | #98 | [W2-07] Translate aes-gcm.c (part 1/2) |
| W2-08 | #99 | [W2-08] Translate aes-gcm.c (part 2/2) |
| W2-09 | #100 | [W2-09] Translate ccmp.c (part 1/2) |
| W2-10 | #101 | [W2-10] Translate ccmp.c (part 2/2) |
| W2-11 | #102 | [W2-11] Translate aes-internal.c (part 1/4) |
| W2-12 | #103 | [W2-12] Translate aes-internal.c (part 2/4) |
| W2-13 | #104 | [W2-13] Translate aes-internal.c (part 3/4) |
| W2-14 | #105 | [W2-14] Translate aes-internal.c (part 4/4) |
| W2-15 | #106 | [W2-15] Translate aes-internal-enc.c |
| W2-16 | #107 | [W2-16] Translate core/crypto/sha256.c |
| W2-17 | #108 | [W2-17] Translate rtw_chplan lookup helpers |
| W2-18 | #109 | [W2-18] Translate rtw_chplan DFS + exclusion helpers |
| W2-19 | #110 | [W2-19] Translate rtw_chplan country lookup |
| W2-20 | #111 | [W2-20] Translate rtw_chplan init_channel_set |
| W3-01 | #112 | [W3-01] Translate rtw_swcrypto CCMP/GCMP wrappers |
| W3-02 | #113 | [W3-02] Translate rtw_swcrypto BIP/SIV wrappers |
| W3-03 | #114 | [W3-03] Translate rtw_ieee80211 IE parse helpers |
| W3-04 | #115 | [W3-04] Translate rtw_security.c part 1 — type string helpers |
| W3-05 | #116 | [W3-05] Translate rtw_security.c part 2 — WEP ARC4/CRC32 primitives |
| W3-06 | #117 | [W3-06] Translate rtw_security.c part 3 — WEP encrypt/decrypt |
| W3-07 | #118 | [W3-07] Translate rtw_security.c part 4 — TKIP MIC + phase1/phase2 |
| W3-08 | #119 | [W3-08] Translate rtw_wlan_util.c part 1 — pure rate classification |
| W3-09 | #120 | [W3-09] Translate rtw_wlan_util.c part 2 — ratetbl + network type |
| W3-10 | #179 | [W3-10] Translate rtw_security_rest.c — TKIP frame encrypt/decrypt |
| W3-11 | #180 | [W3-11] Translate rtw_security_rest.c — AES-CCMP software primitives |
| W3-12 | #181 | [W3-12] Translate rtw_security_rest.c — AES-CCMP frame encrypt |
| W3-13 | #182 | [W3-13] Translate rtw_security_rest.c — AES-CCMP frame decrypt |
| W3-14 | #183 | [W3-14] Translate rtw_security_rest.c — GCMP frame encrypt/decrypt |
| W3-15 | #184 | [W3-15] Translate rtw_security_rest.c — misc security helpers |
| W3-16 | #185 | [W3-16] Translate rtw_security_rest.c — TDLS MIC helpers |
| W3-17 | #186 | [W3-17] Translate rtw_chplan_rest.c — beacon hint + debug dumps |
| W3-18 | #187 | [W3-18] Translate rtw_io_rest.c — continual I/O error + reg sniff matchers |
| W3-19 | #235 | [W3-19] Translate rtw_rf.c — channel layout helpers |
| W3-20 | #236 | [W3-20] Translate rtw_rf.c — channel/frequency conversion |
| W3-21 | #238 | [W3-21] Translate rtw_rf.c — lookup/format tables |
| W3-22 | #239 | [W3-22] Translate rtw_rf.c — global op-class lookup |
| W3-23 | #240 | [W3-23] Translate rtw_rf.c — RF type and trx path helpers |
| W3-24 | #241 | [W3-24] Translate rtw_rf.c — txpwr format and DFS CAC helpers |
| W3-25 | #242 | [W3-25] Translate rtw_rf.c — tx path NSS and bb gain sel |
| W3-26 | #243 | [W3-26] Translate rtw_ieee80211.c — rate and network type helpers |
| W3-27 | #244 | [W3-27] Translate rtw_ieee80211.c — WPA/RSN cipher suite getters |
| W3-28 | #245 | [W3-28] Translate rtw_ieee80211.c — WPA/RSN IE parse |
| W3-29 | #246 | [W3-29] Translate rtw_ieee80211.c — WAPI/WPS/sec-IE getters |
| W3-30 | #247 | [W3-30] Translate rtw_ieee80211.c — string and MAC address helpers |
| W3-31 | #248 | [W3-31] Translate rtw_ieee80211.c — chbw grouping and sync |
| W3-32 | #249 | [W3-32] Translate rtw_ieee80211.c — frame header and HT MCS helpers |
| W3-33 | #250 | [W3-33] Translate rtw_rm_util.c — radio measurement pure helpers |
| W3-34 | #251 | [W3-34] Translate rtw_rm_util.c — RM token generation |
| W3-35 | #252 | [W3-35] Translate rtw_vht.c — VHT MCS and NSS helpers |
| W3-36 | #253 | [W3-36] Translate rtw_vht.c — VHT IE restructure |
| W3-37 | #254 | [W3-37] Translate rtw_sta_mgt.c — match rule and access control |
| W3-38 | #255 | [W3-38] Translate rtw_sta_mgt.c — AID and pre-link sta helpers |
| W3-39 | #256 | [W3-39] Translate rtw_recv.c — recv leaf helpers |
| W3-40 | #257 | [W3-40] Translate rtw_xmit.c — tx rate bitmap helpers |
| T10 | #304 | [T10] CI: refactor workflow path filters for branch-protection compatibility |
| T11 | #305 | [T11] CI: extend L1 checks for *-rest translation units |
| T12 | #306 | [T12] CI: keep verify-ko-probes in sync with linked Rust modules |
| T13 | #307 | [T13] CI: close host-l2 path-filter gaps for *_rest.c sources |
| T14 | #308 | [T14] CI: bindgen drift check (generated.rs freshness) |
| T15 | #309 | [T15] CI: path-scoped L3 on PRs for init/USB/scaffold changes |
| T16 | #310 | [T16] CI: rustfmt --check on rust/** changes |
| W3-41 | #370 | [W3-41] Translate rtw_ieee80211.c — rate-section and ch-offset mapping |
| W3-42 | #371 | [W3-42] Translate rtw_ieee80211.c — HT MCS bitmap and AMSDU mode |
| W3-43 | #372 | [W3-43] Translate rtw_ieee80211.c — P2P IE merge and delete |
| W3-44 | #373 | [W3-44] Translate rtw_ieee80211.c — WFD and multi-AP IE helpers |
| W3-45 | #374 | [W3-45] Translate rtw_vht.c — VHT MCS and rate pure helpers |
| W3-46 | #375 | [W3-46] Translate rtw_recv.c — LLC parse, ethhdr, and BMC gate |
| W3-47 | #376 | [W3-47] Translate rtw_recv.c — PN replay decache |
| W3-48 | #377 | [W3-48] Translate rtw_xmit.c — QoS, SNAP, and submit context |
| W3-49 | #378 | [W3-49] Translate rtw_xmit.c — aggregate rate bitmap and RA short GI |
| W3-50 | #379 | [W3-50] Translate rtw_iol.c — IOL command-buffer append encoders |
| W3-51 | #380 | [W3-51] Translate rtw_rf.c — regd_exc list CRUD and search |
| W3-52 | #381 | [W3-52] Translate rtw_rf.c — txpwr_lmt list CRUD |
| W3-53 | #382 | [W3-53] Translate rtw_mlme.c — BSSID getters and same-network compare |
| W3-54 | #383 | [W3-54] Translate rtw_mlme_ext.c — chset non-OCP and search/valid |
| W3-55 | #384 | [W3-55] Translate sta_mgt st_ctl and AP TIM/VAPID helpers |
| W3-56 | #391 | [W3-56] Translate rtw_rf.c — op_class_pref lifecycle and regulatory apply |
| W3-57 | #392 | [W3-57] Translate rtw_rf.c — op-class debug dump helpers |
| W3-58 | #393 | [W3-58] Translate rtw_rf.c — dump_txpwr_lmt debug formatter |
| W3-59 | #394 | [W3-59] Translate rtw_rf.c — kfree TX gain offset apply |
| W3-60 | #395 | [W3-60] Translate rtw_cmd.c — cmd/evt priv init and teardown |
| W3-61 | #396 | [W3-61] Translate rtw_cmd.c — cmd/evt queue enqueue and filter |
| W3-62 | #397 | [W3-62] Translate rtw_mlme.c — unassociated STA queue helpers |
| W3-63 | #398 | [W3-63] Translate rtw_mlme.c — WMM/RSN IE restructure |
| W3-64 | #399 | [W3-64] Translate rtw_mlme.c — roaming candidate check and select |
| W3-65 | #400 | [W3-65] Translate rtw_mlme.c — scanned network update merge |
| W3-66 | #401 | [W3-66] Translate rtw_mlme.c — 802.11d country IE processing |
| W3-67 | #402 | [W3-67] Translate rtw_mlme.c — HT IE restructure |
| W3-68 | #403 | [W3-68] Translate rtw_mlme_ext.c — mgnt frame attribute builders |
| W3-69 | #404 | [W3-69] Translate rtw_mlme_ext.c — delba timeout and peer alive checks |
| W3-70 | #405 | [W3-70] Translate rtw_mlme_ext.c — scan sparse and channel decision |
| W3-71 | #406 | [W3-71] Translate rtw_mlme_ext.c — sitesurvey channel pick |
| W3-72 | #407 | [W3-72] Translate rtw_mlme_ext.c — band-change beacon IE update |
| W3-73 | #408 | [W3-73] Translate rtw_ap.c — STA assoc IE parse (cap/rates/HT/VHT) |
| W3-74 | #409 | [W3-74] Translate rtw_ap.c — STA security IE parse and policy |
| W3-75 | #410 | [W3-75] Translate rtw_ap.c — beacon TIM and generic IE update |
| W3-76 | #413 | [W3-76] element parse (parse_elems) |
| W3-77 | #414 | [W3-77] stainfo init and hash lookup |
| W3-78 | #415 | [W3-78] stainfo alloc |
| W3-79 | #416 | [W3-79] stainfo free and sta priv lifecycle |
| W3-80 | #417 | [W3-80] BMC multicast tx rate helpers |
| W3-81 | #418 | [W3-81] beacon HT/WPS/ERP refresh |
| W3-82 | #419 | [W3-82] sta keepalive and expire timeout |
| W3-83 | #420 | [W3-83] assoc sta info update |
| W3-84 | #421 | [W3-84] VHT IE build and assoc handlers |
| W3-85 | #422 | [W3-85] sta rx validate and stats |
| W3-86 | #423 | [W3-86] update_attrib builders |
| W3-87 | #424 | [W3-87] join candidate select |
| W3-88 | #425 | [W3-88] join_cmd_hdl |
| W3-89 | #426 | [W3-89] sitesurvey cmd handler (enter/process) |
| W3-90 | #427 | [W3-90] sitesurvey cmd handler (backop/complete) |
| W3-91 | #428 | [W3-91] cmd thread loop |
| W3-92 | #429 | [W3-92] joinbss and drvextra dispatch |
| W3-93 | #430 | [W3-93] traffic/LPS dynamic watchdog |
| W3-94 | #431 | [W3-94] ps deny gate and LPS enter/leave |
| W3-95 | #432 | [W3-95] silent reset lifecycle |
| W3-96 | #433 | [W3-96] remain-on-channel handlers |
| W3-97 | #434 | [W3-97] concurrent roch and init |
| W3-98 | #437 | [W3-98] p2p channel/negotiation leaf |
| W3-99 | #438 | [W3-99] p2p P2P IE frame builders |
| W3-100 | #439 | [W3-100] p2p WFD IE builders (beacon/probe) |
| W3-101 | #440 | [W3-101] p2p WFD IE builders (probe-resp/assoc) |
| W3-102 | #441 | [W3-102] tdls lifecycle + prohibited checks |
| W3-103 | #442 | [W3-103] tdls HT/VHT cap process |
| W3-104 | #443 | [W3-104] br_ext NAT25 addr/hash leaf |
| W3-105 | #444 | [W3-105] br_ext NAT25 DB lifecycle |
| W3-106 | #445 | [W3-106] rson score/IE/choose leaf |
| W3-107 | #446 | [W3-107] mbo IE parse/build leaf |
| W3-108 | #447 | [W3-108] wnm BTM parse/reset leaf |
| W3-109 | #448 | [W3-109] wapi PN/IE/CAM table leaf |
| W3-110 | #449 | [W3-110] ft IE update/build leaf |
| W3-111 | #450 | [W3-111] rm meas subelem parse |
| W3-112 | #451 | [W3-112] rm_fsm obj/queue/clock |
| W3-113 | #452 | [W3-113] mp PMAC sig generators |
| W3-114 | #454 | [W3-114] mem premem buffer helpers |
| W3-115 | #455 | [W3-115] sdio cmd52/53 I/O wrappers |
| W3-116 | #456 | [W3-116] eeprom bit-bang primitives |
| W3-117 | #457 | [W3-117] eeprom read/write API |
| W3-118 | #458 | [W3-118] odm phydm ability and IC init |
| W3-119 | #459 | [W3-119] odm adaptivity msg/parm leaf |
| W3-120 | #460 | [W3-120] odm radar detect and tx power leaf |
| W3-121 | #461 | [W3-121] mi channel union and stay-in checks |
| W3-122 | #462 | [W3-122] mi status and check_status leaf |
| W3-123 | #463 | [W3-123] mi netif buddy queue/carrier leaf |
| W3-124 | #464 | [W3-124] ioctl validate and connect/disassociate |
| W3-125 | #465 | [W3-125] ioctl scan/auth/channel setters |
| W3-126 | #466 | [W3-126] btcoex init and notify leaf |
| W3-127 | #467 | [W3-127] btcoex handler and AMPDU policy leaf |
| W3-128 | #468 | [W3-128] beamforming entry lookup and packet leaf |
| W3-129 | #469 | [W3-129] beamforming init and cmd_hdl leaf |
| T17 | #1139 | [T17] Host WAPI SMS4 differential harness (L2) |
| W3-130 | #1140 | [W3-130] Translate rtw_debug.c — driver version and log level dumps |
| W3-131 | #1141 | [W3-131] Translate rtw_debug.c — dump_drv_cfg Kconfig banner |
| W3-132 | #1142 | [W3-132] Translate rtw_debug.c — manufacturing/test hook globals |
| W3-133 | #1143 | [W3-133] Translate rtw_debug.c — security CAM dump helpers |
| W3-134 | #1144 | [W3-134] Translate rtw_debug.c — tx/rx debug dump leaf |
| W3-135 | #1145 | [W3-135] Translate rtw_wapi_sms4.c — SMS4 block cipher core |
| W3-136 | #1146 | [W3-136] Translate rtw_wapi_sms4.c — PN increment and SMS4 IV header |
| W3-137 | #1147 | [W3-137] Translate rtw_btcoex_wifionly.c — wifionly notify stubs |
| W4-01 | #1148 | [W4-01] Translate hal_com.c — hw_rate_to_m_rate + rate map dump |
| W4-02 | #1149 | [W4-02] Translate hal_com.c — reserved page cache helpers |
| W4-03 | #1150 | [W4-03] Translate hal_com.c — dump_chip_info formatter |
| W4-04 | #1151 | [W4-04] Translate hal_com.c — rtw_hal_dump_macaddr |
| W3-138 | #1153 | [W3-138] Translate rtw_debug.c — dump_drv_cfg HCI + xmit/recv banners |
| W3-139 | #1205 | [W3-139] Translate rtw_wapi_sms4.c — SMS4 OFB payload crypt |
| W3-140 | #1206 | [W3-140] Translate rtw_wapi_sms4.c — SMS4 MIC calculation |
| W3-141 | #1207 | [W3-141] Translate rtw_wapi_sms4.c — QoS PN cache and replay check |
| W4-05 | #1218 | [W4-05] Translate hal_com.c — MRateToHwRate lookup table |
| W4-06 | #1219 | [W4-06] Translate hal_com.c — rtw_dump_fw_info formatter |
| W4-07 | #1220 | [W4-07] Translate hal_intf.c — TRX mode debug dumps |
| W4-08 | #1221 | [W4-08] Translate hal_intf.c — RF path and TRX NSS dumps |
| W4-09 | #1235 | [W4-09] Translate hal_phy.c — PHY_CalculateBitShift |
| W4-10 | #1236 | [W4-10] Translate hal_halmac.c — HALMAC network/beacon ctrl mappers |
| W4-11 | #1237 | [W4-11] Translate hal_halmac.c — reserved page count mappers |
| W4-12 | #1238 | [W4-12] Translate hal_halmac.c — TRX share mode dump |
| W4-13 | #1239 | [W4-13] Translate hal_com_rest.c — HalSetBrateCfg |
| W4-14 | #1261 | [W4-14] Translate hal_com_rest.c — HAL_IsLegalChannel |
| W4-15 | #1262 | [W4-15] Translate hal_com_rest.c — USB out-pipe queue mapping |
| W4-16 | #1263 | [W4-16] Translate hal_com_rest.c — rtw_get_mgntframe_raid |
| W4-17 | #1264 | [W4-17] Translate hal_com_rest.c — HDATA_RATE descriptor strings |
| W4-18 | #1265 | [W4-18] Translate hal_phy_rest.c — RF shadow read/write/refresh |
| W4-19 | #1266 | [W4-19] Translate hal_phy_rest.c — RF shadow compare/recover flags |
| W4-20 | #1288 | [W4-20] Translate hal_phy_rest.c — RF shadow compare/recover |
| W4-21 | #1289 | [W4-21] Translate hal_halmac.c — HW port + network type drv→halmac |
| W4-22 | #1290 | [W4-22] Translate hal_halmac.c — RF type drv↔halmac mappers |
| W4-23 | #1291 | [W4-23] Translate hal_halmac.c — USB mode + FIFO sel mappers |
| W4-24 | #1292 | [W4-24] Translate hal_halmac.c — FW mem, LED mode, phy param val mappers |
| W4-25 | #1293 | [W4-25] Kbuild scaffold — rust/hal_usb.rs + hal_usb_rest.c split |
| W4-26 | #1314 | [W4-26] Translate hal_halmac.c — phy parameter struct drv→halmac |
| W4-27 | #1315 | [W4-27] Translate hal_halmac.c — rtw_halmac_get_version formatter |
| W4-28 | #1316 | [W4-28] Translate hal_usb.c — vendor wvalue WLANON page0 fixup |
| W4-29 | #1317 | [W4-29] Kbuild scaffold — rust/hal_dm.rs + hal_dm_rest.c split |
| W4-30 | #1318 | [W4-30] Translate hal_dm.c — Init_ODM_ComInfo board-type block |
| W4-31 | #1319 | [W4-31] Translate hal_dm.c — dump_sta_info debug formatter |
| W4-32 | #1320 | [W4-32] Translate hal_dm.c — dump_sta_traffic debug formatter |
| W4-33 | #1333 | [W4-33] Translate hal_usb.c — sync vendor read8/16/32 |
| W4-34 | #1334 | [W4-34] Translate hal_usb.c — sync vendor write8/16/32/N |
| W4-35 | #1335 | [W4-35] Translate hal_phy_rest.c — RF6052 max reg query |
| W4-36 | #1336 | [W4-36] Translate hal_intf.c — chip_type_to_odm_ic_type lookup |
| W4-37 | #1337 | [W4-37] Translate hal_usb.c — async vendor write helpers |
| W4-38 | #1338 | [W4-38] Translate hal_dm.c — Init_ODM big-endian hook block |
| W4-39 | #1339 | [W4-39] Translate hal_com.c — dump_tx_aclt_force_val formatter |
| W4-40 | #1340 | [W4-40] Translate hal_com.c — dump_tx_aclt_confs formatter |
| W3-142 | #1341 | [W3-142] Translate rtw_wapi_sms4_rest.c — frame encrypt/decrypt paths |
| W4-41 | #1373 | [W4-41] Translate hal_com.c — dump_hal_spec formatter |
| W4-42 | #1374 | [W4-42] Translate hal_com.c — dump_TX_FIFO WoW debug |
| W4-43 | #1375 | [W4-43] Translate hal_usb.c — usb_free_recv_priv teardown |
| W4-44 | #1376 | [W4-44] Translate hal_usb.c — usb_c2h_hisr_hdl |
| W3-143 | #1377 | [W3-143] Translate rtw_debug_rest.c — MAC/BB register dump leaf |
| W3-144 | #1378 | [W3-144] Translate rtw_debug_rest.c — dump_adapters_status |
| W4-45 | #1388 | [W4-45] Translate hal_usb.c — usb_init_recv_priv (part 1/2) |
| W4-46 | #1389 | [W4-46] Translate hal_usb.c — usb_init_recv_priv (part 2/2) |
| W4-47 | #1390 | [W4-47] Translate hal_com_rest.c — C2H register event read/clear |
| W4-48 | #1391 | [W4-48] Translate hal_com_rest.c — hw port query + HalData defaults |
| W4-49 | #1392 | [W4-49] Translate hal_com_rest.c — RTW_HALMAC hw port enable/disable |
| W3-145 | #1393 | [W3-145] Translate rtw_debug_rest.c — proc mlme/rf leaf getters |
| W3-146 | #1394 | [W3-146] Translate rtw_debug_rest.c — proc_get_sec_info |
| W3-147 | #1395 | [W3-147] Translate rtw_debug_rest.c — proc roam debug leaf |
| W3-148 | #1406 | [W3-148] Translate rtw_debug_rest.c — proc roam setters |
| W3-149 | #1407 | [W3-149] Translate rtw_debug_rest.c — proc defs + register peek/poke |
| W3-150 | #1408 | [W3-150] Translate rtw_debug_rest.c — proc rx/tx stainfo stats |
| W3-151 | #1409 | [W3-151] Translate rtw_debug_rest.c — proc scan param + abort |
| W4-50 | #1410 | [W4-50] Translate hal_com_rest.c — C2H IQK / txpwr offload wait |
| W4-51 | #1411 | [W4-51] Translate hal_com_rest.c — reqtxrpt + MI MAC addr push |
| W4-52 | #1412 | [W4-52] Translate hal_com_rest.c — security CAM register read/write |
| W4-53 | #1413 | [W4-53] Extend rust/hal_usb.rs usb_set_intf_ops — interrupt pipe + hook retarget |
| W3-152 | #1425 | [W3-152] Translate rtw_debug_rest.c — proc war offload enable + IPv4 |
| W3-153 | #1426 | [W3-153] Translate rtw_debug_rest.c — proc war offload IPv6 addr |
| W3-154 | #1427 | [W3-154] Translate rtw_debug_rest.c — proc war offload MDNS domain/machine |
| W3-155 | #1428 | [W3-155] Translate rtw_debug_rest.c — proc rson_data get/set |
| W3-156 | #1429 | [W3-156] Translate rtw_debug_rest.c — proc survey_info get/set |
| W4-54 | #1430 | [W4-54] Translate hal_com_rest.c — security CAM clear + GK probe |
| W4-55 | #1431 | [W4-55] Translate hal_com_rest.c — security CAM entry read/write |
| W4-56 | #1432 | [W4-56] Translate hal_com_rest.c — C2H defeature + customer string rpt |
| W3-157 | #1450 | [W3-157] Translate rtw_debug_rest.c — proc war offload MDNS service/txt |
| W3-158 | #1451 | [W3-158] Translate rtw_debug_rest.c — proc ap_info + infra_ap |
| W3-159 | #1452 | [W3-159] Translate rtw_debug_rest.c — proc trx_info get/reset |
| W3-160 | #1453 | [W3-160] Translate rtw_debug_rest.c — proc rate_ctl leaf getters |
| W4-57 | #1454 | [W4-57] Translate hal_com_rest.c — C2H mac hidden report |
| W4-58 | #1455 | [W4-58] Translate hal_com_rest.c — C2H per-rate + LPS status |

## Superseded issues

Some work was tracked before the bulk migration (`file-issues.sh`). Prefer the canonical GitHub number from the table above:

| Draft ID | Canonical | Superseded | Notes |
|----------|-----------|------------|-------|
| T1 | #76 | #12 | Same title; filed manually before the migration tracker (no `rust-migration` label) |
