// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-143 MAC/BB register dump leaves (8822BU window). */

#include <stdio.h>

typedef unsigned int u32;
typedef struct _adapter _adapter;

#define RTW_PRINT_SEL(sel, fmt, ...) \
	do { \
		char _b[256]; \
		int _n = snprintf(_b, sizeof(_b), fmt, ##__VA_ARGS__); \
		if (_n > 0) \
			fwrite(_b, 1, (size_t)_n, stdout); \
	} while (0)
#define _RTW_PRINT_SEL(sel, fmt, ...) RTW_PRINT_SEL(sel, fmt, ##__VA_ARGS__)
#define rtw_read32(adapter, addr) ((void)(adapter), (addr) ^ 0xa5a5a5a5u)

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
