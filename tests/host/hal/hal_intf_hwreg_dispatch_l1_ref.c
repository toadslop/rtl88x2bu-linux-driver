// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: hwreg/def/odm dispatch + notch (W4-73). */

typedef unsigned char u8;
typedef int HAL_DEF_VARIABLE;
typedef int HAL_ODM_VARIABLE;
typedef unsigned char BOOLEAN;
typedef _Bool bool;
typedef struct _adapter _adapter;

u8 rtw_hal_set_hwreg(_adapter *padapter, u8 variable, u8 *val)
{
	(void)padapter;
	(void)variable;
	(void)val;
	return 0;
}

void rtw_hal_get_hwreg(_adapter *padapter, u8 variable, u8 *val)
{
	(void)padapter;
	(void)variable;
	(void)val;
}

u8 rtw_hal_set_def_var(_adapter *padapter, HAL_DEF_VARIABLE eVariable, void *pValue)
{
	(void)padapter;
	(void)eVariable;
	(void)pValue;
	return 0;
}

u8 rtw_hal_get_def_var(_adapter *padapter, HAL_DEF_VARIABLE eVariable, void *pValue)
{
	(void)padapter;
	(void)eVariable;
	(void)pValue;
	return 0;
}

void rtw_hal_set_odm_var(_adapter *padapter, HAL_ODM_VARIABLE eVariable, void *pValue1,
			 BOOLEAN bSet)
{
	(void)padapter;
	(void)eVariable;
	(void)pValue1;
	(void)bSet;
}

void rtw_hal_get_odm_var(_adapter *padapter, HAL_ODM_VARIABLE eVariable, void *pValue1,
			 void *pValue2)
{
	(void)padapter;
	(void)eVariable;
	(void)pValue1;
	(void)pValue2;
}

void rtw_hal_notch_filter(_adapter *adapter, bool enable)
{
	(void)adapter;
	(void)enable;
}