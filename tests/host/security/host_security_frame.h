/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal frame/adapter layouts for host L2 WEP frame tests (W3-06 / T5).
 * Shared by tests/host/shim/host_security_shim.c and rust/rtw_security.rs
 * (--cfg host_security_test).
 */
#ifndef HOST_SECURITY_FRAME_H
#define HOST_SECURITY_FRAME_H

#include "host_security_types.h"

#define HOST_ETH_ALEN 6
#define HOST_TXDESC_OFFSET 56
#define HOST_MAX_WEP_FRAME 512
#define HOST_MAX_TKIP_FRAME 512
#define HOST_MAX_GCMP_FRAME 512

#define HOST_IS_MCAST(da) (((da)[0] & 0x01) != 0)

typedef struct {
	u8 skey[32];
} host_keytype;

struct host_security_priv {
	u32 dot11_privacy_key_index;
	host_keytype dot11_def_key[6];
	u32 dot11_def_keylen[6];
	u32 dot118021XGrpKeyid;
	host_keytype dot118021XGrpKey[6];
	u8 binstallGrpkey;
};

struct host_sta_info {
	u8 used;
	u8 ta[HOST_ETH_ALEN];
	host_keytype dot118021x_UncstKey;
};

struct host_stapriv {
	struct host_sta_info stas[4];
};

struct host_xmit_priv {
	u32 frag_len;
};

/*
 * Field order after ra must match rust/rtw_security.rs host PktAttrib (W3-10).
 */
struct host_pkt_attrib {
	u8 encrypt;
	u8 nr_frags;
	u8 _pad0;
	u16 hdrlen;
	u32 last_txcmdsz;
	u8 iv_len;
	u8 icv_len;
	u8 _pad1[2];
	u8 ra[HOST_ETH_ALEN];
	u8 ta[HOST_ETH_ALEN];
	host_keytype dot118021x_UncstKey;
};

struct host_xmit_frame {
	struct host_pkt_attrib attrib;
	u8 *buf_addr;
	s8 pkt_offset;
};

struct host_rx_pkt_attrib {
	u16 pkt_len;
	u8 _pad0[3];
	u8 hdrlen;
	u8 _pad1[12];
	u8 encrypt;
	u8 iv_len;
	u8 _pad2[32];
	u8 key_index;
	u8 _pad3;
	u8 ra[HOST_ETH_ALEN];
	u8 ta[HOST_ETH_ALEN];
};

struct host_recv_frame_hdr {
	struct host_rx_pkt_attrib attrib;
	u32 len;
	u8 *rx_data;
};

union host_recv_frame {
	struct {
		struct host_recv_frame_hdr hdr;
	} u;
};

struct host_adapter {
	struct host_security_priv securitypriv;
	struct host_xmit_priv xmitpriv;
	struct host_stapriv stapriv;
};

#endif /* HOST_SECURITY_FRAME_H */
