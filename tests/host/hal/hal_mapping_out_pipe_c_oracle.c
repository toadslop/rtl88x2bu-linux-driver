// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-15 Hal_MappingOutPipe (from hal/hal_com_rest.c). */

#include <stdint.h>

#define _TRUE 1
#define _FALSE 0
#define QN 8

typedef uint8_t BOOLEAN;
typedef uint8_t u8;

struct host_hal_mapping_adapter {
	u8 wifi_spec;
	int rt_out_pipe[4];
	u8 queue2pipe[QN];
};

static void map_queues(struct host_hal_mapping_adapter *a, const u8 idx[QN])
{
	int i;

	for (i = 0; i < QN; i++)
		a->queue2pipe[i] = (u8)a->rt_out_pipe[idx[i]];
}

static const u8 one_map[QN] = {0, 0, 0, 0, 0, 0, 0, 0};
static const u8 two_wmm[QN] = {1, 0, 1, 0, 0, 0, 0, 0};
static const u8 two_typ[QN] = {0, 0, 1, 1, 0, 0, 0, 0};
static const u8 three_wmm[QN] = {0, 1, 2, 1, 0, 0, 0, 0};
static const u8 three_typ[QN] = {0, 1, 2, 2, 0, 0, 0, 0};

BOOLEAN Hal_MappingOutPipe(void *adapter, u8 num_out_pipe)
{
	struct host_hal_mapping_adapter *a = adapter;
	int wmm;

	if (!a)
		return _FALSE;
	wmm = a->wifi_spec ? _TRUE : _FALSE;
	switch (num_out_pipe) {
	case 1:
		map_queues(a, one_map);
		return _TRUE;
	case 2:
		map_queues(a, wmm ? two_wmm : two_typ);
		return _TRUE;
	case 3:
	case 4:
	case 5:
	case 6:
		map_queues(a, wmm ? three_wmm : three_typ);
		return _TRUE;
	default:
		return _FALSE;
	}
}
