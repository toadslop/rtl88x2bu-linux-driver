---
title: "[FX-06] hal_com: stop force-enabling CONFIG_RTW_CUSTOMER_STR; gate Rust on C config"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-06
epic: E14
blocked_by: [FX-07]
estimate_loc: 80
---

## Priority

**P2 (medium)** — default-build behaviour change on 8822B.

## Problem

62570053 (W4-56 PR2, #1445) added to [`include/hal_ic_cfg.h`](../../../include/hal_ic_cfg.h)
(RTL8822B block, ~line 291):

```c
#ifndef CONFIG_RTW_CUSTOMER_STR
#define CONFIG_RTW_CUSTOMER_STR
#endif
```

so that the unconditionally compiled
[`rust/hal_com_c2h_customer_str.rs`](../../../rust/hal_com_c2h_customer_str.rs) links
against shims that live under `#ifdef CONFIG_RTW_CUSTOMER_STR` in
`hal/hal_com_rest.c`. The C config was changed to fit the Rust code: `dvobj_priv`
layout changes (customer-string mutex/fields), new proc/ioctl entries, the
customer-string H2C, and C2H 0x24/0x25 dispatch are now live on 8822B.

Same pattern, latent: [`rust/hal_com_c2h_mac_hidden.rs`](../../../rust/hal_com_c2h_mac_hidden.rs)
is unconditional while its shims are under `CONFIG_RTW_MAC_HIDDEN_RPT`
(8822B defines it; other chips would fail to link).

## Fix

1. Revert the `hal_ic_cfg.h` hunk.
2. Gate the Rust exports with `#[cfg(config_rtw_customer_str)]` /
   `#[cfg(config_rtw_mac_hidden_rpt)]` (cfgs provided by FX-07).

## Acceptance

- L0 default build; `nm 88x2bu.ko` shows no `c2h_customer_str_rpt_hdl` /
  `rtw_rust_hal_com_customer_str_*` unless `CONFIG_RTW_CUSTOMER_STR` is set.
- L0 with `USER_EXTRA_CFLAGS=-DCONFIG_RTW_CUSTOMER_STR` links and exports them.
- L1 for `rust/hal_com.o` with the allowlist updated for the cfg-gated symbols.

## Notes

- **Dependency rationale:** needs the `config_rtw_customer_str` / `config_rtw_mac_hidden_rpt` cfgs that FX-07 adds to the Makefile.
