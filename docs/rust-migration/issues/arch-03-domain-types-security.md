---
title: "[A3] Domain types for security (Wave 3)"
labels: [rust-migration, phase-1, size/~200]
type: child
id: A3
epic: E11
blocked_by: [A1]
estimate_loc: 150
---

## Goal

Extend [`rust/domain/types.rs`](../../../rust/domain/types.rs) with types needed by Wave 3 security ports:

- `SecurityType` — validated enum mirroring `enum security_type` in [`include/rtw_security.h`](../../../include/rtw_security.h) (`_NO_PRIVACY_`, `_WEP40_`, …) and `security_type_str` inputs (`try_from` rejects unknown values)
- `BipGmcs` or equivalent typed cipher selector for `security_type_bip_to_gmcs` (`CONFIG_IEEE80211W`)

## Acceptance

- Types compile in module and host `#[cfg(test)]`
- Invalid values cannot construct without `Result` error
- Document `to_raw` / `from_raw` at ABI edge for `extern "C"` shims
- W3-04 depends on this issue (not optional inline)
