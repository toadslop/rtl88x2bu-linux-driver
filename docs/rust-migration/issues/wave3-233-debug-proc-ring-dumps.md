---
title: "[W3-233] Translate rtw_debug_rest.c — TRX ring debug dumps"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-233
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port TRX ring dump procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_rings.rs`](../../../rust/rtw_debug_proc_rings.rs):

- `proc_get_rx_ring`
- `proc_get_tx_ring`
- `proc_get_tx_ring_ext` / `proc_set_tx_ring_ext` (`DBG_TXBD_DESC_DUMP`)

## Notes

- Holds `irq_th_lock` around dumps — export lock enter/exit via existing dvobj FFI or keep critical sections in C wrappers.
- Mostly formatted register/descriptor output; L2 uses frozen layout strings with minimal stub rings.
- Parallel lane (disjoint from `rtw_debug_proc_pci.rs`).

## Acceptance

- L0 with `CONFIG_PCI_HCI` + optional `DBG_TXBD_DESC_DUMP`
- L1 on listed handlers
- L2: empty ring header lines; tx_ring_ext disabled banner when dump flag clear
