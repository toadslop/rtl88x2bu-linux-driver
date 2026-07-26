# Rust migration issue tracker

Work items are tracked on **GitHub Issues**. Draft specs remain in this directory as the source of truth for titles, labels, dependencies, and acceptance criteria.

- **Issue map:** [`ISSUE-MAP.md`](ISSUE-MAP.md) — draft ID ↔ GitHub number
- **GitHub:** https://github.com/toadslop/rtl88x2bu-linux-driver/issues

Landed docs (implementation, not GitHub Issues):

- [`docs/rust-migration.md`](../../rust-migration.md) — phases, ABI, build contract, PR checklist
- [`docs/smoke-test.md`](../../smoke-test.md) — L4 hardware STA checklist
- [`../architecture.md`](../architecture.md) — domain types / layering
- [`../test-plan.md`](../test-plan.md) — L0–L4 gates
- [`../dev-environment.md`](../dev-environment.md) — toolchain / pinned kernel / L3 QEMU gotchas (Wave 0)

## How to use

1. Browse or filter issues on GitHub (labels: `rust-migration`, `wave-*`, `phase-*`, `size/~200`).
2. To file **new** draft issues from markdown in this directory, run from the repo root:

```bash
bash docs/rust-migration/issues/file-issues.sh
```

That script creates labels (if needed), opens epic issues, then child issues, and maintains `ISSUE-MAP.md` with number ↔ draft-id mappings.

Properties:

- **Idempotent** — skips draft IDs already in `ISSUE-MAP.md` (or matching open/closed titles); appends instead of truncating the map
- **Dependencies** — appends a Tracking footer with resolved `#N` links for `epic` / `blocked_by`; on re-run, refreshes **only** the `## Tracking` section when it still contains `(not filed yet)` (use `FORCE_REFRESH=1` to rewrite Tracking anyway)
- **Links** — rewrites relative markdown links to `https://github.com/<repo>/blob/<ref>/...` using the repo **default branch** (override with `FILE_ISSUES_REF=<sha-or-branch>`). Prefer running after merge to the default branch so links stay stable.

## Sizing

Each implementable child targets about **~200 lines** of meaningful change (roughly 150–250). Do not bundle unrelated work.

## Verification (required)

Hardware is **not** the default gate. See [`../test-plan.md`](../test-plan.md) and [`../architecture.md`](../architecture.md):

- **Characterize C behavior → freeze Rust tests → port** (parity first)
- **L0 build** + **L1 symbols** on every C→Rust swap
- **L2 host/unit differential tests** for crypto/pure chunks
- **Domain types** at Rust APIs; raw pointers only in abi/os shims
- **L3 VM insmod** when touching init; **L4 hardware** at wave milestones

Test-infra: #73 (`E10`), #75–#80 (`T0`–`T5`)  
Architecture: #74 (`E11`), #81–#84 (`A0`–`A3`)

## Filing cadence

- **Filed (Wave 0–3 tranche 1):** phase/wave epics (#64–#74) + Wave 0–2 children + test-infra + architecture children; Wave 2 chplan tail (W2-17…W2-20); Wave 3 tranche 1 (W3-01…W3-09). See [`ISSUE-MAP.md`](ISSUE-MAP.md).
- **Later:** Wave 3 tranche 2 (large `core/` protocol TUs) and Wave 4–6 / Phase 2 children when the previous tranche is mostly done.

## Status

| ID | GitHub | File | Status |
|----|--------|------|--------|
| E01–E11 | #64–#74 | `epic-*.md` | open (epics) |
| W0-01 | #85 | `wave0-01-docs.md` | done |
| W0-02 | #86 | `wave0-02-kbuild.md` | done |
| W0-03 | #87 | `wave0-03-scaffold.md` | done |
| W1-01 | #88 | `wave1-01-bindgen.md` | done |
| W1-02 | #89 | `wave1-02-ffi-module.md` | done |
| T2 | #77 | `test-02-host-crypto-harness.md` | done |
| A1 | #82 | `arch-01-domain-types-seed.md` | done |
| W1-03 | #90 | `wave1-03-pilot-aes-ctr.md` | done |
| W1-04 | #91 | `wave1-04-pilot-makefile-smoke.md` | done (folded into W1-03) |
| W2-01 | #92 | `wave2-01-aes-omac1.md` | done |
| W2-02 | #93 | `wave2-02-gcmp.md` | done |
| W2-03 | #94 | `wave2-03-aes-siv.md` | done |
| W2-04 | #95 | `wave2-04-aes-ccm.md` | done |
| W2-05 | #96 | `wave2-05-sha256-internal.md` | done |
| W2-06 | #97 | `wave2-06-sha256-prf-wrap.md` | done |
| W2-07 | #98 | `wave2-07-aes-gcm-part1.md` | done |
| W2-08 | #99 | `wave2-08-aes-gcm-part2.md` | done |
| W2-09 | #100 | `wave2-09-ccmp-part1.md` | done |
| W2-10 | #101 | `wave2-10-ccmp-part2.md` | done |
| W2-11 | #102 | `wave2-11-aes-internal-part1.md` | done |
| W2-12 | #103 | `wave2-12-aes-internal-part2.md` | done |
| W2-13 | #104 | `wave2-13-aes-internal-part3.md` | done |
| W2-14 | #105 | `wave2-14-aes-internal-part4.md` | done |
| W2-15 | #106 | `wave2-15-aes-internal-enc.md` | done |
| W2-16 | #107 | `wave2-16-sha256.md` | done |
| W2-17 | #108 | `wave2-17-chplan-lookup.md` | done (#57) |
| W2-18 | #109 | `wave2-18-chplan-dfs.md` | done (#58) |
| W2-19 | #110 | `wave2-19-chplan-country.md` | done (#62) |
| W2-20 | #111 | `wave2-20-chplan-init.md` | draft |
| T0 / A0 | #75 / #81 | `test-00-*.md` / `arch-00-*.md` | done |
| T1 | #76 | `test-01-symbol-check.md` | done (supersedes pre-migration #12) |
| T3 | #78 | `test-03-ci-host-tests.md` | done (`.github/workflows/host-l2.yml`) |
| T4 | #79 | `test-04-host-chplan-harness.md` | done (#59) |
| T5 | #80 | `test-05-host-security-wlan-harness.md` | draft |
| A2 | #83 | `arch-02-domain-types-chplan.md` | draft |
| A3 | #84 | `arch-03-domain-types-security.md` | draft |
<<<<<<< HEAD
| W3-01 | #112 | `wave3-01-swcrypto-ccmp-gcmp.md` | done (#60) |
| W3-02 | #113 | `wave3-02-swcrypto-bip-tdls.md` | done (#63) |
| W3-03 … W3-09 | #114–#120 | `wave3-*.md` | draft / in progress (Wave 3 tranche 1 — see #68) |
=======
| W3-01 | #112 | `wave3-01-swcrypto-ccmp-gcmp.md` | in progress (#60) |
| W3-02 … W3-09 | #113–#120 | `wave3-*.md` | draft / in progress (Wave 3 tranche 1 — see #68) |
=======
| ID | File | Status |
|----|------|--------|
| E01–E09 | `epic-*.md` | draft (not on GitHub yet) |
| W0-01 | `wave0-01-docs.md` | done (docs landed; not filed on GitHub) |
| W0-02 | `wave0-02-kbuild.md` | done (KDIR/LLVM + CONFIG_RUST stub link; not filed on GitHub) |
| W0-03 | `wave0-03-scaffold.md` | done (scaffold + C init call; not filed on GitHub) |
| W1-01 | `wave1-01-bindgen.md` | done (bindgen script + allowlisted AES bindings; not filed on GitHub) |
| W1-02 | `wave1-02-ffi-module.md` | done (rust/ffi ownership map + re-exports; not filed on GitHub) |
| T2 | `test-02-host-crypto-harness.md` | done (host aes-ctr vectors + C oracle runner; not filed on GitHub) |
| A1 | `arch-01-domain-types-seed.md` | done (AesKey + AesCtrNonce + host tests; not filed on GitHub) |
| W1-03 | `wave1-03-pilot-aes-ctr.md` | done (Rust aes-ctr + L2 host test; not filed on GitHub) |
| W1-04 | `wave1-04-pilot-makefile-smoke.md` | done (folded into W1-03 Makefile swap; not filed on GitHub) |
| W2-01 | `wave2-01-aes-omac1.md` | done (Rust aes-omac1 + L2 vectors; not filed on GitHub) |
| W2-02 | `wave2-02-gcmp.md` | done (Rust gcmp + L2 vectors; not filed on GitHub) |
| W2-03 | `wave2-03-aes-siv.md` | done (Rust aes-siv + L2 vectors; not filed on GitHub) |
| W2-04 | `wave2-04-aes-ccm.md` | done (Rust aes-ccm + L2 vectors; not filed on GitHub) |
| W2-05 | `wave2-05-sha256-internal.md` | done (Rust sha256-internal + L2 vectors; not filed on GitHub) |
| W2-06 | `wave2-06-sha256-prf-wrap.md` | done (sha256-prf + rtw_crypto_wrap; not filed on GitHub) |
| W2-07 | `wave2-07-aes-gcm-part1.md` | done (Rust aes_gcm_ae + L2 vectors; not filed on GitHub) |
| W2-08 | `wave2-08-aes-gcm-part2.md` | done (aes_gcm_ad/gmac; no aes-gcm*.c in CONFIG_RUST build; not filed on GitHub) |
| W2-09 | `wave2-09-ccmp-part1.md` | done (Rust ccmp decrypt + ccmp_rest.c; not filed on GitHub) |
| W2-10 | `wave2-10-ccmp-part2.md` | done (Rust ccmp encrypt; no ccmp*.c in CONFIG_RUST build; not filed on GitHub) |
| W2-11 | `wave2-11-aes-internal-part1.md` | done (Rust Te0 + aes-internal_rest.c; not filed on GitHub) |
| W2-12 | `wave2-12-aes-internal-part2.md` | done (Rust Td0; not filed on GitHub) |
| W2-13 | `wave2-13-aes-internal-part3.md` | done (Rust Td4s/rcons; not filed on GitHub) |
| W2-14 | `wave2-14-aes-internal-part4.md` | done (Rust rijndaelKeySetupEnc; no aes-internal*.c in CONFIG_RUST build; not filed on GitHub) |
| W2-15 | `wave2-15-aes-internal-enc.md` | done (Rust aes_encrypt_*; not filed on GitHub) |
| W2-16 | `wave2-16-sha256.md` | done (Rust hmac_sha256_vector; no sha256.c in CONFIG_RUST build; not filed on GitHub) |
| T0 / A0 | `test-00-*.md` / `arch-00-*.md` | done (plan docs + links via W0-01) |
| T1 | `test-01-symbol-check.md` | done (#12) |
| T3 | `test-03-ci-host-tests.md` | done (GitHub Actions host-l2.yml; not filed on GitHub) |
| W2-17 | `wave2-17-chplan-lookup.md` | done (chplan lookup L2 + Rust port; not filed on GitHub) |
| W2-18 | `wave2-18-chplan-dfs.md` | done (DFS helpers L2 + Rust; not filed on GitHub) |
| W2-19 | `wave2-19-chplan-country.md` | done (country lookup L2 + Rust; not filed on GitHub) |
| W3-01 | `wave3-01-swcrypto-ccmp-gcmp.md` | done (swcrypto CCMP/GCMP Rust; not filed on GitHub) |
| W3-02 | `wave3-02-swcrypto-bip-tdls.md` | done (swcrypto BIP/TDLS/mesh Rust; not filed on GitHub) |
| W3-03 | `wave3-03-ie-parse.md` | done (IE parse helpers L2 + Rust; not filed on GitHub) |
