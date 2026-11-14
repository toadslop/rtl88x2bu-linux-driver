// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: BB/RF register dispatch (W4-82). */

typedef unsigned int u32;
typedef unsigned char u8;
typedef struct _adapter _adapter;

u32 rtw_hal_read_bbreg(_adapter *padapter, u32 RegAddr, u32 BitMask)
{
	(void)padapter;
	(void)RegAddr;
	(void)BitMask;
	return 0;
}

void rtw_hal_write_bbreg(_adapter *padapter, u32 RegAddr, u32 BitMask, u32 Data)
{
	(void)padapter;
	(void)RegAddr;
	(void)BitMask;
	(void)Data;
}

u32 rtw_hal_read_rfreg(_adapter *padapter, u32 eRFPath, u32 RegAddr, u32 BitMask)
{
	(void)padapter;
	(void)eRFPath;
	(void)RegAddr;
	(void)BitMask;
	return 0;
}

void rtw_hal_write_rfreg(_adapter *padapter, u32 eRFPath, u32 RegAddr, u32 BitMask, u32 Data)
{
	(void)padapter;
	(void)eRFPath;
	(void)RegAddr;
	(void)BitMask;
	(void)Data;
}

#ifdef CONFIG_SYSON_INDIRECT_ACCESS
u32 rtw_hal_read_syson_reg(_adapter *padapter, u32 RegAddr, u32 BitMask)
{
	(void)padapter;
	(void)RegAddr;
	(void)BitMask;
	return 0;
}

void rtw_hal_write_syson_reg(_adapter *padapter, u32 RegAddr, u32 BitMask, u32 Data)
{
	(void)padapter;
	(void)RegAddr;
	(void)BitMask;
	(void)Data;
}
#endif