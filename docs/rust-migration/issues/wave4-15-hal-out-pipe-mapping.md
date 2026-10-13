---
title: "[W4-15] Translate hal_com_rest.c — USB out-pipe queue mapping"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-15
epic: E06
blocked_by: [W4-14]
estimate_loc: 200
---

## Goal

Port USB queue-to-pipe assignment from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `_OneOutPipeMapping`
- `_TwoOutPipeMapping`
- `_ThreeOutPipeMapping`
- `Hal_MappingOutPipe`

## Notes

- Writes `dvobj_priv.Queue2Pipe[]` from `RtOutPipe[]` and `wifi_spec` (WMM vs typical).
- Same-file follow-up in the `hal_com` lane; shares `rust/hal_com.rs` and Makefile object line with W4-14.
- **Dependency rationale:** sequential edits to `hal_com.rs` / `hal_com_rest.c` extraction in this lane.

## Acceptance

- L0 + L1 for `Hal_MappingOutPipe` (static helpers stay internal to the Rust module or `#[no_mangle]` test exports as needed)
- L2: host vectors for `NumOutPipe` 1/2/3, WMM on/off, assert full `Queue2Pipe[0..7]` arrays
