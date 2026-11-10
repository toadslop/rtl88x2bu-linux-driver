---
title: "[FX-18] debug procs: input validation in moved setters (pre-existing C bugs)"
labels: [rust-migration, phase-1, size/~100]
type: child
id: FX-18
epic: E14
blocked_by: [FX-13]
estimate_loc: 80
---

## Priority

**P3** — pre-existing C behaviour carried verbatim into the `rtw_rust_debug_proc_*`
helpers; mostly behind non-default features, but root can corrupt kernel memory.

## Problem

Found while diffing moved helpers against `commit^` (behaviour was preserved, so
these are **not** port regressions):

- MDNS `txt_rsp` setter (#1458): no `count > max_input_size` check before
  `copy_from_user` into a vmalloc'd buffer (heap overflow); `strlen(txt_str) - 1`
  on an empty string.
- WAR offload IPv4/IPv6 setters (#1435, #1439): `index - 1` not range-checked; IPv6
  `memcpy` runs even when `num != 9`.
- `service_info` (#1457): `idx` not bounded by `MAX_MDNS_SERVICE_NUM`; negative
  `txt_idx` accepted.
- `%s` `sscanf` on non-NUL-terminated `tmp` (`domain_name`, `rson`, `survey`), and
  setters with `char tmp[N]` that skip termination when `count == N`.

## Fix

Bound every index, NUL-terminate `tmp` after `copy_from_user`, reject oversize
input with `-EINVAL`. Mark the PR as a **spec change** (behaviour differs from the
original C for invalid input).

## Acceptance

- L0 default and with `CONFIG_WAR_OFFLOAD`/MDNS enabled.
- L2 debug-proc vectors for each out-of-range input return the new error without
  touching memory past the buffer.

## Notes

- **Dependency rationale:** same files as FX-13 (`core/rtw_debug.c`, `core/rtw_debug_rest.c`).
