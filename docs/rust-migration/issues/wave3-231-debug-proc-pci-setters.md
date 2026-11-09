---
title: "[W3-231] Translate rtw_debug_rest.c — PCI conf space setters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-231
epic: E05
blocked_by: []
estimate_loc: 140
---

## Goal

Port PCI configuration **write** procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_pci.rs`](../../../rust/rtw_debug_proc_pci.rs):

- `proc_set_pci_bridge_conf_space`
- `proc_set_pci_conf_space`

(`CONFIG_PCI_HCI`)

## Notes

- Uses `pci_write_config_byte` on device / bridge — keep in thin C FFI shims; Rust owns sscanf validation + logging.
- Relevant for multi-HCI builds; 88x2bu USB profile may compile these out — L0 uses PCI-enabled `.config` fragment from test-plan.
- Root of PCI debug proc lane.

## Acceptance

- L0 with `CONFIG_PCI_HCI`
- L1 on both setters
- L2: invalid reg/value clamp vectors; valid byte write logging hook
