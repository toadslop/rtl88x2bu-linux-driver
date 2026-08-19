/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 rtw_iol_rest tests (W3-50).
 */
#ifndef HOST_IOL_TYPES_H
#define HOST_IOL_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define TXDESC_SIZE 48
#define PACKET_OFFSET_SZ 8
#define TXDESC_OFFSET (TXDESC_SIZE + PACKET_OFFSET_SZ)
#define MAX_XMITBUF_SZ 20480

#define IOL_CMD_LLT 0x00
#define IOL_CMD_WB_REG 0x02
#define IOL_CMD_WW_REG 0x03
#define IOL_CMD_WD_REG 0x04
#define IOL_CMD_DELAY_US 0x80
#define IOL_CMD_DELAY_MS 0x81
#define IOL_CMD_END 0x83

#define RTW_PUT_BE16(a, val)                     \
	do {                                     \
		(a)[0] = (u8)(((u16)(val)) >> 8); \
		(a)[1] = (u8)(((u16)(val)) & 0xff); \
	} while (0)
#define RTW_PUT_BE32(a, val)                                          \
	do {                                                          \
		(a)[0] = (u8)((((u32)(val)) >> 24) & 0xff);            \
		(a)[1] = (u8)((((u32)(val)) >> 16) & 0xff);           \
		(a)[2] = (u8)((((u32)(val)) >> 8) & 0xff);            \
		(a)[3] = (u8)(((u32)(val)) & 0xff);                  \
	} while (0)

#define RTW_INFO(...) do { } while (0)

typedef struct _io_offload_cmd {
	u8 rsvd0;
	u8 cmd;
	u16 address;
	u32 value;
} IOL_CMD;

struct pkt_attrib {
	u32 pktlen;
	u32 last_txcmdsz;
};

struct xmit_frame {
	struct pkt_attrib attrib;
	u8 *buf_addr;
};

int rtw_IOL_append_cmds(struct xmit_frame *xmit_frame, u8 *IOL_cmds, u32 cmd_len);
int rtw_IOL_append_LLT_cmd(struct xmit_frame *xmit_frame, u8 page_boundary);
int _rtw_IOL_append_WB_cmd(struct xmit_frame *xmit_frame, u16 addr, u8 value);
int _rtw_IOL_append_WW_cmd(struct xmit_frame *xmit_frame, u16 addr, u16 value);
int _rtw_IOL_append_WD_cmd(struct xmit_frame *xmit_frame, u16 addr, u32 value);
int rtw_IOL_append_DELAY_US_cmd(struct xmit_frame *xmit_frame, u16 us);
int rtw_IOL_append_DELAY_MS_cmd(struct xmit_frame *xmit_frame, u16 ms);
int rtw_IOL_append_END_cmd(struct xmit_frame *xmit_frame);

#endif /* HOST_IOL_TYPES_H */
