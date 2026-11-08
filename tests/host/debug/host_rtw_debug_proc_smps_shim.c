// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-226 (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_smps_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

#ifndef EFAULT
#define EFAULT 14
#endif
#ifndef EINVAL
#define EINVAL 22
#endif

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size)
{
	if (count < 1)
		return -1;
	if (count > tmp_size)
		return -1;
	if (buffer)
		memcpy(tmp, buffer, count);
	if (count < tmp_size)
		tmp[count] = '\0';
	return (ssize_t)count;
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

void rtw_ssmps_wk_cmd_stub(void *adapter, void *sta, u8 smps, u8 enqueue)
{
	(void)adapter;
	(void)sta;
	(void)smps;
	(void)enqueue;
}

void rtw_rust_debug_proc_emit_smps(void *sel, void *adapter)
{
	struct mlme_ext_priv *pmlmeext;

	if (!adapter)
		return;

	pmlmeext = &((struct _adapter *)adapter)->mlmeextpriv;
	RTW_PRINT_SEL(sel, "Static SMPS %s\n",
		      pmlmeext->ssmps_en ? "enable" : "disable");
	RTW_PRINT_SEL(sel, "Tx TP TH %d\n", pmlmeext->ssmps_tx_tp_th);
	RTW_PRINT_SEL(sel, "Rx TP TH %d\n", pmlmeext->ssmps_rx_tp_th);
#ifdef DBG_STATIC_SMPS
	RTW_PRINT_SEL(sel, "test %d, test_en:%d\n", pmlmeext->ssmps_test,
		      pmlmeext->ssmps_test_en);
#endif
}

long rtw_rust_debug_proc_set_smps(void *adapter, const void *buffer,
				  size_t count)
{
	struct mlme_ext_priv *pmlmeext;

	if (!adapter)
		return -EFAULT;

	pmlmeext = &((struct _adapter *)adapter)->mlmeextpriv;
	char tmp[32];
	u32 smps_en, smps_tx_tp = 0, smps_rx_tp = 0;
#ifdef DBG_STATIC_SMPS
	u32 smps_test = 0, smps_test_en = 0;
#endif

	if (!buffer)
		return -EFAULT;
	if (count < 1)
		return -EFAULT;
	if (count > sizeof(tmp))
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, count)) {
#ifdef DBG_STATIC_SMPS
		int num = sscanf(tmp, "%u %u %u %u %u", &smps_en, &smps_tx_tp,
				 &smps_rx_tp, &smps_test, &smps_test_en);
#else
		int num = sscanf(tmp, "%u %u %u", &smps_en, &smps_tx_tp,
				 &smps_rx_tp);
#endif

		if (num < 1)
			return -EINVAL;

		pmlmeext->ssmps_en = (u8)smps_en;
		if (smps_tx_tp) {
			pmlmeext->ssmps_tx_tp_th = (u16)smps_tx_tp;
			pmlmeext->ssmps_rx_tp_th = (u16)smps_tx_tp;
		}
		if (smps_rx_tp)
			pmlmeext->ssmps_rx_tp_th = (u16)smps_rx_tp;
#ifdef DBG_STATIC_SMPS
		if (num > 3) {
			pmlmeext->ssmps_test = (u8)smps_test;
			pmlmeext->ssmps_test_en = (u8)smps_test_en;
		}
#endif
	}

	return (long)count;
}
