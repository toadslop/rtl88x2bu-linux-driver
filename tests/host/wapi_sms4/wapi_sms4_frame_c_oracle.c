// SPDX-License-Identifier: GPL-2.0
/* C oracle for WAPI SMS4 frame encrypt (W3-142 PR1). */
#include <string.h>

#include "host_wapi_sms4_frame_oracle.h"

#define _SUCCESS 1
#define _FAIL 0

void host_wapi_sms4_frame_adapter_init(struct host_wapi_adapter *a)
{
	memset(a, 0, sizeof(*a));
	a->wapi_support = 1;
	a->wapi_info.b_wapi_enable = 1;
	a->wapi_info.extra_prefix_len = HOST_WAPI_EXT_LEN;
	a->wapi_info.extra_postfix_len = HOST_WAPI_SMS4_MIC_LEN;
}

void host_sec_sw_sms4_encryption(struct host_wapi_adapter *padapter,
				 struct host_wapi_xmit_frame *pxmit)
{
	/* Matches core/rtw_wapi_sms4_rest.c + rust/wapi_sms4_frame.rs (frame path disabled). */
	(void)padapter;
	(void)pxmit;
}

u8 host_sec_sw_sms4_decryption(struct host_wapi_adapter *padapter,
			       union host_wapi_recv_frame *precv)
{
	/* Matches core/rtw_wapi_sms4_rest.c temporary HW-decrypt drop. */
	(void)padapter;
	(void)precv;
	return 0;
}

u32 host_rtw_sms4_encrypt(struct host_wapi_adapter *padapter,
			  struct host_wapi_xmit_frame *pxmit)
{
	if (!padapter->wapi_support || !padapter->wapi_info.b_wapi_enable)
		return _FAIL;
	if (!pxmit->buf_addr)
		return _FAIL;
	host_sec_sw_sms4_encryption(padapter, pxmit);
	return _SUCCESS;
}

u32 host_rtw_sms4_decrypt(struct host_wapi_adapter *padapter,
			  union host_wapi_recv_frame *precv)
{
	if (!padapter->wapi_support || !padapter->wapi_info.b_wapi_enable)
		return _FAIL;
	(void)precv;
	/* Matches rust/wapi_sms4_frame.rs rtw_sms4_decrypt after enable checks. */
	return _FAIL;
}

#ifndef WAPI_SMS4_FRAME_L1_REF
/* Production symbol names for Rust L1 / differential runner */
void SecSWSMS4Encryption(void *padapter, u8 *pxmitframe)
{
	host_sec_sw_sms4_encryption((struct host_wapi_adapter *)padapter,
				    (struct host_wapi_xmit_frame *)pxmitframe);
}

u32 rtw_sms4_encrypt(void *padapter, u8 *pxmitframe)
{
	return host_rtw_sms4_encrypt((struct host_wapi_adapter *)padapter,
				     (struct host_wapi_xmit_frame *)pxmitframe);
}

u8 SecSWSMS4Decryption(void *padapter, u8 *precv_frame, void *precv_priv)
{
	(void)precv_priv;
	return host_sec_sw_sms4_decryption((struct host_wapi_adapter *)padapter,
					   (union host_wapi_recv_frame *)precv_frame);
}

u32 rtw_sms4_decrypt(void *padapter, u8 *precvframe)
{
	return host_rtw_sms4_decrypt((struct host_wapi_adapter *)padapter,
				     (union host_wapi_recv_frame *)precvframe);
}

#else

void SecSWSMS4Encryption(void *padapter, u8 *pxmitframe)
{
	host_sec_sw_sms4_encryption((struct host_wapi_adapter *)padapter,
				    (struct host_wapi_xmit_frame *)pxmitframe);
}

u32 rtw_sms4_encrypt(void *padapter, u8 *pxmitframe)
{
	return host_rtw_sms4_encrypt((struct host_wapi_adapter *)padapter,
				     (struct host_wapi_xmit_frame *)pxmitframe);
}

u8 SecSWSMS4Decryption(void *padapter, u8 *precv_frame, void *precv_priv)
{
	(void)precv_priv;
	return host_sec_sw_sms4_decryption((struct host_wapi_adapter *)padapter,
					   (union host_wapi_recv_frame *)precv_frame);
}

u32 rtw_sms4_decrypt(void *padapter, u8 *precvframe)
{
	return host_rtw_sms4_decrypt((struct host_wapi_adapter *)padapter,
				     (union host_wapi_recv_frame *)precvframe);
}

#endif
