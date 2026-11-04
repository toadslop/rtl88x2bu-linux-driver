/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-209 proc en_fwps (check_fw_ps) get/set. */
#ifndef HOST_RTW_DEBUG_PROC_FWPS_TYPES_H
#define HOST_RTW_DEBUG_PROC_FWPS_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

struct registry_priv {
	u8 check_fw_ps;
};

struct _adapter {
	struct registry_priv registrypriv;
};

struct net_device {
	void *priv;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

ssize_t rtw_rust_debug_proc_copy_user_input(const void *buffer, size_t count,
					    char *tmp, size_t tmp_size);

void *rtw_rust_debug_proc_adapter(void *m);
void *rtw_rust_debug_proc_adapter_from_data(void *data);

void rtw_rust_debug_print_sel(void *sel, const char *line);

void rtw_rust_debug_proc_emit_en_fwps(void *sel, void *adapter);
long rtw_rust_debug_proc_set_en_fwps(void *adapter, const void *buffer,
				     size_t count);

int proc_get_en_fwps(struct seq_file *m, void *v);
ssize_t proc_set_en_fwps(void *file, const void *buffer, size_t count,
			 long *pos, void *data);

#endif /* HOST_RTW_DEBUG_PROC_FWPS_TYPES_H */
