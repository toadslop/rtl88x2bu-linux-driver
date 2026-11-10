---
title: "[FX-12] hal_com: rtw_sec_read_cam_ent writes to NULL+2 when mac is NULL"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-12
epic: E14
blocked_by: []
estimate_loc: 30
---

## Priority

**P3 (low/medium)** — exported C API contract break; no current caller hits it.

## Problem

[`rust/hal_com_sec_cam_ent.rs`](../../../rust/hal_com_sec_cam_ent.rs) (7d4204f4 W4-55 PR1, #1436):

```rust
1 => unsafe { cp(mac.add(2), r.as_ptr(), 4) },
_ => unsafe { cp(key.add(((i - 2) * 4) as usize), r.as_ptr(), 4) },
```

`cp()` null-checks only `dst`. With `(ctrl != NULL, mac == NULL, key != NULL)`
the range is `0..=5`, so `mac.add(2)` = `0x2` passes the check and 4 bytes are
written to address 2 (and `.add` on null is UB). C guarded with `if (mac)` /
`if (key)` before computing the offset. Reproduced in a host harness
(`memcpy dst=0x2 len=4`).

Current callers: `(ctrl, mac, key)` in `core/rtw_debug*.c`, `(NULL, NULL, key)` in
`hal/hal_com*.c`, `(ctrl, NULL, NULL)` at `hal/hal_com.c:2334`.

## Fix

Guard before the offset: `1 if !mac.is_null() => …`, `_ if !key.is_null() => …`.

## Acceptance

- L0; L1 unchanged.
- L2 host vector for `(ctrl, NULL, key)`: no write outside `ctrl`/`key`, `key` filled
  identically to the C oracle.
