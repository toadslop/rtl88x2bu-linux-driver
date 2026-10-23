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

## Children (tranche 6 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-26 | `wave4-26-hal-halmac-phy-parameter.md` | `_phy_parameter_drv2halmac` struct mapper |
| W4-27 | `wave4-27-hal-halmac-version-string.md` | `rtw_halmac_get_version` formatter |
| W4-28 | `wave4-28-hal-usb-vendor-wvalue.md` | USB vendor wvalue page0 fixup |
| W4-29 | `wave4-29-hal-dm-scaffold.md` | `rust/hal_dm.rs` Kbuild scaffold |
| W4-30 | `wave4-30-hal-dm-board-type.md` | `Init_ODM_ComInfo` board-type block |
| W4-31 | `wave4-31-hal-dm-dump-sta-info.md` | `dump_sta_info` debug dump |
| W4-32 | `wave4-32-hal-dm-dump-sta-traffic.md` | `dump_sta_traffic` debug dump |

## Children (tranche 7 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-33 | `wave4-33-hal-usb-sync-read.md` | `hal_usb.c` sync vendor read8/16/32 |
| W4-34 | `wave4-34-hal-usb-sync-write.md` | `hal_usb.c` sync vendor write8/16/32/N |
| W4-35 | `wave4-35-hal-phy-rf-max-reg.md` | `hal_phy_rest.c` RF6052 max reg query |
| W4-36 | `wave4-36-hal-intf-chip-odm-map.md` | `hal_intf.c` chip_type → ODM IC lookup |
| W4-37 | `wave4-37-hal-usb-async-vendor-write.md` | `hal_usb.c` async vendor writes |
| W4-38 | `wave4-38-hal-dm-odm-endian-hooks.md` | `hal_dm.c` Init_ODM endian hook block |
| W4-39 | `wave4-39-hal-com-tx-aclt-force-dump.md` | `hal_com.c` `dump_tx_aclt_force_val` |
| W4-40 | `wave4-40-hal-com-tx-aclt-confs-dump.md` | `hal_com.c` `dump_tx_aclt_confs` |

## Children (tranche 8 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-41 | `wave4-41-hal-dump-hal-spec.md` | `hal_com.c` `dump_hal_spec` |
| W4-42 | `wave4-42-hal-dump-tx-fifo.md` | `hal_com.c` `dump_TX_FIFO` |
| W4-43 | `wave4-43-hal-usb-free-recv-priv.md` | `hal_usb.c` recv priv teardown |
| W4-44 | `wave4-44-hal-usb-c2h-hisr.md` | `hal_usb.c` `usb_c2h_hisr_hdl` |

## Children (tranche 9 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-45 | `wave4-45-hal-usb-init-recv-priv-p1.md` | `usb_init_recv_priv` queues + interrupt pipe |
| W4-46 | `wave4-46-hal-usb-init-recv-priv-p2.md` | `usb_init_recv_priv` recv_buf + prealloc SKB |
| W4-47 | `wave4-47-hal-com-c2h-evt-read.md` | `c2h_evt_clear` / `c2h_evt_read_88xx` |
| W4-48 | `wave4-48-hal-com-get-port-defaults.md` | `rtw_hal_get_port` + HalData defaults |
| W4-49 | `wave4-49-hal-com-hw-port-ctrl.md` | RTW_HALMAC hw port enable/disable |

## Children (tranche 10 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-50 | `wave4-50-hal-com-c2h-iqk-offload.md` | C2H IQK + txpwr offload wait |
| W4-51 | `wave4-51-hal-com-reqtxrpt-mi-mac.md` | reqtxrpt + MI MAC addr |
| W4-52 | `wave4-52-hal-com-sec-cam-rw.md` | security CAM read/write |
| W4-53 | `wave4-53-hal-usb-set-intf-ops.md` | Extend Rust `usb_set_intf_ops` (post-W4-25; interrupt pipe) |

## Children (tranche 11 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-54 | `wave4-54-hal-com-sec-cam-clr.md` | security CAM clear + GK probe |
| W4-55 | `wave4-55-hal-com-sec-cam-ent.md` | security CAM entry read/write |
| W4-56 | `wave4-56-hal-com-c2h-debug-leaf.md` | C2H defeature + customer string rpt |

## Children (tranche 12 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-57 | `wave4-57-hal-com-c2h-mac-hidden.md` | C2H mac hidden report |
| W4-58 | `wave4-58-hal-com-c2h-per-rate-lps.md` | C2H per-rate + LPS status |

Further slices: deeper PHYDM / bulk USB RX path — file as `wave4-*.md` when tranche 13 is needed.
