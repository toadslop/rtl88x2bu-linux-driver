---
title: "[W1-01] Bindgen script + allowlisted bindings for pilot headers"
labels: [rust-migration, phase-1, wave-1, size/~200]
type: child
id: W1-01
epic: E03
blocked_by: [W0-03]
estimate_loc: 200
---

## Goal

Add a reproducible bindgen (or equivalent) pipeline that generates Rust bindings from the C headers needed by the crypto pilot, with allowlists so review stays tractable.

## Deliverables

- `scripts/bindgen_rtw.sh` (or similar)
- Generated output under `rust/bindings/` (generated blob may be large; keep handmade script/wrapper near ~200 LOC)
- Short note in `docs/rust-migration.md` on how to regenerate

## Acceptance

- Script runs against the pinned kernel/headers setup documented in Wave 0
- Bindings compile as part of the module (or as a crate unit linked into it)

## Out of scope

- Translating crypto `.c` files (W1-03+)
- Full `drv_types.h` surface if not needed by the pilot
