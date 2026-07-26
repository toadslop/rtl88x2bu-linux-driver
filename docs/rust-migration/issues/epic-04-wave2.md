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

See `wave2-*.md`. **W2-01…W2-16** (crypto) are done. **W2-17…W2-20** (`rtw_chplan.c`) close out Wave 2 leaf work:

| ID | File | Focus |
|----|------|--------|
| W2-17 | `wave2-17-chplan-lookup.md` | Pure getters; tables stay in C (`extern`) |
| W2-18 | `wave2-18-chplan-dfs.md` | DFS + exclusion helpers |
| W2-19 | `wave2-19-chplan-country.md` | Country lookup (8822BU generic map) |
| W2-20 | `wave2-20-chplan-init.md` | `init_channel_set` (beacon hint / dump_* deferred) |

Supporting: **T4** (host chplan harness — implemented on `cursor/w2-17a-chplan-harness-3dd4`), **A2** (channel/rate domain types).

## Chplan table strategy (canonical)

Static channel-plan tables (`RTW_ChannelPlanMap`, `country_chplan_map`, channel defs, …) **remain in C**. Rust lookup ports read them via `extern "C"`. Migrating table data into Rust `const` arrays is a separate future issue, not part of W2-17…W2-20.
