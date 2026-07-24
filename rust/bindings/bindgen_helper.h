/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Input header for scripts/bindgen_rtw.sh (W1-01).
 *
 * Allowlisted FFI surface for the Wave 1 aes-ctr pilot. Includes
 * core/crypto/aes.h directly (no hand-copied prototypes) while keeping
 * bindgen off drv_types.h (explicitly out of scope for W1-01).
 *
 * Use `#define u8 uint8_t` (not `typedef uint8_t u8`) before the include so
 * preprocessing rewrites aes.h signatures to uint8_t. A C typedef named `u8`
 * makes bindgen emit `pub type u8_ = u8` and pollute the FFI surface.
 */

#ifndef RTW_BINDGEN_HELPER_H
#define RTW_BINDGEN_HELPER_H

#include <stddef.h>
#include <stdint.h>

#define u8 uint8_t

#include "../../core/crypto/aes.h"

#endif /* RTW_BINDGEN_HELPER_H */
