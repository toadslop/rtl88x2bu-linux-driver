---
title: "[T17] Host WAPI SMS4 differential harness (L2)"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T17
epic: E10
blocked_by: []
estimate_loc: 200
---

## Goal

Add a host-side L2 oracle for the pure SMS4 block cipher helpers in
[`core/rtw_wapi_sms4.c`](../../../core/rtw_wapi_sms4.c) (`SMS4Crypt`, `SMS4KeyExt`,
`xor_block`, and round constants), so Wave 3 WAPI slices can swap Rust objects with
differential tests (same pattern as T2/T5).

## Notes

- Scope is **algorithm-only** — no `_adapter` / xmitframe paths in this harness.
- Reuse `tests/host/crypto` layout and Makefile patterns; vectors may be derived from
  known SMS4 test keys/blocks (document provenance in the test file).
- Does not require hardware or QEMU beyond existing CI host-l2 workflow path filters
  (extend filters in the same PR or a tiny follow-up nit if needed).

## Acceptance

- `make -C tests/host/wapi_sms4 all` (or equivalent path) builds and passes locally
- CI host-l2 job runs the new target on PRs touching the harness or `rtw_wapi_sms4.c`
- Document the gate in [`test-plan.md`](../../test-plan.md) (one paragraph)
