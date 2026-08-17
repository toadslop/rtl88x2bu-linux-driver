# Rust ecosystem strategy

This document is normative for **post-parity** work: reusing crates from the wider
Rust ecosystem where it makes sense, and extracting reusable pieces of this driver
for community contribution. It complements
[`architecture.md`](architecture.md) (layering) and
[`test-plan.md`](test-plan.md) (L2 parity gates).

**Phase 1 is unchanged:** port C → Rust with byte/return parity and domain types.
Do not swap in external crates or publish libraries while behavior is still being
frozen — the C oracle and L2 vectors are the contract.

## Goals

1. **Adopt existing Rust crates** for generic primitives (AES, SHA-256, GCM, etc.)
   once parity is locked and the dependency is viable inside an RfL kernel module.
2. **Extract standalone crates** for 802.11 wire-format and security logic that
   is useful beyond this Realtek driver, and contribute them upstream (crates.io
   and/or shared kernel helpers as RfL matures).

## Non-goals

- A generic **WiFi driver toolkit** (USB HAL, firmware download, `cfg80211`
  integration, chip-specific state machines). Those stay in the driver and RfL
  shims — see architecture layering.
- Replacing ported code **before** characterization tests exist.
- Cargo-only builds that drop the Kbuild/RfL path for `88x2bu.ko`.

## When this work runs

| Phase | Ecosystem work |
|-------|----------------|
| **Phase 1** (parity) | Faithful ports only. L2 host harness builds `.rs` sources directly with `rustc` (`host_crypto_test`, etc.) — no `Cargo.toml` required yet. |
| **Phase 2** (idiomatic) | **Evaluate crate adoption** module-by-module: swap hand-rolled primitives where L2 (and L4 where relevant) stay green. Start a **Cargo workspace** alongside Kbuild for shared logic. |
| **Phase 3** (ecosystem) | **Publish extracted crates**, trim driver-local duplicates, document contribution paths. Driver may `path =` / vendor / git-depend on extracted crates. |

Phase 3 starts after Phase 1 exit (no remaining C objects, STA smoke green). Phase 2
and Phase 3 can overlap per module once that module’s parity tests are stable.

## Layering: what can move out

From [`architecture.md`](architecture.md):

```text
os / usb / netdev / cfg80211     →  stays in driver (RfL shims)
domain services (mlme, xmit, …)  →  mostly driver-specific; extract only pure subsets
domain types + pure crypto/IE    →  adoption + extraction candidates
abi / ffi                        →  shrinks in Phase 2; not published as crates
```

## Track A — Adopt existing Rust crates

### Decision criteria

Adopt a crates.io (or git) dependency only when **all** of the following hold:

1. **Parity** — existing L2 vectors (and any module-specific oracles) pass
   unchanged against the new implementation. Intentional differences are a spec
   change: update tests in the same PR and document why.
2. **`no_std` + license** — crate works in the kernel module context
   (`#![no_std]` where required) and is license-compatible with GPL-2.0
   (driver is GPL-2.0; prefer `MIT OR Apache-2.0` deps or explicit GPL compatibility).
3. **RfL / Kbuild integration** — dependency can be built and linked into
   `88x2bu.ko` (via `rust/` workspace crate consumed by Kbuild, or vendored
   sources). Document the exact integration in the PR.
4. **Binary size & audit surface** — acceptable `.ko` growth and maintainer
   willingness to track the dependency (security advisories, MSRV alignment with
   the pinned kernel `rustc`).

### Likely mapping (evaluate, do not assume)

| Our code (today) | Existing ecosystem | Notes |
|------------------|-------------------|--------|
| `aes_internal`, `aes_internal_enc` | [`aes`](https://crates.io/crates/aes), [`cipher`](https://crates.io/crates/cipher) | Generic block cipher; high value **if** L2 stays bit-identical or spec change is justified. |
| `sha256_internal`, `sha256` | [`sha2`](https://crates.io/crates/sha2), [`digest`](https://crates.io/crates/digest) | Strong candidate once HMAC/PRF wrappers are re-tested. |
| `aes_gcm`, GHASH pieces | [`aes-gcm`](https://crates.io/crates/aes-gcm) | Kernel `no_std` + constant-time expectations need review. |
| `aes_ccm`, `ccmp`, `gcmp` | [`ccm`](https://crates.io/crates/ccm), [`aes`](https://crates.io/crates/aes) | **802.11 framing and PN/key handling stay ours**; may adopt primitive layers only. |
| `rtw_ieee80211` IE walk/parse | thinner ecosystem | Prefer **extract** (Track B) over forcing a poor-fit dependency. |
| `rtw_chplan` | none direct | Regulatory tables are Realtek-heavy; extract only pure math/helpers. |

**Default stance:** generic crypto → adopt mature crates in Phase 2;
802.11-specific composition → keep or extract, do not force-fit a generic crate.

### Process per module

```text
1. Module at Phase 1 parity (L0 + L1 + L2 green)
2. Spike: branch + swap implementation behind same public Rust API / extern "C" shims
3. Re-run full L2 suite; L3 if linked into .ko; L4 if security path touched
4. Merge with CHANGELOG note: "adopted <crate> for <symbols>"
```

Keep `extern "C"` symbol names stable until the ABI layer is deliberately removed in
Phase 2.

## Track B — Extract standalone crates

### Extraction candidates (priority order)

| Crate (working name) | Source in tree | Audience |
|----------------------|----------------|----------|
| `wlan-types` | `rust/domain/types.rs` | MAC addresses, cipher enums, channel/bandwidth newtypes — `no_std` + optional `std`. |
| `ieee80211-ie` | `rtw_ieee80211` (+ later IE builders) | Management-frame IE parse/build without driver globals. |
| `wlan-crypto` | CCMP/GCMP/Tkip-adjacent paths, `rtw_swcrypto` pure slices | 802.11 security primitives not covered cleanly by generic `aes-gcm` alone. |
| `wlan-chplan` (optional) | Pure helpers from `rtw_chplan` | Only if separated from Realtek static tables. |

Each extracted crate should:

- Ship with the **same L2 vectors** (or a subset) as its test suite on crates.io CI.
- Use **`no_std` by default**, `std` feature for host development.
- Carry a clear **scope line** in the README (wire-format helpers, not a driver framework).
- Use a license chosen for ecosystem uptake (e.g. `MIT OR Apache-2.0` for libraries,
  while the driver remains GPL-2.0 — dual-licensing the extracted code may require
  copyright/attribution review before publish).

### Target repo layout (additive)

Kbuild remains authoritative for `88x2bu.ko`. A workspace is **additive**:

```text
rust/
  crates/
    wlan-types/       # Cargo crate
    ieee80211-ie/
    wlan-crypto/
  aes_ctr.rs          # Kbuild units; depend on workspace crates via path / include
  ...
```

Driver modules today duplicate `domain/types.rs` via `#[path]` per Kbuild unit
(see `rust/aes_ctr.rs`). Consolidation into `wlan-types` removes drift and is a
prerequisite for publishing.

### Extraction process

```text
1. Create workspace crate; move logic + L2 tests
2. Driver Kbuild unit depends on crate (path dependency initially)
3. L0–L3 green on driver; crate tests green in isolation
4. Publish to crates.io (or contribute to an existing project if overlap)
5. Driver switches to crates.io version pin; document in release notes
```

## Build and test contract

- **L2 remains the oracle** for both adoption and extraction — never drop
  differential tests without equivalent crate-local coverage.
- Host harness (`tests/host/`) should grow to compile workspace crates where
  practical, not only monolithic `.rs` files.
- Kernel build: document any `RUSTFLAGS` / overflow-check alignment already used
  in host L2 (see `tests/host/crypto/Makefile`).

## PR checklist (ecosystem changes)

Use in addition to the standard verification block in
[`rust-migration.md`](../rust-migration.md):

```markdown
## Ecosystem
- [ ] Parity: L2 (and L1 if extern "C") unchanged unless spec change called out
- [ ] Dependency: `no_std` + license + rustc pin noted
- [ ] Scope: adopt vs extract — fits Track A or B above
- [ ] Not a driver-framework / HAL abstraction creep
```

## Related epics

- **E09** — Phase 2 idiomatic Rust (crate adoption starts here)
- **E12** — Distribution (driver DKMS/releases; separate from crates.io publish)
- **E13** — Ecosystem crates (draft epic; children filed after Phase 1 exit)
