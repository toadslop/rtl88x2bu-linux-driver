---
title: "[W1-03] Pilot: exact-translate core/crypto/aes-ctr.c to Rust"
labels: [rust-migration, phase-1, wave-1, size/~200]
type: child
id: W1-03
epic: E03
blocked_by: [W1-02]
estimate_loc: 120
---

## Goal

Port [`core/crypto/aes-ctr.c`](../../../core/crypto/aes-ctr.c) with **parity to characterized C behavior**, using domain types at the Rust API and a thin `extern "C"` shim for remaining C callers.

## Deliverables

- Characterization vectors already green on C (T2) — extend if gaps found
- Typed Rust implementation (e.g. length-checked key/nonce types from A1)
- `extern "C"` shim preserving original symbol names/ABI
- Remove C `.o` from link once L0–L2 green

## Acceptance

- **Characterize→test→port** order followed
- **L0 / L1 / L2** green; Rust API not a raw `*mut u8` soup
- Diff stays near ~200 meaningful lines (split shim vs types if needed)
- L4 hardware is a Wave 1 milestone only

## Out of scope

- Intentional crypto behavior changes (parity only)
- Translating other crypto files (Wave 2)
