// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-03 dump_chip_info formatter (from hal/hal_com_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_hal_com_chip_info_types.h"

int dump_chip_info_format(HAL_VERSION ChipVersion, char *buf, size_t buflen)
{
	int cnt = 0;

	if (!buf || buflen == 0)
		return -1;

	if (IS_8188E(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8188E_");
	else if (IS_8188F(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8188F_");
	else if (IS_8188GTV(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8188GTV_");
	else if (IS_8812_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8812_");
	else if (IS_8192E(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8192E_");
	else if (IS_8821_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8821_");
	else if (IS_8723B_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8723B_");
	else if (IS_8703B_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8703B_");
	else if (IS_8723D_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8723D_");
	else if (IS_8814A_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8814A_");
	else if (IS_8822B_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8822B_");
	else if (IS_8821C_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8821C_");
	else if (IS_8710B_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8710B_");
	else if (IS_8192F_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8192F_");
	else if (IS_8822C_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8822C_");
	else if (IS_8814B_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8814B_");
	else if (IS_8723F_SERIES(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_8723F_");
	else
		cnt += snprintf(buf + cnt, buflen - cnt,
				"Chip Version Info: CHIP_UNKNOWN_");

	if ((size_t)cnt >= buflen)
		return -1;

	cnt += snprintf(buf + cnt, buflen - cnt, "%s",
			IS_NORMAL_CHIP(ChipVersion) ? "" : "T_");
	if ((size_t)cnt >= buflen)
		return -1;

	if (IS_CHIP_VENDOR_TSMC(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "%s", "T");
	else if (IS_CHIP_VENDOR_UMC(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "%s", "U");
	else if (IS_CHIP_VENDOR_SMIC(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "%s", "S");

	if ((size_t)cnt >= buflen)
		return -1;

	if (IS_A_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "1_");
	else if (IS_B_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "2_");
	else if (IS_C_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "3_");
	else if (IS_D_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "4_");
	else if (IS_E_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "5_");
	else if (IS_F_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "6_");
	else if (IS_I_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "9_");
	else if (IS_J_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "10_");
	else if (IS_K_CUT(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "11_");
	else
		cnt += snprintf(buf + cnt, buflen - cnt, "UNKNOWN_Cv(%d)_",
				ChipVersion.CUTVersion);

	if ((size_t)cnt >= buflen)
		return -1;

	if (IS_1T1R(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "1T1R_");
	else if (IS_1T2R(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "1T2R_");
	else if (IS_2T2R(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "2T2R_");
	else if (IS_3T3R(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "3T3R_");
	else if (IS_3T4R(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "3T4R_");
	else if (IS_4T4R(ChipVersion))
		cnt += snprintf(buf + cnt, buflen - cnt, "4T4R_");
	else
		cnt += snprintf(buf + cnt, buflen - cnt, "UNKNOWN_RFTYPE(%d)_",
				ChipVersion.RFType);

	if ((size_t)cnt >= buflen)
		return -1;

	cnt += snprintf(buf + cnt, buflen - cnt, "RomVer(%d)\n",
			ChipVersion.ROMVer);
	if ((size_t)cnt >= buflen)
		return -1;

	return cnt;
}
