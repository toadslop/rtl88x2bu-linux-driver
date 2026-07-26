---
title: "[A2] Domain types for channel plan + rates (Wave 3)"
labels: [rust-migration, phase-1, size/~200]
type: child
id: A2
epic: E11
blocked_by: [A1]
estimate_loc: 200
---

## Goal

Extend [`rust/domain/types.rs`](../../../rust/domain/types.rs) with types needed by Wave 3 ports:

- `ChannelPlanId` — validated `u8` plan id (`try_from` rejects invalid/empty per `rtw_is_channel_plan_valid`)
- `CountryCode` — two-letter ISO code (`try_from` rejects non-alpha)
- `RegulatoryDomain` — typed txpwr limit enum where meaningful
- `WifiRate` / `NetworkType` — rate-byte and BSS network classification (for W3-08 / W3-09)

## Acceptance

- Types compile in module and host `#[cfg(test)]`
- Invalid values cannot construct without `Result` error
- Document `to_raw` / `from_raw` at ABI edge for `extern "C"` shims
