/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host L2 recv_rest tests (W3-39).
 */
#ifndef HOST_RECV_TYPES_H
#define HOST_RECV_TYPES_H

#include <string.h>

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define TID_NUM 16
#define MAX_CONTINUAL_NORXPACKET_COUNT 4

typedef int ATOMIC_T;
#define ATOMIC_INC_RETURN(v) __sync_add_and_fetch((v), 1)
#define ATOMIC_SET(v, x) (*(v) = (x))

struct sta_info {
	ATOMIC_T continual_no_rx_packet[TID_NUM];
};

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index);

#endif /* HOST_RECV_TYPES_H */
