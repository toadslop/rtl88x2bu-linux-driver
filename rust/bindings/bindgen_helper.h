/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Input header for scripts/bindgen_rtw.sh (W1-01).
 *
 * Redeclares the AES leaf APIs that the aes-ctr pilot will call from Rust,
 * using stdint/stddef types so bindgen does not pull drv_types.h (explicitly
 * out of scope for W1-01) or emit a C `u8` typedef that clashes with Rust's
 * primitive `u8`.
 *
 * Keep these prototypes in sync with core/crypto/aes.h.
 */

#ifndef RTW_BINDGEN_HELPER_H
#define RTW_BINDGEN_HELPER_H

#include <stddef.h>
#include <stdint.h>

#define AES_BLOCK_SIZE 16

void *aes_encrypt_init(const uint8_t *key, size_t len);
int aes_encrypt(void *ctx, const uint8_t *plain, uint8_t *crypt);
void aes_encrypt_deinit(void *ctx);

void *aes_decrypt_init(const uint8_t *key, size_t len);
int aes_decrypt(void *ctx, const uint8_t *crypt, uint8_t *plain);
void aes_decrypt_deinit(void *ctx);

#endif /* RTW_BINDGEN_HELPER_H */
