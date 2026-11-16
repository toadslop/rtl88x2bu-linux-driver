#include "host_hal_dm_get_odm_var_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}

#ifdef CONFIG_ANTENNA_DIVERSITY
u8 rtw_rust_hal_dm_fat_rx_idle_ant(struct dm_struct *phydm)
{
	if (!phydm)
		return 0;
	return phydm->dm_fat_table.rx_idle_ant;
}
#endif