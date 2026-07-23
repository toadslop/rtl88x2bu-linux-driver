---
title: "[W2-10] Translate ccmp.c (part 2/2)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-10
epic: E04
blocked_by: [W2-09]
estimate_loc: 200
---

## Goal

Finish [`core/crypto/ccmp.c`](../../../core/crypto/ccmp.c); remove C object from build.

## Acceptance

- CCMP path works under WPA2 STA smoke
