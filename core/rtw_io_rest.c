/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_IO_REST_C_

#ifdef HOST_IO_TEST
#include "host_io_types.h"
#else
#include <drv_types.h>
#include <rtw_io.h>
#endif

/*
* Increase and check if the continual_io_error of this @param dvobjprive is larger than MAX_CONTINUAL_IO_ERR
* @return _TRUE:
* @return _FALSE:
*/
#if !defined(CONFIG_RUST) || defined(HOST_IO_TEST)
int rtw_inc_and_chk_continual_io_error(struct dvobj_priv *dvobj)
{
	int ret = _FALSE;
	int value;

	value = ATOMIC_INC_RETURN(&dvobj->continual_io_error);
	if (value > MAX_CONTINUAL_IO_ERR) {
		RTW_INFO("[dvobj:%p][ERROR] continual_io_error:%d > %d\n", dvobj, value, MAX_CONTINUAL_IO_ERR);
		ret = _TRUE;
	} else {
		/* RTW_INFO("[dvobj:%p] continual_io_error:%d\n", dvobj, value); */
	}
	return ret;
}

/*
* Set the continual_io_error of this @param dvobjprive to 0
*/
void rtw_reset_continual_io_error(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->continual_io_error, 0);
}
#endif /* !CONFIG_RUST || HOST_IO_TEST */
#ifdef DBG_IO
#define RTW_IO_SNIFF_TYPE_RANGE	0 /* specific address range is accessed */
#define RTW_IO_SNIFF_TYPE_VALUE	1 /* value match for sniffed range */

struct rtw_io_sniff_ent {
	u8 chip;
	u8 hci;
	u32 addr;
	u8 type;
	union {
		u32 end_addr;
		struct {
			u32 mask;
			u32 val;
			bool equal;
		} vm; /* value match */
	} u;
	bool trace;
	char *tag;
	bool (*assert_protsel)(_adapter *adapter, u32 addr, u8 len);
};

#define RTW_IO_SNIFF_RANGE_ENT(_chip, _hci, _addr, _end_addr, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.end_addr = _end_addr, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_RANGE,}

#define RTW_IO_SNIFF_RANGE_PROT_ENT(_chip, _hci, _addr, _end_addr, _assert_protsel, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.end_addr = _end_addr, .trace = 1, .assert_protsel = _assert_protsel, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_RANGE,}

#define RTW_IO_SNIFF_VALUE_ENT(_chip, _hci, _addr, _mask, _val, _equal, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.vm.mask = _mask, .u.vm.val = _val, .u.vm.equal = _equal, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_VALUE,}

/* part or all sniffed range is enabled (not all 0) */
#define RTW_IO_SNIFF_EN_ENT(_chip, _hci, _addr, _mask, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.vm.mask = _mask, .u.vm.val = 0, .u.vm.equal = 0, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_VALUE,}

/* part or all sniffed range is disabled (not all 1) */
#define RTW_IO_SNIFF_DIS_ENT(_chip, _hci, _addr, _mask, _trace, _tag) \
	{.chip = _chip, .hci = _hci, .addr = _addr, .u.vm.mask = _mask, .u.vm.val = 0xFFFFFFFF, .u.vm.equal = 0, .trace = _trace, .tag = _tag, .type = RTW_IO_SNIFF_TYPE_VALUE,}

const struct rtw_io_sniff_ent read_sniff[] = {
#ifdef DBG_IO_HCI_EN_CHK
	RTW_IO_SNIFF_EN_ENT(MAX_CHIP_TYPE, RTW_SDIO, 0x02, 0x1FC, 1, "SDIO 0x02[8:2] not all 0"),
	RTW_IO_SNIFF_EN_ENT(MAX_CHIP_TYPE, RTW_USB, 0x02, 0x1E0, 1, "USB 0x02[8:5] not all 0"),
	RTW_IO_SNIFF_EN_ENT(MAX_CHIP_TYPE, RTW_PCIE, 0x02, 0x01C, 1, "PCI 0x02[4:2] not all 0"),
#endif
#ifdef DBG_IO_SNIFF_EXAMPLE
	RTW_IO_SNIFF_RANGE_ENT(MAX_CHIP_TYPE, 0, 0x522, 0x522, 0, "read TXPAUSE"),
	RTW_IO_SNIFF_DIS_ENT(MAX_CHIP_TYPE, 0, 0x02, 0x3, 0, "0x02[1:0] not all 1"),
#endif
#ifdef HOST_IO_TEST
	RTW_IO_SNIFF_RANGE_ENT(1, RTW_USB, 0x600, 0x600, 0, "host chip1 usb read"),
	RTW_IO_SNIFF_VALUE_ENT(MAX_CHIP_TYPE, 0, 0x100, 0xFF, 0xAB, 1, 0, "host read value equal len1"),
	RTW_IO_SNIFF_VALUE_ENT(MAX_CHIP_TYPE, 0, 0x200, 0xFFFF, 0x1234, 1, 0, "host read value equal len2"),
	RTW_IO_SNIFF_VALUE_ENT(MAX_CHIP_TYPE, 0, 0x300, 0xFFFFFFFF, 0x12345678, 1, 0, "host read value equal len4"),
	RTW_IO_SNIFF_VALUE_ENT(MAX_CHIP_TYPE, 0, 0x102, 0xFF, 0x42, 1, 0, "host read unaligned len4 equal"),
	RTW_IO_SNIFF_VALUE_ENT(MAX_CHIP_TYPE, 0, 0x201, 0xFF, 0x34, 1, 0, "host read unaligned len2 equal"),
	RTW_IO_SNIFF_VALUE_ENT(MAX_CHIP_TYPE, 0, 0x500, 0xFF0000, 0x420000, 1, 0, "host read negative mask_shift"),
#endif
#ifdef DBG_IO_PROT_SEL
	RTW_IO_SNIFF_RANGE_PROT_ENT(MAX_CHIP_TYPE, 0, 0x1501, 0x1513, rtw_assert_protsel_port, "protsel port"),
	RTW_IO_SNIFF_RANGE_PROT_ENT(MAX_CHIP_TYPE, 0, 0x153a, 0x153b, rtw_assert_protsel_atimdtim, "protsel atimdtim"),
#endif
};

int read_sniff_num = sizeof(read_sniff) / sizeof(struct rtw_io_sniff_ent);

const struct rtw_io_sniff_ent write_sniff[] = {
#ifdef DBG_IO_HCI_EN_CHK
	RTW_IO_SNIFF_EN_ENT(MAX_CHIP_TYPE, RTW_SDIO, 0x02, 0x1FC, 1, "SDIO 0x02[8:2] not all 0"),
	RTW_IO_SNIFF_EN_ENT(MAX_CHIP_TYPE, RTW_USB, 0x02, 0x1E0, 1, "USB 0x02[8:5] not all 0"),
	RTW_IO_SNIFF_EN_ENT(MAX_CHIP_TYPE, RTW_PCIE, 0x02, 0x01C, 1, "PCI 0x02[4:2] not all 0"),
#endif
#ifdef DBG_IO_8822C_1TX_PATH_EN
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x1a04, 0xc0000000, 0x02, 1, 0, "write tx_path_en_cck A enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x1a04, 0xc0000000, 0x01, 1, 0, "write tx_path_en_cck B enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x1a04, 0xc0000000, 0x03, 1, 1, "write tx_path_en_cck AB enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x820, 0x03, 0x01, 1, 0, "write tx_path_en_ofdm_1sts A enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x820, 0x03, 0x02, 1, 0, "write tx_path_en_ofdm_1sts B enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x820, 0x03, 0x03, 1, 1, "write tx_path_en_ofdm_1sts AB enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x820, 0x30, 0x01, 1, 0, "write tx_path_en_ofdm_2sts A enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x820, 0x30, 0x02, 1, 0, "write tx_path_en_ofdm_2sts B enabled"),
	RTW_IO_SNIFF_VALUE_ENT(RTL8822C, 0, 0x820, 0x30, 0x03, 1, 1, "write tx_path_en_ofdm_2sts AB enabled"),
#endif
#ifdef DBG_IO_SNIFF_EXAMPLE
	RTW_IO_SNIFF_RANGE_ENT(MAX_CHIP_TYPE, 0, 0x522, 0x522, 0, "write TXPAUSE"),
	RTW_IO_SNIFF_DIS_ENT(MAX_CHIP_TYPE, 0, 0x02, 0x3, 0, "0x02[1:0] not all 1"),
#endif
#ifdef HOST_IO_TEST
	RTW_IO_SNIFF_VALUE_ENT(MAX_CHIP_TYPE, 0, 0x400, 0xFFFF, 0x5600, 1, 0, "host write value equal len2"),
#endif
};

int write_sniff_num = sizeof(write_sniff) / sizeof(struct rtw_io_sniff_ent);

#if !defined(CONFIG_RUST) || defined(HOST_IO_TEST)
static bool match_io_sniff_ranges(_adapter *adapter
	, const struct rtw_io_sniff_ent *sniff, int i, u32 addr, u16 len)
{

	/* check if IO range after sniff end address */
	if (addr > sniff->u.end_addr)
		return 0;

	if (sniff->assert_protsel &&
	    sniff->assert_protsel(adapter, addr, len))
		return 0;

	return 1;
}

static bool match_io_sniff_value(_adapter *adapter
	, const struct rtw_io_sniff_ent *sniff, int i, u32 addr, u8 len, u32 val)
{
	u8 sniff_len;
	s8 mask_shift;
	u32 mask;
	s8 value_shift;
	u32 value;
	bool ret = 0;

	/* check if IO range after sniff end address */
	sniff_len = 4;
	while (!(sniff->u.vm.mask & (0xFF << ((sniff_len - 1) * 8)))) {
		sniff_len--;
		if (sniff_len == 0)
			goto exit;
	}
	if (sniff->addr + sniff_len <= addr)
		goto exit;

	/* align to IO addr */
	mask_shift = (sniff->addr - addr) * 8;
	value_shift = mask_shift + bitshift(sniff->u.vm.mask);
	if (mask_shift > 0)
		mask = sniff->u.vm.mask << mask_shift;
	else if (mask_shift < 0)
		mask = sniff->u.vm.mask >> -mask_shift;
	else
		mask = sniff->u.vm.mask;

	if (value_shift > 0)
		value = sniff->u.vm.val << value_shift;
	else if (mask_shift < 0)
		value = sniff->u.vm.val >> -value_shift;
	else
		value = sniff->u.vm.val;

	if ((sniff->u.vm.equal && (mask & val) == (mask & value))
		|| (!sniff->u.vm.equal && (mask & val) != (mask & value))
	) {
		ret = 1;
		if (0)
			RTW_INFO(FUNC_ADPT_FMT" addr:0x%x len:%u val:0x%x (i:%d sniff_len:%u m_shift:%d mask:0x%x v_shifd:%d value:0x%x equal:%d)\n"
				, FUNC_ADPT_ARG(adapter), addr, len, val, i, sniff_len, mask_shift, mask, value_shift, value, sniff->u.vm.equal);
	}

exit:
	return ret;
}

static bool match_io_sniff(_adapter *adapter
	, const struct rtw_io_sniff_ent *sniff, int i, u32 addr, u8 len, u32 val)
{
	bool ret = 0;

	if (sniff->chip != MAX_CHIP_TYPE
		&& sniff->chip != rtw_get_chip_type(adapter))
		goto exit;
	if (sniff->hci
		&& !(sniff->hci & rtw_get_intf_type(adapter)))
		goto exit;
	if (sniff->addr >= addr + len) /* IO range below sniff start address */
		goto exit;

	switch (sniff->type) {
	case RTW_IO_SNIFF_TYPE_RANGE:
		ret = match_io_sniff_ranges(adapter, sniff, i, addr, len);
		break;
	case RTW_IO_SNIFF_TYPE_VALUE:
		if (len == 1 || len == 2 || len == 4)
			ret = match_io_sniff_value(adapter, sniff, i, addr, len, val);
		break;
	default:
		rtw_warn_on(1);
		break;
	}

exit:
	return ret;
}

u32 match_read_sniff(_adapter *adapter, u32 addr, u16 len, u32 val)
{
	int i;
	bool trace = 0;
	u32 match = 0;

	for (i = 0; i < read_sniff_num; i++) {
		if (match_io_sniff(adapter, &read_sniff[i], i, addr, len, val)) {
			match++;
			trace |= read_sniff[i].trace;
			if (read_sniff[i].tag)
				RTW_INFO("DBG_IO TAG %s\n", read_sniff[i].tag);
		}
	}

	rtw_warn_on(trace);

	return match;
}

u32 match_write_sniff(_adapter *adapter, u32 addr, u16 len, u32 val)
{
	int i;
	bool trace = 0;
	u32 match = 0;

	for (i = 0; i < write_sniff_num; i++) {
		if (match_io_sniff(adapter, &write_sniff[i], i, addr, len, val)) {
			match++;
			trace |= write_sniff[i].trace;
			if (write_sniff[i].tag)
				RTW_INFO("DBG_IO TAG %s\n", write_sniff[i].tag);
		}
	}

	rtw_warn_on(trace);

	return match;
}

#endif /* !CONFIG_RUST || HOST_IO_TEST */

struct rf_sniff_ent {
	u8 path;
	u16 reg;
	u32 mask;
};
#ifdef HOST_IO_TEST
struct rf_sniff_ent rf_read_sniff_ranges[] = {
	{0, 0x55, 0xFF},
	{MAX_RF_PATH, 0x66, 0x0F},
};
struct rf_sniff_ent rf_write_sniff_ranges[] = {
	{1, 0x55, 0xFF},
};
#else
struct rf_sniff_ent rf_read_sniff_ranges[] = {
	/* example for all path addr 0x55 with all RF Reg mask */
	/* {MAX_RF_PATH, 0x55, bRFRegOffsetMask}, */
};

struct rf_sniff_ent rf_write_sniff_ranges[] = {
	/* example for all path addr 0x55 with all RF Reg mask */
	/* {MAX_RF_PATH, 0x55, bRFRegOffsetMask}, */
};
#endif
int rf_read_sniff_num = sizeof(rf_read_sniff_ranges) / sizeof(struct rf_sniff_ent);
int rf_write_sniff_num = sizeof(rf_write_sniff_ranges) / sizeof(struct rf_sniff_ent);

#if !defined(CONFIG_RUST) || defined(HOST_IO_TEST)
bool match_rf_read_sniff_ranges(_adapter *adapter, u8 path, u32 addr, u32 mask)
{
	int i;

	for (i = 0; i < rf_read_sniff_num; i++) {
		if (rf_read_sniff_ranges[i].path == MAX_RF_PATH || rf_read_sniff_ranges[i].path == path)
			if (addr == rf_read_sniff_ranges[i].reg && (mask & rf_read_sniff_ranges[i].mask))
				return _TRUE;
	}

	return _FALSE;
}

bool match_rf_write_sniff_ranges(_adapter *adapter, u8 path, u32 addr, u32 mask)
{
	int i;

	for (i = 0; i < rf_write_sniff_num; i++) {
		if (rf_write_sniff_ranges[i].path == MAX_RF_PATH || rf_write_sniff_ranges[i].path == path)
			if (addr == rf_write_sniff_ranges[i].reg && (mask & rf_write_sniff_ranges[i].mask))
				return _TRUE;
	}

	return _FALSE;
}
#endif /* !CONFIG_RUST || HOST_IO_TEST */

#endif /* DBG_IO */

#if defined(CONFIG_RUST) && !defined(HOST_IO_TEST)
ATOMIC_T *rtw_rust_dvobj_continual_io_error(struct dvobj_priv *dvobj)
{
	return &dvobj->continual_io_error;
}

int rtw_rust_atomic_inc_return(ATOMIC_T *v)
{
	return ATOMIC_INC_RETURN(v);
}

void rtw_rust_atomic_set(ATOMIC_T *v, int val)
{
	ATOMIC_SET(v, val);
}

u8 rtw_rust_get_chip_type(_adapter *adapter)
{
	return rtw_get_chip_type(adapter);
}

u8 rtw_rust_get_intf_type(_adapter *adapter)
{
	return rtw_get_intf_type(adapter);
}

void rtw_rust_io_warn_on(int condition)
{
	rtw_warn_on(condition);
}

void rtw_rust_io_dbg_tag(const char *tag)
{
	if (tag)
		RTW_INFO("DBG_IO TAG %s\n", tag);
}

void rtw_rust_io_continual_io_error_log(void *dvobj, int value, int max)
{
	RTW_INFO("[dvobj:%p][ERROR] continual_io_error:%d > %d\n", dvobj, value, max);
}

bool rtw_rust_io_sniff_assert_protsel(bool (*fn)(_adapter *, u32, u8),
				      _adapter *adapter, u32 addr, u8 len)
{
	if (!fn)
		return false;
	return fn(adapter, addr, len);
}

u8 rtw_rust_io_max_chip_type(void)
{
	return MAX_CHIP_TYPE;
}

u8 rtw_rust_io_max_rf_path(void)
{
	return MAX_RF_PATH;
}
#endif /* CONFIG_RUST && !HOST_IO_TEST */
