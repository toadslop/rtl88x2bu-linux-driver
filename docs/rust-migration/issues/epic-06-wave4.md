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

## Children (tranche 4 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-14 | `wave4-14-hal-legal-channel.md` | `HAL_IsLegalChannel` |
| W4-15 | `wave4-15-hal-out-pipe-mapping.md` | `Hal_MappingOutPipe` + queue→pipe tables |
| W4-16 | `wave4-16-hal-mgntframe-raid.md` | `rtw_get_mgntframe_raid` |
| W4-17 | `wave4-17-hal-hdata-rate-strings.md` | `_HDATA_RATE` / `HDATA_RATE` strings |
| W4-18 | `wave4-18-hal-phy-rf-shadow-core.md` | RF shadow read/write/refresh |
| W4-19 | `wave4-19-hal-phy-rf-shadow-flags.md` | RF shadow compare/recover flags |

## Children (tranche 5 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-20 | `wave4-20-hal-phy-rf-shadow-compare.md` | RF shadow compare/recover MMIO |
| W4-21 | `wave4-21-hal-halmac-drv-network-port.md` | HW port + network drv→halmac |
| W4-22 | `wave4-22-hal-halmac-rf-type.md` | RF type drv↔halmac |
| W4-23 | `wave4-23-hal-halmac-usb-fifo-mappers.md` | USB mode + FIFO sel mappers |
| W4-24 | `wave4-24-hal-halmac-fw-led-phy-param.md` | FW mem, LED, phy param val mappers |
| W4-25 | `wave4-25-hal-usb-scaffold.md` | `rust/hal_usb.rs` Kbuild scaffold |

Further slices: `hal_usb` I/O paths, `hal_dm.c` / PHYDM debug — file as `wave4-*.md` when tranche 6 is underway.
