// SPDX-License-Identifier: GPL-2.0
/* FX-12 L2: CAM fixture + C oracle + memcpy guard. */

#include <stdint.h>
#include <string.h>

struct _adapter;

#define CAM_SLOTS 8

static unsigned int g_cam[CAM_SLOTS];
static int g_cam_len;
int host_sec_cam_bad_write;

unsigned int host_sec_cam_read(unsigned char addr)
{
	unsigned int idx = addr & 7U;

	if (idx >= (unsigned int)g_cam_len)
		return 0;
	return g_cam[idx];
}

void host_sec_cam_reset(void)
{
	memset(g_cam, 0, sizeof(g_cam));
	g_cam_len = 0;
	host_sec_cam_bad_write = 0;
}

void host_sec_cam_set_slot(unsigned char addr, unsigned int val)
{
	unsigned int idx = addr & 7U;

	if (idx >= CAM_SLOTS)
		return;
	g_cam[idx] = val;
	if ((int)(idx + 1) > g_cam_len)
		g_cam_len = (int)(idx + 1);
}

void _rtw_memcpy(void *dst, const void *src, unsigned int sz)
{
	if (dst && (uintptr_t)dst < 0x1000UL)
		host_sec_cam_bad_write = 1;
	memcpy(dst, src, sz);
}

void rtw_rust_hal_com_warn_on(int condition)
{
	(void)condition;
}

void rtw_sec_read_cam_ent_c_oracle(struct _adapter *adapter, unsigned char id,
				   unsigned char *ctrl, unsigned char *mac,
				   unsigned char *key)
{
	unsigned char i, begin = 0, end = 5;
	unsigned int rdata;

	if (!ctrl && !mac && !key)
		return;
	if (!ctrl && !mac)
		begin = 2;
	if (!key && !mac)
		end = 0;
	else if (!key)
		end = 2;

	for (i = begin; i <= end; i++) {
		rdata = host_sec_cam_read((id << 3) | i);
		switch (i) {
		case 0:
			if (ctrl)
				_rtw_memcpy(ctrl, (unsigned char *)(&rdata), 2);
			if (mac)
				_rtw_memcpy(mac, ((unsigned char *)(&rdata)) + 2, 2);
			break;
		case 1:
			if (mac)
				_rtw_memcpy(mac + 2, (unsigned char *)(&rdata), 4);
			break;
		default:
			if (key)
				_rtw_memcpy(key + (i - 2) * 4,
					    (unsigned char *)(&rdata), 4);
			break;
		}
	}
	(void)adapter;
}
