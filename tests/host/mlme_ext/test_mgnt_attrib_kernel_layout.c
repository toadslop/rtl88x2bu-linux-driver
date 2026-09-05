// SPDX-License-Identifier: GPL-2.0
/*
 * L2 lock-in for W3-68 kernel update_mgntframe_attrib_addr / subtype:
 * USB 8822B TXDESC_OFFSET is TXDESC_SIZE(48) + PACKET_OFFSET_SZ(8) = 56.
 * Host L2 uses TXDESC_SIZE 40 so offset 48; hardcoding 48 on the kernel
 * path reads 8 bytes of packet-offset pad / TX desc tail as the 802.11
 * header, so attrib.ra/ta and subtype are wrong on every mgnt TX.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define KERNEL_USB_8822B_TXDESC_OFFSET 56
#define HOST_TXDESC_OFFSET 48
#define ETH_ALEN 6

struct pkt_attrib {
	u8 type;
	u8 subtype;
	u8 bswenc;
	u8 dhcp_pkt;
	u16 ether_type;
	u16 seqnum;
	u8 hw_ssn_sel;
	u16 pkt_hdrlen;
	u16 hdrlen;
	u32 pktlen;
	u32 last_txcmdsz;
	u8 nr_frags;
	u8 encrypt;
	u8 bmc_camid;
	u8 iv_len;
	u8 icv_len;
	u8 iv[18];
	u8 icv[16];
	u8 priority;
	u8 ack_policy;
	u8 mac_id;
	u8 vcs_mode;
	u8 dst[6];
	u8 src[6];
	u8 ta[6];
	u8 ra[6];
	u8 key_idx;
	u8 qos_en;
	u8 ht_en;
	u8 raid;
	u8 bwmode;
	u8 ch_offset;
	u8 sgi;
	u8 ampdu_en;
	u8 ampdu_spacing;
	u8 amsdu;
	u8 amsdu_ampdu_en;
	u8 mdata;
	u8 pctrl;
	u8 triggered;
	u8 qsel;
	u8 order;
	u8 eosp;
	u8 rate;
	u8 intel_proxim;
	u8 retry_ctrl;
	u8 mbssid;
	u8 ldpc;
	u8 stbc;
	u8 trigger_frame;
	void *psta;
	u8 rtsen;
	u8 cts2self;
	u8 dot11tkiptxmickey[32];
	u8 dot118021x_UncstKey[32];
	u8 key_type;
	u8 icmp_pkt;
	u8 hipriority_pkt;
	u16 txbf_p_aid;
	u16 txbf_g_id;
	u8 bf_pkt_type;
	u8 ps_dontq;
};

struct xmit_frame {
	u8 list[16];
	struct pkt_attrib attrib;
	u16 os_qid;
	void *pkt;
	int frame_tag;
	void *padapter;
	u8 *buf_addr;
};

_Static_assert(offsetof(struct xmit_frame, attrib) == 16, "list is 16 bytes");
_Static_assert(offsetof(struct pkt_attrib, ra) != 0, "ra present");

void update_mgntframe_attrib_addr(void *padapter, struct xmit_frame *pmgntframe);
void update_mgntframe_subtype(void *padapter, struct xmit_frame *pmgntframe);

u32 rtw_rust_mgnt_txdesc_offset(void)
{
	return KERNEL_USB_8822B_TXDESC_OFFSET;
}

u8 rtw_rust_mgnt_tx_rate(void *padapter)
{
	(void)padapter;
	return 0;
}

u16 rtw_rust_mgnt_mgnt_seq(void *padapter)
{
	(void)padapter;
	return 0;
}

u8 rtw_rust_mgnt_hw_ssn_seq_no(void *padapter)
{
	(void)padapter;
	return 0;
}

u8 rtw_rust_mgnt_hal_rf_type(void *padapter)
{
	(void)padapter;
	return 0;
}

u8 rtw_rust_mgnt_mlme_is_adhoc(void *padapter)
{
	(void)padapter;
	return 0;
}

void *rtw_rust_mgnt_stapriv(void *padapter)
{
	(void)padapter;
	return NULL;
}

u8 rtw_get_mgntframe_raid(void *adapter, unsigned char network_type)
{
	(void)adapter;
	(void)network_type;
	return 0;
}

void *rtw_get_stainfo(void *pstapriv, const u8 *hwaddr)
{
	(void)pstapriv;
	(void)hwaddr;
	return NULL;
}

int rtw_action_frame_parse(const u8 *frame, u32 frame_len, u8 *category,
			   u8 *action)
{
	(void)frame;
	(void)frame_len;
	if (category)
		*category = 0;
	if (action)
		*action = 0;
	return 0;
}

void rtw_bf_update_attrib(void *padapter, struct pkt_attrib *pattrib, void *psta)
{
	(void)padapter;
	(void)pattrib;
	(void)psta;
}

static int fail(const char *msg)
{
	fprintf(stderr, "FAIL: %s\n", msg);
	return 1;
}

int main(void)
{
	u8 buf[KERNEL_USB_8822B_TXDESC_OFFSET + 32];
	u8 dummy_adapter;
	struct xmit_frame xf;
	const u8 ra[ETH_ALEN] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0x01 };
	const u8 ta[ETH_ALEN] = { 0x23, 0x45, 0x67, 0x89, 0xab, 0x01 };
	/* Poison the host-oracle slot (offset 48) so a hardcoded 48 cannot
	 * accidentally match the real addresses at the USB 8822B offset. */
	const u8 poison_ra[ETH_ALEN] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
	const u8 poison_ta[ETH_ALEN] = { 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb };

	memset(buf, 0, sizeof(buf));
	memset(&xf, 0, sizeof(xf));

	/* Fake 802.11 header at the wrong (host) offset. */
	buf[HOST_TXDESC_OFFSET] = 0x08;
	memcpy(buf + HOST_TXDESC_OFFSET + 4, poison_ra, ETH_ALEN);
	memcpy(buf + HOST_TXDESC_OFFSET + 10, poison_ta, ETH_ALEN);

	/* Real deauth at the kernel USB offset. */
	buf[KERNEL_USB_8822B_TXDESC_OFFSET] = 0xc0;
	buf[KERNEL_USB_8822B_TXDESC_OFFSET + 1] = 0x00;
	memcpy(buf + KERNEL_USB_8822B_TXDESC_OFFSET + 4, ra, ETH_ALEN);
	memcpy(buf + KERNEL_USB_8822B_TXDESC_OFFSET + 10, ta, ETH_ALEN);

	xf.buf_addr = buf;
	xf.attrib.pktlen = 24;

	update_mgntframe_attrib_addr(&dummy_adapter, &xf);
	if (memcmp(xf.attrib.ra, ra, ETH_ALEN) != 0)
		return fail("ra read from host offset 48, not USB TXDESC_OFFSET 56");
	if (memcmp(xf.attrib.ta, ta, ETH_ALEN) != 0)
		return fail("ta read from host offset 48, not USB TXDESC_OFFSET 56");
	if (memcmp(xf.attrib.ra, poison_ra, ETH_ALEN) == 0)
		return fail("ra matched poison at offset 48");

	update_mgntframe_subtype(&dummy_adapter, &xf);
	if (xf.attrib.subtype != 0xc0)
		return fail("subtype parsed from host offset 48, not USB TXDESC_OFFSET 56");
	if (xf.attrib.ps_dontq != 0)
		return fail("deauth at offset 56 should set ps_dontq=0");

	printf("PASS: kernel USB TXDESC_OFFSET 56 (poison at 48 ignored)\n");
	printf("  ra=%02x%02x%02x%02x%02x%02x ta=%02x%02x%02x%02x%02x%02x subtype=0x%x\n",
	       xf.attrib.ra[0], xf.attrib.ra[1], xf.attrib.ra[2],
	       xf.attrib.ra[3], xf.attrib.ra[4], xf.attrib.ra[5],
	       xf.attrib.ta[0], xf.attrib.ta[1], xf.attrib.ta[2],
	       xf.attrib.ta[3], xf.attrib.ta[4], xf.attrib.ta[5],
	       xf.attrib.subtype);
	return 0;
}
