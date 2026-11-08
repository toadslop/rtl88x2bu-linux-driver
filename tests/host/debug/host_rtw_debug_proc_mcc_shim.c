// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-222 MCC proc setters (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_mcc_types.h"

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

long rtw_rust_debug_proc_set_mcc_enable(void *adapter, const void *buffer,
					size_t count)
{
	return host_proc_set_mcc_enable(adapter, buffer, count);
}

long rtw_rust_debug_proc_set_mcc_duration(void *adapter, const void *buffer,
					  size_t count)
{
	return host_proc_set_mcc_duration(adapter, buffer, count);
}

#ifdef CONFIG_MCC_PHYDM_OFFLOAD
long rtw_rust_debug_proc_set_mcc_phydm_offload_enable(void *adapter,
						      const void *buffer,
						      size_t count)
{
	return host_proc_set_mcc_phydm_offload_enable(adapter, buffer, count);
}
#endif

long rtw_rust_debug_proc_set_mcc_single_tx_criteria(void *adapter,
						    const void *buffer,
						    size_t count)
{
	return host_proc_set_mcc_single_tx_criteria(adapter, buffer, count);
}

long rtw_rust_debug_proc_set_mcc_ap_bw20_target_tp(void *adapter,
						   const void *buffer,
						   size_t count)
{
	return host_proc_set_mcc_ap_bw20_target_tp(adapter, buffer, count);
}

long rtw_rust_debug_proc_set_mcc_ap_bw40_target_tp(void *adapter,
						   const void *buffer,
						   size_t count)
{
	return host_proc_set_mcc_ap_bw40_target_tp(adapter, buffer, count);
}

long rtw_rust_debug_proc_set_mcc_ap_bw80_target_tp(void *adapter,
						   const void *buffer,
						   size_t count)
{
	return host_proc_set_mcc_ap_bw80_target_tp(adapter, buffer, count);
}

long rtw_rust_debug_proc_set_mcc_sta_bw20_target_tp(void *adapter,
						    const void *buffer,
						    size_t count)
{
	return host_proc_set_mcc_sta_bw20_target_tp(adapter, buffer, count);
}

long rtw_rust_debug_proc_set_mcc_sta_bw40_target_tp(void *adapter,
						    const void *buffer,
						    size_t count)
{
	return host_proc_set_mcc_sta_bw40_target_tp(adapter, buffer, count);
}

long rtw_rust_debug_proc_set_mcc_sta_bw80_target_tp(void *adapter,
						    const void *buffer,
						    size_t count)
{
	return host_proc_set_mcc_sta_bw80_target_tp(adapter, buffer, count);
}
