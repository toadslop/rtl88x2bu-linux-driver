// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-141 MBSSID CAM HW read + dump (hal_com_rest.c). */

#include "host_hal_com_mbid_cam_types.h"
#include <string.h>

extern u32 rtw_rust_hal_com_read32(void *adapter, u32 addr);
extern u16 rtw_rust_hal_com_read16(void *adapter, u32 addr);
extern void rtw_rust_hal_com_write32(void *adapter, u32 addr, u32 val);
extern u8 rtw_rust_hal_com_cannot_run(void *adapter);
extern void rtw_rust_hal_com_print_line(void *sel, const char *line);
extern void *rtw_rust_hal_com_adapter_to_dvobj(void *adapter);

static void oracle_memset(u8 *dst, u8 val, unsigned n)
{
	unsigned i;

	for (i = 0; i < n; i++)
		dst[i] = val;
}

static void read_mbssid_cam_c_oracle(void *padapter, u8 cam_addr, u8 *mac)
{
	u8 poll = 1;
	u8 cam_ready = 0;
	u32 cam_data1 = 0;
	u16 cam_data2 = 0;

	if (rtw_rust_hal_com_cannot_run(padapter))
		return;

	rtw_rust_hal_com_write32(
		padapter, HOST_REG_MBIDCAMCFG_2,
		HOST_BIT_MBIDCAM_POLL |
			(((u32)cam_addr & HOST_MBIDCAM_ADDR_MASK) << HOST_MBIDCAM_ADDR_SHIFT));

	do {
		if (!(rtw_rust_hal_com_read32(padapter, HOST_REG_MBIDCAMCFG_2) &
		      HOST_BIT_MBIDCAM_POLL)) {
			cam_ready = 1;
			break;
		}
		poll++;
	} while ((poll % 10) != 0 && !rtw_rust_hal_com_cannot_run(padapter));

	if (!cam_ready)
		return;

	cam_data1 = rtw_rust_hal_com_read32(padapter, HOST_REG_MBIDCAMCFG_1);
	mac[0] = (u8)(cam_data1 & 0xffu);
	mac[1] = (u8)((cam_data1 >> 8) & 0xffu);
	mac[2] = (u8)((cam_data1 >> 16) & 0xffu);
	mac[3] = (u8)((cam_data1 >> 24) & 0xffu);

	cam_data2 = rtw_rust_hal_com_read16(padapter, HOST_REG_MBIDCAMCFG_2);
	mac[4] = (u8)(cam_data2 & 0xffu);
	mac[5] = (u8)((cam_data2 >> 8) & 0xffu);
}

int rtw_mbid_cam_dump_c_oracle(void *sel, const char *fun_name, void *adapter)
{
	u8 i;
	u8 mac_addr[ETH_ALEN];

	(void)rtw_rust_hal_com_adapter_to_dvobj(adapter);

	rtw_rust_hal_com_print_line(sel, "\n== MBSSID HW-CAM DUMP (");
	rtw_rust_hal_com_print_line(sel, fun_name);
	rtw_rust_hal_com_print_line(sel, ")==\n");

	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++) {
		oracle_memset(mac_addr, 0, ETH_ALEN);
		read_mbssid_cam_c_oracle(adapter, i, mac_addr);
		(void)sel;
		(void)i;
	}
	return 0;
}

#ifndef RUST_HAL_COM_MBID_CAM_READ_ORACLE
int rtw_mbid_cam_dump(void *sel, const char *fun_name, struct _adapter *adapter)
{
	return rtw_mbid_cam_dump_c_oracle(sel, fun_name, adapter);
}
#endif

void read_mbssid_cam_probe_c_oracle(void *adapter, u8 cam_addr, u8 *mac)
{
	read_mbssid_cam_c_oracle(adapter, cam_addr, mac);
}