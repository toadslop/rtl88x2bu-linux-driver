# Issue map

Generated/updated by `docs/rust-migration/issues/file-issues.sh`.

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
| T0 | #75 | [T0] Land offline test plan + PR verification checklist |
| T1 | #76 | [T1] Add check-symbols.sh for C→Rust object ABI gate (L1) |
| T2 | #77 | [T2] Host differential test harness for crypto (L2) + aes-ctr vectors |
| T3 | #78 | [T3] CI: run host L2 tests on every PR |
| T4 | #79 | [T4] Host chplan differential harness + vectors |
| T5 | #80 | [T5] Host security + wlan_util differential harness |
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

## Superseded issues

Some work was tracked before the bulk migration (`file-issues.sh`). Prefer the canonical GitHub number from the table above:

| Draft ID | Canonical | Superseded | Notes |
|----------|-----------|------------|-------|
| T1 | #76 | #12 | Same title; filed manually before the migration tracker (no `rust-migration` label) |
