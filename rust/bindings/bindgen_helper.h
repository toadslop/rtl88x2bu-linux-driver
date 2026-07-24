/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Input header for scripts/bindgen_rtw.sh (W1-01).
 *
 * Allowlisted FFI surface for the Wave 1 aes-ctr pilot: only the AES encrypt
 * primitives that aes-ctr.c calls today. Keeps bindgen off drv_types.h and the
 * rest of the driver include tree (see wave1-01-bindgen.md out-of-scope).
 *
 * `#define u8 uint8_t` before including aes.h so signatures use fixed-width
 * types bindgen maps cleanly under --use-core (avoids a recursive `type u8 = u8`).
 */

#ifndef RTW_BINDGEN_HELPER_H
#define RTW_BINDGEN_HELPER_H

#include <stddef.h>
#include <stdint.h>

#define u8 uint8_t

#include "../../core/crypto/aes.h"

#endif /* RTW_BINDGEN_HELPER_H */
