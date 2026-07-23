---
title: "[Epic] Phase 1 — Exact C→Rust translation"
labels: [rust-migration, phase-1]
type: epic
id: E01
---

## Goal

Mechanically translate the out-of-tree `88x2bu` driver from C to Rust while preserving behavior, C ABI, layouts, and symbol names. Prefer `unsafe` / `extern "C"` / `#[repr(C)]` over idiomatic redesign.

## Out of scope

- Idiomatic Rust refactors (Phase 2)
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
