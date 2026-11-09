---
title: "[W3-232] Translate rtw_debug_rest.c — PCI conf dumps + ASPM readout"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-232
epic: E05
blocked_by: [W3-231]
estimate_loc: 180
---

## Goal

Port PCI configuration **read/dump** procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_pci.rs`](../../../rust/rtw_debug_proc_pci.rs):

- `proc_get_pci_bridge_conf_space`
- `proc_get_pci_conf_space`
- `proc_get_pci_aspm`

## Notes

- ASPM path reads PCI express link cap/ctl and `rtw_hal_pci_*` helpers via FFI.
- Same Rust module as W3-231 — chain only for Makefile/kbuild ordering, not functional coupling.
- `proc_get_tx_ring_ext` / setter deferred (see W3-233).

## Acceptance

- L0 with `CONFIG_PCI_HCI`
- L1 on all three getters
- L2: golden substring checks for ASPM enable/disable banners (host oracle with stub pci_read hooks)
