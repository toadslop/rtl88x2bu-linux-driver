// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-206 tx AMPDU misc procs (from core/rtw_debug_rest.c). */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_ampdu_types.h"

struct host_proc_seq {
	struct _adapter *adapter;
	FILE *out;
};

int proc_get_tx_quick_addba_req(struct seq_file *m, void *v)
{
	struct host_proc_seq *ctx = (struct host_proc_seq *)m;
	struct _adapter *padapter = ctx->adapter;
	struct registry_priv *pregpriv = &padapter->registrypriv;

	if (padapter)
		RTW_PRINT_SEL(ctx->out, "tx_quick_addba_req = %x\n",
			      pregpriv->tx_quick_addba_req);

	return 0;
}

long proc_set_tx_quick_addba_req(void *file, const void *buffer, unsigned long count,
				 long *pos, void *data)
{
	struct _adapter *padapter = data;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	char tmp[32];
	unsigned int enable;

	(void)file;
	(void)pos;

	if (count < 1)
		return -14;

	if (count > sizeof(tmp))
		return -14;

	if (buffer && count <= sizeof(tmp)) {
		memcpy(tmp, buffer, count);
		tmp[count] = '\0';

		if (sscanf(tmp, "%u ", &enable) == 1 && padapter)
			pregpriv->tx_quick_addba_req = (u8)enable;
	}

	return (long)count;
}

int proc_get_tx_aval_th(struct seq_file *m, void *v)
{
	struct host_proc_seq *ctx = (struct host_proc_seq *)m;
	struct _adapter *padapter = ctx->adapter;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	(void)v;

	if (padapter) {
		switch (dvobj->tx_aval_int_thr_mode) {
		case 0:
			RTW_PRINT_SEL(ctx->out,
				      "tx_aval_int_thr_mode = %u (auto) \n",
				      dvobj->tx_aval_int_thr_mode);
			break;
		case 1:
			RTW_PRINT_SEL(ctx->out,
				      "tx_aval_int_thr_mode = %u (fixed)\n",
				      dvobj->tx_aval_int_thr_mode);
			RTW_PRINT_SEL(ctx->out, "tx_aval_threshold = 0x%x\n",
				      dvobj->tx_aval_int_thr_value);
			break;
		case 2:
			RTW_PRINT_SEL(ctx->out,
				      "tx_aval_int_thr_mode = %u(by sdio_tx_max_len)\n",
				      dvobj->tx_aval_int_thr_mode);
			break;
		default:
			break;
		}
	}
	return 0;
}

long proc_set_tx_aval_th(void *file, const void *buffer, unsigned long count,
			 long *pos, void *data)
{
	struct _adapter *padapter = data;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	char tmp[32];
	unsigned int mode;
	unsigned int threshold;
	int num;

	(void)file;
	(void)pos;

	if (count < 1)
		return -14;

	if (count > sizeof(tmp))
		return -14;

	if (buffer && count <= sizeof(tmp)) {
		memcpy(tmp, buffer, count);
		tmp[count] = '\0';

		num = sscanf(tmp, "%u %u ", &mode, &threshold);
		if (num >= 1)
			dvobj->tx_aval_int_thr_mode = (u8)mode;
		if (num >= 2)
			dvobj->tx_aval_int_thr_value = (u8)threshold;
	}

	return (long)count;
}
