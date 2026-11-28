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

## Children (tranche 13 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-59 | `wave4-59-hal-intf-chip-configure.md` | `rtw_hal_chip_configure` + `rtw_hal_read_chip_info` |
| W4-60 | `wave4-60-hal-intf-chip-version-defaults.md` | `rtw_hal_read_chip_version` + `rtw_hal_def_value_init` |
| W4-61 | `wave4-61-hal-intf-data-init.md` | `rtw_hal_data_init` part 1 |
| W4-62 | `wave4-62-hal-intf-data-teardown.md` | `rtw_hal_data_deinit` + `rtw_hal_free_data` |

## Children (tranche 14 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-63 | `wave4-63-hal-intf-dm-init.md` | `rtw_hal_dm_init` + `rtw_hal_dm_deinit` |
| W4-64 | `wave4-64-hal-intf-chip-rftype.md` | `rtw_chip_rftype_to_hal_rftype` |
| W4-65 | `wave4-65-hal-intf-rfpath-init.md` | `rtw_hal_rfpath_init` |
| W4-66 | `wave4-66-hal-intf-trxnss-init.md` | `rtw_hal_trxnss_init` |
| W4-67 | `wave4-67-hal-intf-power-on-off.md` | `rtw_hal_power_on` + `rtw_hal_power_off` |

## Children (tranche 15 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-68 | `wave4-68-hal-intf-sw-led.md` | SW LED init/deinit |
| W4-69 | `wave4-69-hal-intf-init-opmode.md` | `rtw_hal_init_opmode` |
| W4-70 | `wave4-70-hal-intf-interrupt-ips.md` | Interrupt enable/disable + IPS check |
| W4-71 | `wave4-71-hal-intf-fw-dl.md` | `rtw_hal_fw_dl` |
| W4-72 | `wave4-72-hal-intf-inirp.md` | `rtw_hal_inirp_init` / `deinit` |

## Children (tranche 16 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-73 | `wave4-73-hal-intf-hwreg-odm.md` | hwreg / def / ODM + notch dispatch |
| W4-74 | `wave4-74-hal-intf-sreset-dispatch.md` | Silent-reset `hal_func` wrappers |
| W4-75 | `wave4-75-hal-intf-hal-deinit.md` | `rtw_hal_deinit` |
| W4-76 | `wave4-76-hal-intf-fw-mem-clear-int.md` | `fw_mem_dl` + `clear_interrupt` |
| W4-77 | `wave4-77-hal-com-c2h-pkt-dispatch.md` | C2H packet pre/handle (`hal_com_c2h_pkt.rs`) |
| W4-78 | `wave4-78-hal-intf-thread-iol-dispatch.md` | IOL + xmit/recv thread dispatch |
| W4-79 | `wave4-79-hal-dm-fw-offload-caps.md` | IQK/param FW offload caps |
| W4-80 | `wave4-80-hal-intf-iface-init.md` | `rtw_hal_iface_init` (new netdev path) |

## Children (tranche 17 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-81 | `wave4-81-hal-intf-hal-init.md` | `rtw_hal_init` orchestration |
| W4-82 | `wave4-82-hal-intf-bb-rf-reg.md` | BB/RF (and optional syson) register dispatch |
| W4-83 | `wave4-83-hal-intf-xmit-recv-thread.md` | xmit/recv priv + SDIO thread wrappers |
| W4-84 | `wave4-84-hal-dm-phydm-priv-odm-iface.md` | `phydm_priv_init` + early `Init_ODM` |
| W4-85 | `wave4-85-hal-dm-init-odm-adaptivity-hooks.md` | `Init_ODM` adaptivity + traffic hooks |
| W4-86 | `wave4-86-hal-com-customer-str-sync.md` | customer string read/write API |
| W4-87 | `wave4-87-hal-intf-xmit-dispatch.md` | xmit enqueue + mgnt xmit dispatch |

## Children (tranche 18 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-88 | `wave4-88-hal-dm-init-odm-hook-tail.md` | Init_ODM ComInfo pointer-hook tail |
| W4-89 | `wave4-89-hal-dm-phydm-ops-init.md` | `rtw_phydm_ops_func_init` + `record_ra_info` |
| W4-90 | `wave4-90-hal-com-config-channel-plan-p1.md` | `hal_com_config_channel_plan` HW branch |
| W4-91 | `wave4-91-hal-com-config-channel-plan-p2.md` | `hal_com_config_channel_plan` SW/done tail |
| W4-92 | `wave4-92-hal-dm-phydm-igi-iqk-queries.md` | `rtw_phydm_get_cur_igi` + IQK-in-progress |

## Children (tranche 19 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-93 | `wave4-93-hal-dm-min-rssi-phy-cnt-queries.md` | min RSSI + PHY FA/CCA counters |
| W4-94 | `wave4-94-hal-dm-set-odm-var-core.md` | `SetHalODMVar` core switch cases |
| W4-95 | `wave4-95-hal-dm-get-odm-var.md` | `GetHalODMVar` |
| W4-96 | `wave4-96-hal-dm-set-odm-var-rx-dump.md` | `SetHalODMVar` RX info dump |
| W4-97 | `wave4-97-hal-com-update-sta-wset-mimo.md` | STA wireless set + MIMO type |
| W4-98 | `wave4-98-hal-dm-iqk-lck-trigger.md` | IQK/LCK `halrf` trigger wrappers |

## Children (tranche 20 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-99 | `wave4-99-hal-com-sta-smps-rate-mask.md` | STA SMPS cap + rate mask |
| W4-100 | `wave4-100-hal-dm-dynamic-txpwr.md` | Dynamic TX power desc helpers |
| W4-101 | `wave4-101-hal-dm-phydm-cal-trigger.md` | `rtw_hal_phydm_cal_trigger` |
| W4-102 | `wave4-102-hal-dm-rrsr-ctrl.md` | RRSR set + dynamic enable |
| W4-103 | `wave4-103-hal-dm-runtime-trx-path.md` | Runtime TRX path decision (8822B) |
| W4-104 | `wave4-104-hal-dm-crystal-cap.md` | Crystal cap register setter |

## Children (tranche 21 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-105 | `wave4-105-hal-dm-iqk-trigger-dbg.md` | IQK trigger debug wrapper |
| W4-106 | `wave4-106-hal-dm-dbg-rf-cal-test.md` | DBG RF cal test wrappers |
| W4-107 | `wave4-107-hal-dm-ra-registed.md` | PHYDM RA registered hook |
| W4-108 | `wave4-108-hal-dm-trx-cfg-txss.md` | TRX cfg for TX SS by throughput |
| W4-109 | `wave4-109-hal-dm-halmac-dm-leaf.md` | HALMAC DM leaf delegates |
| W4-110 | `wave4-110-hal-dm-cfg-phy-para.md` | PHYDM HALMAC cfg_phy_para mapper |
| W4-111 | `wave4-111-hal-dm-dyn-soml.md` | Dynamic SOML helpers |
| W4-112 | `wave4-112-hal-dm-read-efuse-trim.md` | PHYDM efuse trim read hooks |
| W4-113 | `wave4-113-hal-dm-phydm-init.md` | PHYDM init orchestration |

## Children (tranche 22 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-117 | `wave4-117-hal-com-update-sta-ra-info.md` | `rtw_hal_update_sta_ra_info` orchestrator |
| W4-118 | `wave4-118-hal-com-req-per-rpt-cmd.md` | `rtw_hal_set_req_per_rpt_cmd` H2C packer |

## Children (tranche 23 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-119 | `wave4-119-hal-com-mbid-cam-search.md` | MBSSID CAM init/search |
| W4-120 | `wave4-120-hal-com-mbid-cam-alloc.md` | MBSSID CAM alloc/clean |
| W4-121 | `wave4-121-hal-com-get-rsvd-page.md` | HALMAC rsvd page fetch |
| W4-122 | `wave4-122-hal-com-dump-rsvd-fifo.md` | RSVD/FIFO debug dumps |
| W4-123 | `wave4-123-hal-com-gpio-wl-ctrl.md` | GPIO WL control switch |
| W4-124 | `wave4-124-hal-com-tx-aclt-setters.md` | TX AC lifetime setters |

## Children (tranche 24 — filed)

| ID | File | Focus |
|----|------|--------|
| W4-125 | `wave4-125-hal-com-mbid-cam-cache-dump.md` | MBSSID CAM cache debug dump |
| W4-126 | `wave4-126-hal-com-mbid-cam-hw-push.md` | MBSSID CAM HW push + enable |
| W4-127 | `wave4-127-hal-com-rcr-leaf.md` | RCR add/clear/check leaf |
| W4-128 | `wave4-128-hal-com-rcr-chk-bssid.md` | RCR check-BSSID policy |
| W4-129 | `wave4-129-hal-com-tsf-iface-ctrl.md` | TSF iface enable/disable |
| W4-130 | `wave4-130-hal-com-tsf-periodic-chk.md` | TSF periodic update check |
| W4-131 | `wave4-131-hal-com-dft-port-h2c.md` | Default port H2C cmd |
| ~~W4-132~~ | `wave4-132-hal-com-bcn-interval-adjust.md` | ~~SW TXBCN interval adjust~~ (cancelled — not in 8822B profile) |

## Children (tranche 25 — drafted)

| ID | File | Focus |
|----|------|--------|
| W4-133 | `wave4-133-hal-com-mbid-macaddr.md` | MBSSID MAC address set/change |
| W4-134 | `wave4-134-hal-com-msr-nettype.md` | MSR network type get/set (HALMAC) |
| W4-135 | `wave4-135-hal-com-halmac-bssid-mac.md` | HALMAC BSSID + port MAC set/get |
| W4-136 | `wave4-136-hal-com-fw-rsvdpage-h2c.md` | RSVD page location H2C packer |
| W4-137 | `wave4-137-hal-com-rcr-am-hwvar.md` | `hw_var_set_rcr_am` delegate |
| W4-138 | `wave4-138-hal-com-fw-bcn-offload.md` | FW beacon offload + BCN rsvd page H2C |
| W4-139 | `wave4-139-hal-com-fw-media-status.md` | Media status report H2C |

## Children (tranche 26 — drafted)

| ID | File | Focus |
|----|------|--------|
| W4-140 | `wave4-140-hal-com-gpio-pin-mmio.md` | GPIO output/input MMIO |
| W4-141 | `wave4-141-hal-com-mbid-cam-read.md` | MBSSID CAM read + dump |
| W4-142 | `wave4-142-hal-com-gpio-api.md` | `CONFIG_GPIO_API` get/config/set |
| W4-143 | `wave4-143-hal-com-tsf-reset-offload.md` | TSF reset H2C offload |
| W4-144 | `wave4-144-hal-com-btc-port-h2c.md` | BT coexist WLAN port H2C |
| W4-145 | `wave4-145-hal-com-fw-aoac-rsvd.md` | WOW AOAC rsvd-page H2C |

## Children (tranche 27 — drafted)

| ID | File | Focus |
|----|------|--------|
| W4-146 | `wave4-146-hal-com-ps-rsvd-page.md` | PS join rsvd-page report |
| W4-147 | `wave4-147-hal-com-p2p-ps-offload.md` | P2P PS HALMAC offload |
| W4-148 | `wave4-148-hal-com-fw-dbg-msg-h2c.md` | FW debug msg rsvd-page H2C |
| W4-149 | `wave4-149-hal-com-wow-rxdma.md` | WOW RX DMA pause/release |
| W4-150 | `wave4-150-hal-com-wow-ctrl-chk.md` | WOW ctrl check |
| W4-151 | `wave4-151-hal-com-bf-rf-num.md` | BF txbfer/txbfee RF counts |
| W4-152 | `wave4-152-hal-com-bf-csi-rate.md` | BF CSI rate cfg |
| W4-153 | `wave4-153-hal-com-p2p-rsvd-h2c.md` | P2P rsvd-page H2C |
| W4-154 | `wave4-154-hal-com-p2p-wow-offload-h2c.md` | P2P WoWLAN offload H2C |
| W4-155 | `wave4-155-hal-com-lps-frame-construct.md` | LPS PSPoll/beacon/null frames |
| W4-156 | `wave4-156-hal-com-lps-pg-info-rsvd.md` | LPS PG info rsvd page |

Further slices: USB bulk port bodies stay under Wave 5 `os_dep`; continue `hal_com_rest.c` tails (P2P construct bodies, WOW cmd orch., LPS PG orchestration) in new `wave4-*.md` batches.
