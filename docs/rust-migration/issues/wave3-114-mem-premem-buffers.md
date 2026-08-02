---
title: "[W3-114] mem premem buffer helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-114
epic: E05
blocked_by: [W3-113]
estimate_loc: 170
---

## Goal

Port helpers from [`core/rtw_mem.c`](../../../core/rtw_mem.c) to [`rust/rtw_mem.rs`](../../../rust/rtw_mem.rs):

- `rtw_get_buf_premem`
- `rtw_rtkm_get_buff_size`
- `rtw_rtkm_get_nr_recv_skb`
- `rtw_alloc_skb_premem`
- `rtw_free_skb_premem`

## Notes

- Premem skb pool helpers; low HAL coupling — first slice of `rtw_mem.c`.
- Module init/exit (`rtw_mem_init` / `rtw_mem_exit`) stay in C until a later issue.
- **Kbuild / `CONFIG_*` (default 88x2bu profile):** `core/rtw_mem.c` is **not linked**
  into `88x2bu.ko` today (no `rtw_mem.o` in the top-level `Makefile`). Premem helpers
  are additionally gated behind **`CONFIG_PREALLOC_RX_SKB_BUFFER`** (off in the default
  config). At implement time, add a guarded `rtk_core += core/rtw_mem.o` entry (or port
  from the file without linking) and enable the premem config before L0 can satisfy L1
  for these symbols — or defer to a Wave 4 / optional-build tranche.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for mem premem buffer helpers
