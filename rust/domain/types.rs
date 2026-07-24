// SPDX-License-Identifier: GPL-2.0
//! Validated domain types for crypto and later driver ports (A1).
//!
//! Typed Rust APIs use these newtypes; thin `extern "C"` shims in `abi`/`ffi`
//! convert raw pointers at the C edge (see W1-03 `aes_ctr_encrypt`).
//!
//! Layout: a single `domain/types.rs` for the pilot. Split into
//! `domain/types/*.rs` (e.g. `crypto.rs`, `mac.rs`) once more types land;
//! `domain_types.rs` stays the Kbuild crate root that aggregates them.

/// Domain validation failure (illegal length or value at a boundary).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DomainError {
    InvalidLength,
}

/// AES-CTR counter block / nonce (16 bytes, `AES_BLOCK_SIZE`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(transparent)]
pub struct AesCtrNonce([u8; 16]);

impl AesCtrNonce {
    pub const SIZE: usize = 16;

    pub fn from_bytes(bytes: [u8; Self::SIZE]) -> Self {
        Self(bytes)
    }

    pub fn try_from_slice(slice: &[u8]) -> Result<Self, DomainError> {
        if slice.len() != Self::SIZE {
            return Err(DomainError::InvalidLength);
        }
        let mut bytes = [0u8; Self::SIZE];
        bytes.copy_from_slice(slice);
        Ok(Self(bytes))
    }

    pub fn as_bytes(&self) -> &[u8; Self::SIZE] {
        &self.0
    }
}

/// Length-checked AES key material for 128/192/256-bit variants.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AesKey {
    Aes128([u8; 16]),
    Aes192([u8; 24]),
    Aes256([u8; 32]),
}

impl AesKey {
    pub fn try_from_slice(key: &[u8]) -> Result<Self, DomainError> {
        match key.len() {
            16 => {
                let mut bytes = [0u8; 16];
                bytes.copy_from_slice(key);
                Ok(Self::Aes128(bytes))
            }
            24 => {
                let mut bytes = [0u8; 24];
                bytes.copy_from_slice(key);
                Ok(Self::Aes192(bytes))
            }
            32 => {
                let mut bytes = [0u8; 32];
                bytes.copy_from_slice(key);
                Ok(Self::Aes256(bytes))
            }
            _ => Err(DomainError::InvalidLength),
        }
    }

    pub fn key_len(&self) -> usize {
        match self {
            Self::Aes128(_) => 16,
            Self::Aes192(_) => 24,
            Self::Aes256(_) => 32,
        }
    }

    pub fn as_bytes(&self) -> &[u8] {
        match self {
            Self::Aes128(k) => k,
            Self::Aes192(k) => k,
            Self::Aes256(k) => k,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn aes_ctr_nonce_rejects_short_slice() {
        assert_eq!(
            AesCtrNonce::try_from_slice(&[0u8; 15]),
            Err(DomainError::InvalidLength)
        );
    }

    #[test]
    fn aes_ctr_nonce_rejects_long_slice() {
        assert_eq!(
            AesCtrNonce::try_from_slice(&[0u8; 17]),
            Err(DomainError::InvalidLength)
        );
    }

    #[test]
    fn aes_ctr_nonce_rejects_empty_slice() {
        assert_eq!(
            AesCtrNonce::try_from_slice(&[]),
            Err(DomainError::InvalidLength)
        );
    }

    #[test]
    fn aes_ctr_nonce_accepts_16_bytes() {
        let nonce = AesCtrNonce::try_from_slice(&[1u8; 16]).unwrap();
        assert_eq!(nonce.as_bytes(), &[1u8; 16]);
    }

    #[test]
    fn aes_key_accepts_valid_lengths_and_variants() {
        let k128 = AesKey::try_from_slice(&[0u8; 16]).unwrap();
        assert!(matches!(k128, AesKey::Aes128(_)));
        assert_eq!(k128.key_len(), 16);

        let k192 = AesKey::try_from_slice(&[0u8; 24]).unwrap();
        assert!(matches!(k192, AesKey::Aes192(_)));
        assert_eq!(k192.key_len(), 24);

        let k256 = AesKey::try_from_slice(&[0u8; 32]).unwrap();
        assert!(matches!(k256, AesKey::Aes256(_)));
        assert_eq!(k256.key_len(), 32);
    }

    #[test]
    fn aes_key_rejects_bad_length() {
        assert_eq!(
            AesKey::try_from_slice(&[]),
            Err(DomainError::InvalidLength)
        );
        assert_eq!(
            AesKey::try_from_slice(&[0u8; 15]),
            Err(DomainError::InvalidLength)
        );
        assert_eq!(
            AesKey::try_from_slice(&[0u8; 17]),
            Err(DomainError::InvalidLength)
        );
        assert_eq!(
            AesKey::try_from_slice(&[0u8; 33]),
            Err(DomainError::InvalidLength)
        );
    }
}
