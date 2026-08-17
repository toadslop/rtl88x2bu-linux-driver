---
title: "[Epic] Phase 3 — Rust ecosystem (adopt + extract crates)"
labels: [rust-migration, phase-3]
type: epic
id: E13
blocked_by: [E01]
---

## Goal

After Phase 1 behavior parity: (1) replace hand-rolled primitives with mature Rust
ecosystem crates where L2 parity holds, and (2) extract reusable 802.11 wire-format
and security logic as standalone crates for community contribution.

Normative plan: [`docs/rust-migration/ecosystem.md`](../ecosystem.md).

## Context

- Phase 1 ports intentionally mirror C for oracle tests; several crypto units are
  hand-rolled (`aes_internal`, `sha256_internal`, CCMP/GCMP composition).
- Generic crates (`aes`, `sha2`, `aes-gcm`, `ccm`, …) overlap parts of Wave 2;
  802.11-specific layers and IE helpers are gaps worth publishing.
- Kbuild compiles each `rust/*.rs` as its own unit today; `domain/types.rs` is
  duplicated via `#[path]` — consolidation into workspace crates is a prerequisite
  for extraction.

## Children

**Not filed yet.** Slice after Phase 1 exit. Suggested first children:

- E13-01 — Cargo workspace scaffold (`rust/crates/`) + `wlan-types` from `domain/types`
- E13-02 — Evaluate `sha2` adoption for `sha256_internal` / `sha256` (L2 gate)
- E13-03 — Evaluate `aes` + `cipher` adoption for `aes_internal` (L2 gate)
- E13-04 — Extract `ieee80211-ie` crate from `rtw_ieee80211` pure helpers
- E13-05 — Extract `wlan-crypto` (CCMP/GCMP framing) after workspace deps land

Late Phase 2 may start E13-02/E13-03 early per module once that module’s L2 suite
is stable (see ecosystem.md “When this work runs”).

## Exit criteria

- At least one workspace crate published or ready to publish with L2-equivalent tests
- Driver builds via Kbuild against path or crates.io deps without losing L0–L3
- Documented license and scope for each published crate (not a driver toolkit)
- Remaining hand-rolled crypto either justified in ecosystem.md or slated for adoption

## Non-goals

- Generic WiFi driver / HAL / USB toolkit
- Dropping GPL-2.0 driver license
- Phase 1 parity work or crate swaps without L2 coverage
