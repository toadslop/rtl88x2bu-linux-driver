// SPDX-License-Identifier: GPL-2.0
/*
 * L2 lock-in for W3-66 kernel process_80211d: RT_CHANNEL_INFO is 32 bytes
 * (ChannelNum@0, flags@1, hidden_bss_cnt@16, os_chan@24). A packed 2-byte
 * overlay memcpy would write merged channel numbers into padding / os_chan
 * and leave later entries' ChannelNum as 0 (scan stops after one channel).
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t sint;

#define MAX_CHANNEL_NUM 59
#define _FIXED_IE_LENGTH_ 12
#define _COUNTRY_IE_ 7
#define WIRELESS_11G 0x02

struct rt_channel_info {
	u8 ChannelNum;
	u8 flags;
	u8 pad0[6];
	unsigned long non_ocp_end_time;
	u8 hidden_bss_cnt;
	u8 pad1[7];
	void *os_chan;
};

_Static_assert(sizeof(struct rt_channel_info) == 32,
	       "kernel RT_CHANNEL_INFO stride");
_Static_assert(__builtin_offsetof(struct rt_channel_info, non_ocp_end_time) == 8,
	       "non_ocp offset");
_Static_assert(__builtin_offsetof(struct rt_channel_info, hidden_bss_cnt) == 16,
	       "hidden_bss_cnt offset");
_Static_assert(__builtin_offsetof(struct rt_channel_info, os_chan) == 24,
	       "os_chan offset");

struct adapter {
	u8 enable80211d;
	u8 wireless_mode;
	u8 update_done;
	struct rt_channel_info channel_set[MAX_CHANNEL_NUM];
};

struct bss {
	u32 ie_length;
	u8 ies[768];
};

void process_80211d(void *padapter, void *bssid);

void *_rtw_memset(void *s, int c, size_t n)
{
	return memset(s, c, n);
}

void *_rtw_memcpy(void *d, const void *s, size_t n)
{
	return memcpy(d, s, n);
}

u8 *rtw_rust_80211d_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint i = 0;
	const u8 *p = pbuf;

	if (limit < 1)
		return NULL;
	*len = 0;
	while (1) {
		sint tmp;

		if (*p == (u8)index) {
			*len = *(p + 1);
			return (u8 *)p;
		}
		tmp = *(p + 1);
		p += tmp + 2;
		i += tmp + 2;
		if (i >= limit)
			break;
	}
	return NULL;
}

u8 rtw_rust_80211d_enable(struct adapter *a)
{
	return a->enable80211d;
}

u8 rtw_rust_80211d_wireless_mode(struct adapter *a)
{
	return a->wireless_mode;
}

u8 rtw_rust_80211d_update_done(struct adapter *a)
{
	return a->update_done;
}

void rtw_rust_80211d_set_update_done(struct adapter *a, u8 v)
{
	a->update_done = v;
}

u8 rtw_rust_80211d_ch_num(struct adapter *a, u8 i)
{
	if (i >= MAX_CHANNEL_NUM)
		return 0;
	return a->channel_set[i].ChannelNum;
}

u8 rtw_rust_80211d_ch_flags(struct adapter *a, u8 i)
{
	if (i >= MAX_CHANNEL_NUM)
		return 0;
	return a->channel_set[i].flags;
}

void rtw_rust_80211d_zero_chset(struct adapter *a)
{
	memset(a->channel_set, 0, sizeof(a->channel_set));
}

void rtw_rust_80211d_set_ch(struct adapter *a, u8 i, u8 num, u8 flags)
{
	if (i >= MAX_CHANNEL_NUM)
		return;
	a->channel_set[i].ChannelNum = num;
	a->channel_set[i].flags = flags;
}

void rtw_rust_80211d_reg_change(struct adapter *a)
{
	(void)a;
}

u8 *rtw_rust_80211d_bss_ies(struct bss *b)
{
	return b->ies;
}

u32 rtw_rust_80211d_bss_ie_len(struct bss *b)
{
	return b->ie_length;
}

int main(void)
{
	static const u8 us2g[] = {0x07, 0x09, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e};
	struct adapter a;
	struct bss b;
	int i;
	int bad = 0;

	memset(&a, 0, sizeof(a));
	memset(&b, 0, sizeof(b));
	a.enable80211d = 1;
	a.wireless_mode = WIRELESS_11G;
	a.channel_set[0].ChannelNum = 1;
	a.channel_set[1].ChannelNum = 6;
	a.channel_set[2].ChannelNum = 11;
	for (i = 0; i < MAX_CHANNEL_NUM; i++) {
		memset(a.channel_set[i].pad0, 0xAB, sizeof(a.channel_set[i].pad0));
		a.channel_set[i].os_chan = (void *)(uintptr_t)(0x1000u + (unsigned)i);
	}

	memcpy(b.ies + _FIXED_IE_LENGTH_, us2g, sizeof(us2g));
	b.ie_length = _FIXED_IE_LENGTH_ + (u32)sizeof(us2g);

	process_80211d(&a, &b);

	if (!a.update_done) {
		fprintf(stderr, "FAIL update_done not set\n");
		bad = 1;
	}
	for (i = 0; i < 11; i++) {
		if (a.channel_set[i].ChannelNum != (u8)(i + 1)) {
			fprintf(stderr, "FAIL ch[%d]=%u want %d\n", i,
				a.channel_set[i].ChannelNum, i + 1);
			bad = 1;
		}
		if (a.channel_set[i].pad0[0] != 0) {
			fprintf(stderr, "FAIL ch[%d] padding overwritten with 0x%02x\n",
				i, a.channel_set[i].pad0[0]);
			bad = 1;
		}
		if (a.channel_set[i].os_chan != NULL) {
			fprintf(stderr, "FAIL ch[%d] os_chan=%p not cleared\n",
				i, a.channel_set[i].os_chan);
			bad = 1;
		}
	}
	if (a.channel_set[11].ChannelNum != 0) {
		fprintf(stderr, "FAIL extra channel %u\n",
			a.channel_set[11].ChannelNum);
		bad = 1;
	}
	if (!bad)
		printf("PASS kernel-layout 802.11d (32-byte RT_CHANNEL_INFO stride)\n");
	return bad;
}
