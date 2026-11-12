// SPDX-License-Identifier: GPL-2.0
/* Host L2 rpwm must keep full-width systime through cpwm_polling_core (FX-01 / T24). */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define _SUCCESS 1
#define PS_TOGGLE (1U << 7)

typedef uintptr_t PADAPTER;
typedef unsigned char u8;
typedef unsigned int u32;

void host_pwrctrl_rpwm_reset_hal(void);
void host_pwrctrl_rpwm_set_time(unsigned long t);
void host_pwrctrl_rpwm_set_cpwm_behavior(u8 cpwm_orig, u32 poll_reads_before_toggle);
u8 rtw_cpwm_polling(PADAPTER adapter, u8 rpwm, u8 cpwm_orig_in);

#if ULONG_MAX <= 0xffffffffUL
int main(void)
{
	return 0;
}
#else
int main(void)
{
	char adapter[64];
	int rc = 0;

	memset(adapter, 0, sizeof(adapter));
	host_pwrctrl_rpwm_reset_hal();
	host_pwrctrl_rpwm_set_time((1UL << 32) + 0x10UL);
	host_pwrctrl_rpwm_set_cpwm_behavior(0, 1);
	if (rtw_cpwm_polling((PADAPTER)adapter, 0, 0) != _SUCCESS) {
		fprintf(stderr, "rpwm host: cpwm_polling failed past 2^32 jiffies\n");
		rc = 1;
	}
	return rc;
}
#endif
