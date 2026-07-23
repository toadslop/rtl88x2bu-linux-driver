---
title: "[A1] Seed rust/domain/types (MacAddr + validated buffer pattern)"
labels: [rust-migration, phase-1, size/~200]
type: child
id: A1
epic: E11
blocked_by: [W0-02, A0]
estimate_loc: 200
---

## Goal

Add the first domain types used by the crypto pilot and later ports:

- `MacAddr` (or defer if unused by pilot — then ship `KeyMaterial` / `AesKey` length-checked newtypes instead)
- Pattern: checked `try_from` / `from_raw` / `to_raw`, `#[repr(transparent)]` where ABI-adjacent
- Host unit tests for accept/reject cases

## Acceptance

- Types compile in module and/or host test crate
- Invalid lengths/values cannot construct the type without `Result` error
- Document how W1-03 should use them at the Rust API while keeping `extern "C"` shim for C
