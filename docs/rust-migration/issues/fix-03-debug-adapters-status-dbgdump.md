---
title: "[FX-03] debug: dump_adapters_status(RTW_DBGDUMP, …) prints nothing"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-03
epic: E14
blocked_by: []
estimate_loc: 20
---

## Priority

**P1 (high, diagnostics)** — default build; loses the dump on anomaly paths.

## Problem

[`rust/rtw_debug_adapters_status.rs`](../../../rust/rtw_debug_adapters_status.rs)
(9e8afd2d, #1387):

```rust
if sel.is_null() || dvobj.is_null() { return; }
```

`RTW_DBGDUMP` is `0` ([`include/rtw_debug.h`](../../../include/rtw_debug.h) line 69):
a NULL `sel` means "print to the kernel log". The original C never tested `sel`.
Every in-kernel caller passes `RTW_DBGDUMP`, including
`core/rtw_mlme_ext.c:14774` and `:14829`, right before `rtw_warn_on(1)`.
Only the `/proc` reader (non-NULL `seq_file`) still prints.

## Fix

Check only `dvobj`. Add a `// NULL sel == RTW_DBGDUMP (kernel log)` comment so
the rule is not "fixed" back.

## Acceptance

- L0; L1 unchanged.
- `llvm-objdump -d rust/rtw_debug_adapters_status.o` no longer tests `%rdi` before
  calling `dump_mi_status`.

## Notes

- Audit done during review: no other kernel-path Rust export rejects NULL `sel`
  (`rtw_mi_dump_status_host_types.rs` is host-only).
