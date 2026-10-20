// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbol for W3-144 L1 (dump_adapters_status). */

#include <stddef.h>

typedef struct dvobj_priv dvobj_priv;

void dump_adapters_status(void *sel, struct dvobj_priv *dvobj)
{
	(void)sel;
	(void)dvobj;
}
