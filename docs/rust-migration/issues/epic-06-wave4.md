---
title: "[Epic] Wave 4 — HAL / HALMAC / PHYDM / USB HCI"
labels: [rust-migration, phase-1, wave-4]
type: epic
id: E06
blocked_by: [E05]
---

## Goal

Translate hardware-facing HAL, HALMAC, PHYDM, BTC, and USB HCI paths with identical register/USB semantics.

## Children (tranche 1 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-01 | `wave4-01-hal-hw-rate-map.md` | `hal_com.c` hw rate → mac rate map |
| W4-02 | `wave4-02-hal-rsvd-page-cache.md` | `hal_com.c` reserved page cache |
| W4-03 | `wave4-03-hal-dump-chip-info.md` | `hal_com.c` chip info debug dump |
| W4-04 | `wave4-04-hal-dump-macaddr.md` | `hal_com.c` MAC address debug dump |

Further slices: `hal_intf.c`, `hal_phy.c`, `hal_halmac.c`, USB HCI — file as `wave4-*.md` when tranche 1 is underway.
