// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-222 MCC setters (host L2). */

#include "host_rtw_debug_proc_mcc_types.h"

ssize_t proc_set_mcc_enable(void *file, const void *buffer, size_t count,
			    long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_enable(padapter, buffer, count);
}

ssize_t proc_set_mcc_duration(void *file, const void *buffer, size_t count,
			    long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_duration(padapter, buffer, count);
}

#ifdef CONFIG_MCC_PHYDM_OFFLOAD
ssize_t proc_set_mcc_phydm_offload_enable(void *file, const void *buffer,
					  size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_phydm_offload_enable(padapter, buffer, count);
}
#endif

ssize_t proc_set_mcc_single_tx_criteria(void *file, const void *buffer,
					size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_single_tx_criteria(padapter, buffer, count);
}

ssize_t proc_set_mcc_ap_bw20_target_tp(void *file, const void *buffer,
				       size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_ap_bw20_target_tp(padapter, buffer, count);
}

ssize_t proc_set_mcc_ap_bw40_target_tp(void *file, const void *buffer,
				       size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_ap_bw40_target_tp(padapter, buffer, count);
}

ssize_t proc_set_mcc_ap_bw80_target_tp(void *file, const void *buffer,
				       size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_ap_bw80_target_tp(padapter, buffer, count);
}

ssize_t proc_set_mcc_sta_bw20_target_tp(void *file, const void *buffer,
					size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_sta_bw20_target_tp(padapter, buffer, count);
}

ssize_t proc_set_mcc_sta_bw40_target_tp(void *file, const void *buffer,
					size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_sta_bw40_target_tp(padapter, buffer, count);
}

ssize_t proc_set_mcc_sta_bw80_target_tp(void *file, const void *buffer,
					size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_mcc_sta_bw80_target_tp(padapter, buffer, count);
}
