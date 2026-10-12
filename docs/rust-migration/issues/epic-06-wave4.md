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

## Children (tranche 2 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-05 | `wave4-05-hal-mrate-to-hw-rate.md` | `hal_com_rest.c` `MRateToHwRate` table |
| W4-06 | `wave4-06-hal-dump-fw-info.md` | `hal_com_rest.c` `rtw_dump_fw_info` |
| W4-07 | `wave4-07-hal-intf-dump-trx-mode.md` | `hal_intf.c` TRX mode debug dumps |
| W4-08 | `wave4-08-hal-intf-dump-rf-nss.md` | `hal_intf.c` RF path + NSS dumps |

## Children (tranche 3 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-09 | `wave4-09-hal-phy-bit-shift.md` | `hal_phy.c` `PHY_CalculateBitShift` |
| W4-10 | `wave4-10-hal-halmac-enum-mappers.md` | `hal_halmac.c` network + beacon ctrl mappers |
| W4-11 | `wave4-11-hal-halmac-rsvd-page-map.md` | `hal_halmac.c` reserved page count mappers |
| W4-12 | `wave4-12-hal-halmac-trx-share-dump.md` | `hal_halmac.c` TRX share mode dump |
| W4-13 | `wave4-13-hal-set-brate-cfg.md` | `hal_com_rest.c` `HalSetBrateCfg` |

Further slices: USB HCI, `hal_dm.c` / PHYDM debug — file as `wave4-*.md` when tranche 3 is underway.
