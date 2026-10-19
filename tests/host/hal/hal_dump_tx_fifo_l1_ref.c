// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: dump_TX_FIFO symbol surface (W4-42). */

struct _adapter;

void dump_TX_FIFO(struct _adapter *padapter, unsigned char page_num, unsigned short page_size)
{
	(void)padapter;
	(void)page_num;
	(void)page_size;
}
