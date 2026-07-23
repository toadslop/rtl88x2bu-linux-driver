---
title: "[Epic] Wave 2 — Leaf / pure unit translation"
labels: [rust-migration, phase-1, wave-2]
type: epic
id: E04
blocked_by: [E03]
---

## Goal

Translate bottom-up, low-kernel-coupling units starting with `core/crypto/*`, in ~200 LOC chunks.

## Smoke gate

WPA2 STA associate + encrypted ping/traffic after each crypto PR that touches the data path; otherwise Wave 0 load/iface gate.

## Children

See `wave2-*.md`. Further leaf helpers (`rtw_chplan` slices, etc.) get new issues when crypto is nearly done.
