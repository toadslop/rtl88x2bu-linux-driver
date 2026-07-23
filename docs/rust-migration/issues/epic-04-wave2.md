---
title: "[Epic] Wave 2 — Leaf / pure unit translation"
labels: [rust-migration, phase-1, wave-2]
type: epic
id: E04
blocked_by: [E03]
---

## Goal

Translate bottom-up, low-kernel-coupling units starting with `core/crypto/*`, in ~200 LOC chunks.

## Verification gate

Per-PR: L0 build + L1 symbols + L2 host crypto vectors ([test-plan.md](../test-plan.md)).  
Wave milestone (L4): WPA2 STA associate + encrypted ping when hardware is available.

## Children

See `wave2-*.md`. Further leaf helpers (`rtw_chplan` slices, etc.) get new issues when crypto is nearly done.
