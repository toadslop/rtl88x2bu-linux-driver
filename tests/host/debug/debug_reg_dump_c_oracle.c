// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-143/W3-169 register dump leaves (8822BU window). */

#include <stdio.h>

typedef unsigned int u32;
typedef unsigned char u8;
typedef struct _adapter _adapter;

struct hal_spec_t {
	u8 rf_reg_path_num;
};

struct hal_data_t {
	struct hal_spec_t hal_spec;
};

struct _adapter {
	struct hal_data_t hal_data;
};

#define GET_HAL_DATA(adapter) (&(adapter)->hal_data)
#define GET_HAL_SPEC(adapter) (&(GET_HAL_DATA(adapter))->hal_spec)

#define RTW_PRINT_SEL(sel, fmt, ...) \
	do { \
		char _b[256]; \
		int _n = snprintf(_b, sizeof(_b), fmt, ##__VA_ARGS__); \
		if (_n > 0) \
			fwrite(_b, 1, (size_t)_n, stdout); \
	} while (0)
#define _RTW_PRINT_SEL(sel, fmt, ...) RTW_PRINT_SEL(sel, fmt, ##__VA_ARGS__)
#define rtw_read32(adapter, addr) ((void)(adapter), (addr) ^ 0xa5a5a5a5u)

u32 rtw_hal_read_rfreg(_adapter *adapter, u32 path, u32 addr, u32 mask)
{
	return (path << 24) | (addr << 8) | (mask & 0xffu);
}

static void dump_win(void *sel, _adapter *a, int start, int end, int *j)
{
	int i;

	for (i = start; i < end; i += 4) {
		if (*j % 4 == 1)
			RTW_PRINT_SEL(sel, "0x%04x", i);
		_RTW_PRINT_SEL(sel, " 0x%08x ", rtw_read32(a, i));
		if (((*j)++) % 4 == 0)
			_RTW_PRINT_SEL(sel, "\n");
	}
}

static void dump_bb_ex_line(void *sel, _adapter *a, int addr)
{
	RTW_PRINT_SEL(sel, "0x%04x", addr);
	_RTW_PRINT_SEL(sel, " 0x%08x ", rtw_read32(a, addr));
	_RTW_PRINT_SEL(sel, "\n");
}

void mac_reg_dump(void *sel, _adapter *adapter)
{
	int j = 1;

	RTW_PRINT_SEL(sel, "======= MAC REG =======\n");
	dump_win(sel, adapter, 0x0, 0x800, &j);
	dump_win(sel, adapter, 0x1000, 0x1800, &j);
}

void bb_reg_dump(void *sel, _adapter *adapter)
{
	int j = 1;

	RTW_PRINT_SEL(sel, "======= BB REG =======\n");
	dump_win(sel, adapter, 0x800, 0x1000, &j);
	dump_win(sel, adapter, 0x1800, 0x2000, &j);
}

void bb_reg_dump_ex(void *sel, _adapter *adapter)
{
	int i;

	RTW_PRINT_SEL(sel, "======= BB REG =======\n");
	for (i = 0x800; i < 0x1000; i += 4)
		dump_bb_ex_line(sel, adapter, i);
	for (i = 0x1800; i < 0x2000; i += 4)
		dump_bb_ex_line(sel, adapter, i);
}

void rf_reg_dump(void *sel, _adapter *adapter)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(adapter);
	int i, j = 1, path;
	u32 value;
	u8 path_nums = hal_spec->rf_reg_path_num;

	RTW_PRINT_SEL(sel, "======= RF REG =======\n");

	for (path = 0; path < path_nums; path++) {
		RTW_PRINT_SEL(sel, "RF_Path(%x)\n", path);
		for (i = 0; i < 0x100; i++) {
			value = rtw_hal_read_rfreg(adapter, path, i, 0xffffffff);
			if (j % 4 == 1)
				RTW_PRINT_SEL(sel, "0x%02x ", i);
			_RTW_PRINT_SEL(sel, " 0x%08x ", value);
			if ((j++) % 4 == 0)
				_RTW_PRINT_SEL(sel, "\n");
		}
	}
}
