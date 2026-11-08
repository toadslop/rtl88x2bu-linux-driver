// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-226 proc static SMPS handlers (host L2). */

#include "host_rtw_debug_proc_smps_types.h"

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

#ifndef EFAULT
#define EFAULT 14
#endif
#ifndef EINVAL
#define EINVAL 22
#endif

int proc_get_smps(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;

	(void)v;

	RTW_PRINT_SEL(m, "Static SMPS %s\n",
		      pmlmeext->ssmps_en ? "enable" : "disable");
	RTW_PRINT_SEL(m, "Tx TP TH %d\n", pmlmeext->ssmps_tx_tp_th);
	RTW_PRINT_SEL(m, "Rx TP TH %d\n", pmlmeext->ssmps_rx_tp_th);
#ifdef DBG_STATIC_SMPS
	RTW_PRINT_SEL(m, "test %d, test_en:%d\n", pmlmeext->ssmps_test,
		      pmlmeext->ssmps_test_en);
#endif
	return 0;
}

ssize_t proc_set_smps(void *file, const void *buffer, size_t count,
		      long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	char tmp[32];
	u32 smps_en, smps_tx_tp = 0, smps_rx_tp = 0;
#ifdef DBG_STATIC_SMPS
	u32 smps_test = 0, smps_test_en = 0;
#endif

	(void)file;
	(void)pos;

	if (!buffer)
		return -EFAULT;
	if (count < 1)
		return -EFAULT;
	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

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

	return (ssize_t)count;
}
