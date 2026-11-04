// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-212 proc_tdls_display_tdls_function_info (host L2 PR3). */

#include "host_rtw_debug_proc_tdls_types.h"

int proc_tdls_display_tdls_function_info(struct seq_file *m)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = (struct _adapter *)rtw_netdev_priv(dev);
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	u8 SpaceBtwnItemAndValue = TDLS_DBG_INFO_SPACE_BTWN_ITEM_AND_VALUE;

	RTW_PRINT_SEL(m, "============[TDLS Function Info]============\n");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Enable",
		      (rtw_is_tdls_enabled(padapter) == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Driver Setup",
		      (ptdlsinfo->driver_setup == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Prohibited",
		      (ptdlsinfo->ap_prohibited == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue,
		      "TDLS Channel Switch Prohibited",
		      (ptdlsinfo->ch_switch_prohibited == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Link Established",
		      (ptdlsinfo->link_established == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %d/%d\n", SpaceBtwnItemAndValue,
		      "TDLS STA Num (Linked/Allowed)", ptdlsinfo->sta_cnt,
		      MAX_ALLOWED_TDLS_STA_NUM);
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue,
		      "TDLS Allowed STA Num Reached",
		      (ptdlsinfo->sta_maximum == _TRUE) ? "_TRUE" : "_FALSE");
	RTW_PRINT_SEL(m, "%-*s = %s\n", SpaceBtwnItemAndValue, "TDLS Device Discovered",
		      (ptdlsinfo->dev_discovered == _TRUE) ? "_TRUE" : "_FALSE");

	return 0;
}
