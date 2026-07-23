---
title: "[Epic] Domain-typed architecture + characterization tests"
labels: [rust-migration, phase-1]
type: epic
id: E11
blocked_by: []
---

## Goal

Enforce the architecture in [`docs/rust-migration/architecture.md`](../architecture.md): strong validated domain types, unsafe at edges, and characterization tests that freeze C behavior before/with each port.

## Children

- A0 — land architecture doc + wire into W0-01 / PR checklist
- A1 — initial `domain/types` seed (MacAddr, length-checked buffers, pattern for `from_raw`/`to_raw`)
- Characterization remains part of T2+ and every Wave 1/2 child (not optional)

## Non-negotiable

Ported Rust modules must not expose new public APIs as raw pointers / unvalidated integers. ABI shims may, temporarily, for C.
