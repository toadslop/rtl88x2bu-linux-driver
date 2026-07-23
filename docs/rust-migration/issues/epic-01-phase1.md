---
title: "[Epic] Phase 1 — Exact C→Rust translation"
labels: [rust-migration, phase-1]
type: epic
id: E01
---

## Goal

Port the out-of-tree `88x2bu` driver from C to Rust with **observable behavior parity** (characterization tests), while adopting the layered architecture in [`architecture.md`](../architecture.md): domain types in Rust services, raw `extern "C"` / `#[repr(C)]` only at the mixed-C ABI edge.

## Out of scope

- Intentional behavior changes (those are later, with test updates)
- Full Phase 2 cleanup (RfL-heavy, delete ABI layer)
- Rewriting onto mac80211 / in-tree rtw88 architecture
- PCI/SDIO HCI product configurations (USB 8822B only for exit criteria)

## Child waves

- Wave 0 — build scaffold
- Wave 1 — bindgen / FFI + pilot
- Wave 2 — leaf/pure units (crypto first)
- Waves 3–6 — core, HAL, os_dep, `module!` (children filed later)

## Exit criteria

- Default `CONFIG_RTL8822B=y` + `CONFIG_USB_HCI=y` build has no remaining `.c` objects (headers may remain for bindgen)
- Module loads on real hardware with STA feature parity for supported USB IDs
