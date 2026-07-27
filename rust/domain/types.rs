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
    InvalidValue,
}

/// IEEE 802.11 security / cipher type (`enum security_type` in `rtw_security.h`).
///
/// Use `try_from` at Rust API boundaries; `to_raw` / `from_raw_unchecked` are for
/// `extern "C"` shims only.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum SecurityType {
    NoPrivacy = 0x00,
    Wep40 = 0x01,
    Tkip = 0x02,
    TkipWtMic = 0x03,
    Aes = 0x04,
    Wep104 = 0x05,
    Sms4 = 0x06,
    Gcmp = 0x07,
    Ccmp256 = 0x14,
    Gcmp256 = 0x17,
    BipCmac128 = 0x20,
    BipGmac128 = 0x21,
    BipGmac256 = 0x22,
    BipCmac256 = 0x23,
}

impl SecurityType {
    /// Maximum base cipher index (`_SEC_TYPE_MAX_` in C).
    pub const SEC_TYPE_MAX: u8 = 0x08;

    /// Raw wire value for ABI shims.
    pub const fn to_raw(self) -> u8 {
        self as u8
    }

    /// Construct without validation — ABI edge only.
    pub const fn from_raw_unchecked(raw: u8) -> Self {
        match raw {
            0x00 => Self::NoPrivacy,
            0x01 => Self::Wep40,
            0x02 => Self::Tkip,
            0x03 => Self::TkipWtMic,
            0x04 => Self::Aes,
            0x05 => Self::Wep104,
            0x06 => Self::Sms4,
            0x07 => Self::Gcmp,
            0x14 => Self::Ccmp256,
            0x17 => Self::Gcmp256,
            0x20 => Self::BipCmac128,
            0x21 => Self::BipGmac128,
            0x22 => Self::BipGmac256,
            0x23 => Self::BipCmac256,
            _ => Self::NoPrivacy,
        }
    }

    pub fn try_from(raw: u8) -> Result<Self, DomainError> {
        match raw {
            0x00 => Ok(Self::NoPrivacy),
            0x01 => Ok(Self::Wep40),
            0x02 => Ok(Self::Tkip),
            0x03 => Ok(Self::TkipWtMic),
            0x04 => Ok(Self::Aes),
            0x05 => Ok(Self::Wep104),
            0x06 => Ok(Self::Sms4),
            0x07 => Ok(Self::Gcmp),
            0x14 => Ok(Self::Ccmp256),
            0x17 => Ok(Self::Gcmp256),
            0x20 => Ok(Self::BipCmac128),
            0x21 => Ok(Self::BipGmac128),
            0x22 => Ok(Self::BipGmac256),
            0x23 => Ok(Self::BipCmac256),
            _ => Err(DomainError::InvalidValue),
        }
    }

    /// Whether `security_type_str` would return a label for this value.
    pub fn has_type_string(self) -> bool {
        Self::try_from(self.to_raw()).is_ok()
    }
}

/// IEEE 802.11 rate byte (CCK or OFDM, `rate & 0x7f` semantics).
///
/// Mirrors `WIFI_CCKRATES` / `WIFI_OFDMRATES` classification in `rtw_wlan_util.c`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(transparent)]
pub struct WifiRate(u8);

impl WifiRate {
    const CCK_RATES: [u8; 4] = [0x02, 0x04, 0x0b, 0x16];
    const OFDM_RATES: [u8; 8] = [0x0c, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6c];

    pub const fn to_raw(self) -> u8 {
        self.0
    }

    pub const fn from_raw_unchecked(raw: u8) -> Self {
        Self(raw)
    }

    pub fn rate_index(self) -> u8 {
        self.0 & 0x7f
    }

    pub fn is_cck(self) -> bool {
        let idx = self.rate_index();
        let mut i = 0;
        while i < Self::CCK_RATES.len() {
            if Self::CCK_RATES[i] == idx {
                return true;
            }
            i += 1;
        }
        false
    }

    pub fn is_ofdm(self) -> bool {
        let idx = self.rate_index();
        let mut i = 0;
        while i < Self::OFDM_RATES.len() {
            if Self::OFDM_RATES[i] == idx {
                return true;
            }
            i += 1;
        }
        false
    }

    pub fn try_from(raw: u8) -> Result<Self, DomainError> {
        let rate = Self(raw);
        if rate.is_cck() || rate.is_ofdm() {
            Ok(rate)
        } else {
            Err(DomainError::InvalidValue)
        }
    }
}

/// BSS / adapter wireless mode bitmask (`enum NETWORK_TYPE` in `ieee80211.h`).
///
/// Combinations are valid at the ABI edge; `try_from` rejects reserved bits.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(transparent)]
pub struct NetworkType(u8);

impl NetworkType {
    /// Bits used by `WIRELESS_MODE_MAX` and `WIRELESS_AUTO`.
    const VALID_MASK: u8 = 0x7f;

    pub const INVALID: Self = Self(0);
    pub const WIRELESS_11B: Self = Self(1 << 0);
    pub const WIRELESS_11G: Self = Self(1 << 1);
    pub const WIRELESS_11A: Self = Self(1 << 2);
    pub const WIRELESS_11_24N: Self = Self(1 << 3);
    pub const WIRELESS_11_5N: Self = Self(1 << 4);
    pub const WIRELESS_AUTO: Self = Self(1 << 5);
    pub const WIRELESS_11AC: Self = Self(1 << 6);

    pub const WIRELESS_11BG: Self = Self(Self::WIRELESS_11B.0 | Self::WIRELESS_11G.0);
    pub const WIRELESS_MODE_24G: Self =
        Self(Self::WIRELESS_11B.0 | Self::WIRELESS_11G.0 | Self::WIRELESS_11_24N.0);
    pub const WIRELESS_MODE_5G: Self =
        Self(Self::WIRELESS_11A.0 | Self::WIRELESS_11_5N.0 | Self::WIRELESS_11AC.0);
    pub const WIRELESS_MODE_MAX: Self = Self(
        Self::WIRELESS_11A.0
            | Self::WIRELESS_11B.0
            | Self::WIRELESS_11G.0
            | Self::WIRELESS_11_24N.0
            | Self::WIRELESS_11_5N.0
            | Self::WIRELESS_11AC.0,
    );

    pub const fn to_raw(self) -> u8 {
        self.0
    }

    pub const fn from_raw_unchecked(raw: u8) -> Self {
        Self(raw)
    }

    pub fn try_from(raw: u8) -> Result<Self, DomainError> {
        if raw & !Self::VALID_MASK != 0 {
            return Err(DomainError::InvalidValue);
        }
        Ok(Self(raw))
    }

    pub fn contains(self, flag: Self) -> bool {
        self.0 & flag.0 == flag.0
    }
}

/// Channel plan table index (`RTW_ChannelPlanMap` in `rtw_chplan.c`).
///
/// `try_from` mirrors `rtw_is_channel_plan_valid` bounds and rejects
/// `CHPLAN_ENT_NOT_DEFINED` slots (0x05–0x1f). Keep `MAP_SIZE` in sync with
/// `RTW_ChannelPlanMap_size` when the table grows. If the map later adds empty
/// slots outside that range, W2-17 chplan wiring should delegate to
/// `rtw_is_channel_plan_valid` at the FFI boundary or add an exhaustive
/// `0..MAP_SIZE` oracle test so drift is caught automatically.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(transparent)]
pub struct ChannelPlanId(u8);

impl ChannelPlanId {
    pub const MAP_SIZE: u8 = 0x80;

    pub const fn to_raw(self) -> u8 {
        self.0
    }

    pub const fn from_raw_unchecked(raw: u8) -> Self {
        Self(raw)
    }

    const fn is_undefined_slot(raw: u8) -> bool {
        raw >= 0x05 && raw <= 0x1f
    }

    pub fn try_from(raw: u8) -> Result<Self, DomainError> {
        if raw >= Self::MAP_SIZE || Self::is_undefined_slot(raw) {
            return Err(DomainError::InvalidValue);
        }
        Ok(Self(raw))
    }
}

/// ISO 3166-1 alpha-2 country code (two ASCII letters).
///
/// `try_from` rejects non-alpha pairs per A2. C's `rtw_set_country_cmd` still
/// accepts `"00"` as a worldwide sentinel before country-map lookup; handle that
/// at the W2-19 command shim, not in this type.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct CountryCode([u8; 2]);

impl CountryCode {
    pub const fn to_bytes(self) -> [u8; 2] {
        self.0
    }

    pub const fn from_bytes_unchecked(bytes: [u8; 2]) -> Self {
        Self(bytes)
    }

    const fn is_alpha(byte: u8) -> bool {
        (byte >= b'A' && byte <= b'Z') || (byte >= b'a' && byte <= b'z')
    }

    pub fn try_from(bytes: [u8; 2]) -> Result<Self, DomainError> {
        if Self::is_alpha(bytes[0]) && Self::is_alpha(bytes[1]) {
            Ok(Self(bytes))
        } else {
            Err(DomainError::InvalidValue)
        }
    }
}

/// Regulatory tx-power limit domain (`REGULATION_TXPWR_LMT` in `rtw_rf.h`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum RegulatoryDomain {
    None = 0,
    Fcc = 1,
    Mkk = 2,
    Etsi = 3,
    Ic = 4,
    Kcc = 5,
    Ncc = 6,
    Acma = 7,
    Chile = 8,
    Ukraine = 9,
    Mexico = 10,
    Cn = 11,
    Ww = 12,
}

impl RegulatoryDomain {
    pub const MAX: u8 = Self::Ww as u8;

    pub const fn to_raw(self) -> u8 {
        self as u8
    }

    /// ABI edge: unknown raw values clamp to `Ww`, mirroring C `regd_str()`.
    pub const fn from_raw_unchecked(raw: u8) -> Self {
        match raw {
            0 => Self::None,
            1 => Self::Fcc,
            2 => Self::Mkk,
            3 => Self::Etsi,
            4 => Self::Ic,
            5 => Self::Kcc,
            6 => Self::Ncc,
            7 => Self::Acma,
            8 => Self::Chile,
            9 => Self::Ukraine,
            10 => Self::Mexico,
            11 => Self::Cn,
            _ => Self::Ww,
        }
    }

    pub fn try_from(raw: u8) -> Result<Self, DomainError> {
        match raw {
            0 => Ok(Self::None),
            1 => Ok(Self::Fcc),
            2 => Ok(Self::Mkk),
            3 => Ok(Self::Etsi),
            4 => Ok(Self::Ic),
            5 => Ok(Self::Kcc),
            6 => Ok(Self::Ncc),
            7 => Ok(Self::Acma),
            8 => Ok(Self::Chile),
            9 => Ok(Self::Ukraine),
            10 => Ok(Self::Mexico),
            11 => Ok(Self::Cn),
            12 => Ok(Self::Ww),
            _ => Err(DomainError::InvalidValue),
        }
    }
}

/// BIP cipher selector returned by `security_type_bip_to_gmcs` (`WPA_CIPHER_BIP_*`).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(transparent)]
pub struct BipGmcs(u32);

impl BipGmcs {
    pub const CMAC_128: Self = Self(1 << 8);
    pub const GMAC_128: Self = Self(1 << 9);
    pub const GMAC_256: Self = Self(1 << 10);
    pub const CMAC_256: Self = Self(1 << 11);

    pub const fn to_raw(self) -> u32 {
        self.0
    }

    pub const fn from_raw_unchecked(raw: u32) -> Self {
        Self(raw)
    }

    pub fn try_from_security_type(sec: SecurityType) -> Result<Self, DomainError> {
        match sec {
            SecurityType::BipCmac128 => Ok(Self::CMAC_128),
            SecurityType::BipGmac128 => Ok(Self::GMAC_128),
            SecurityType::BipGmac256 => Ok(Self::GMAC_256),
            SecurityType::BipCmac256 => Ok(Self::CMAC_256),
            _ => Err(DomainError::InvalidValue),
        }
    }
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

/// AES-OMAC1 / CMAC output (128 bits).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(transparent)]
pub struct AesMac([u8; 16]);

impl AesMac {
    pub const SIZE: usize = 16;

    pub fn from_bytes(bytes: [u8; Self::SIZE]) -> Self {
        Self(bytes)
    }

    pub fn as_bytes(&self) -> &[u8; Self::SIZE] {
        &self.0
    }

    pub fn write_to_slice(self, out: &mut [u8]) -> Result<(), DomainError> {
        if out.len() != Self::SIZE {
            return Err(DomainError::InvalidLength);
        }
        out.copy_from_slice(&self.0);
        Ok(())
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

    #[test]
    fn aes_mac_write_to_slice_rejects_bad_length() {
        let mac = AesMac::from_bytes([0u8; 16]);
        let mut short = [0u8; 15];
        assert_eq!(
            mac.write_to_slice(&mut short),
            Err(DomainError::InvalidLength)
        );
    }

    #[test]
    fn aes_mac_write_to_slice_accepts_16_bytes() {
        let mac = AesMac::from_bytes([0xabu8; 16]);
        let mut out = [0u8; 16];
        mac.write_to_slice(&mut out).unwrap();
        assert_eq!(out, [0xabu8; 16]);
    }

    #[test]
    fn security_type_accepts_vector_values() {
        for raw in [
            0u8, 1, 2, 3, 4, 5, 6, 7, 20, 23, 32, 33, 34, 35,
        ] {
            assert!(
                SecurityType::try_from(raw).is_ok(),
                "expected valid security type for {}",
                raw
            );
        }
    }

    #[test]
    fn security_type_rejects_gaps() {
        for raw in [8u8, 9, 19, 24, 31, 36, 255] {
            assert_eq!(
                SecurityType::try_from(raw),
                Err(DomainError::InvalidValue),
                "expected reject for {}",
                raw
            );
        }
    }

    #[test]
    fn security_type_round_trips_raw() {
        for raw in [0u8, 4, 7, 20, 23, 32] {
            let t = SecurityType::try_from(raw).unwrap();
            assert_eq!(t.to_raw(), raw);
        }
    }

    #[test]
    fn bip_gmcs_maps_bip_security_types() {
        assert_eq!(
            BipGmcs::try_from_security_type(SecurityType::BipCmac128).unwrap(),
            BipGmcs::CMAC_128
        );
        assert_eq!(
            BipGmcs::try_from_security_type(SecurityType::BipGmac128).unwrap(),
            BipGmcs::GMAC_128
        );
        assert_eq!(
            BipGmcs::try_from_security_type(SecurityType::BipGmac256).unwrap(),
            BipGmcs::GMAC_256
        );
        assert_eq!(
            BipGmcs::try_from_security_type(SecurityType::BipCmac256).unwrap(),
            BipGmcs::CMAC_256
        );
        assert_eq!(
            BipGmcs::try_from_security_type(SecurityType::Aes),
            Err(DomainError::InvalidValue)
        );
    }

    #[test]
    fn wifi_rate_accepts_cck_and_ofdm_bytes() {
        for raw in [2u8, 4, 11, 22, 12, 24, 48, 108] {
            let rate = WifiRate::try_from(raw).unwrap();
            assert_eq!(rate.to_raw(), raw);
        }
        let basic = WifiRate::try_from(0x82).unwrap();
        assert_eq!(basic.rate_index(), 2);
        assert!(basic.is_cck());
    }

    #[test]
    fn wifi_rate_rejects_unknown_bytes() {
        assert_eq!(WifiRate::try_from(0), Err(DomainError::InvalidValue));
        assert_eq!(WifiRate::try_from(1), Err(DomainError::InvalidValue));
        assert_eq!(WifiRate::try_from(255), Err(DomainError::InvalidValue));
    }

    #[test]
    fn network_type_accepts_single_bits_and_combinations() {
        for raw in [
            0u8,
            NetworkType::WIRELESS_11B.to_raw(),
            NetworkType::WIRELESS_11G.to_raw(),
            NetworkType::WIRELESS_11BG.to_raw(),
            NetworkType::WIRELESS_MODE_24G.to_raw(),
            NetworkType::WIRELESS_MODE_5G.to_raw(),
            NetworkType::WIRELESS_MODE_MAX.to_raw(),
        ] {
            let nt = NetworkType::try_from(raw).unwrap();
            assert_eq!(nt.to_raw(), raw);
        }
    }

    #[test]
    fn network_type_rejects_reserved_bits() {
        assert_eq!(NetworkType::try_from(0x80), Err(DomainError::InvalidValue));
        assert_eq!(NetworkType::try_from(0xff), Err(DomainError::InvalidValue));
    }

    #[test]
    fn network_type_contains_flags() {
        let bg = NetworkType::WIRELESS_11BG;
        assert!(bg.contains(NetworkType::WIRELESS_11B));
        assert!(bg.contains(NetworkType::WIRELESS_11G));
        assert!(!bg.contains(NetworkType::WIRELESS_11A));
    }

    #[test]
    fn channel_plan_id_accepts_defined_slots() {
        for raw in [0u8, 1, 4, 32, 118, 127] {
            let id = ChannelPlanId::try_from(raw).unwrap();
            assert_eq!(id.to_raw(), raw);
        }
    }

    #[test]
    fn channel_plan_id_rejects_undefined_and_oob() {
        for raw in [5u8, 31, 128, 255] {
            assert_eq!(
                ChannelPlanId::try_from(raw),
                Err(DomainError::InvalidValue),
                "expected reject for {}",
                raw
            );
        }
    }

    #[test]
    fn country_code_accepts_alpha_pairs() {
        let us = CountryCode::try_from(*b"US").unwrap();
        assert_eq!(us.to_bytes(), *b"US");
        let de = CountryCode::try_from(*b"de").unwrap();
        assert_eq!(de.to_bytes(), *b"de");
    }

    #[test]
    fn country_code_rejects_non_alpha() {
        assert_eq!(
            CountryCode::try_from(*b"U1"),
            Err(DomainError::InvalidValue)
        );
        assert_eq!(
            CountryCode::try_from([0u8, b'A']),
            Err(DomainError::InvalidValue)
        );
    }

    #[test]
    fn regulatory_domain_round_trips() {
        for raw in 0..=RegulatoryDomain::MAX {
            let regd = RegulatoryDomain::try_from(raw).unwrap();
            assert_eq!(regd.to_raw(), raw);
        }
        assert_eq!(
            RegulatoryDomain::try_from(13),
            Err(DomainError::InvalidValue)
        );
    }
}
