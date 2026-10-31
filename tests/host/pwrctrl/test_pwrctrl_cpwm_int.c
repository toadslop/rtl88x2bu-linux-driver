// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

typedef unsigned char u8;
typedef unsigned int u32;

struct reportpwrstate_parm {
	unsigned char mode;
	unsigned char state;
	unsigned short rsvd;
};

void host_pwrctrl_cpwm_int_reset(void);
void host_pwrctrl_cpwm_int_set_time(unsigned int t);
u8 host_pwrctrl_cpwm_int_get_cpwm(void *);
u8 host_pwrctrl_cpwm_int_get_cpwm_tog(void *);
unsigned int host_pwrctrl_cpwm_int_cmd_sem_up(void *);
unsigned int host_pwrctrl_cpwm_int_xmit_sem_up(void *);
void LPS_Leave_check(void *padapter);
void cpwm_int_hdl(void *padapter, struct reportpwrstate_parm *report);

struct adapter_blob {
	int lock;
	u8 pwr_mode;
	u8 cpwm;
	u8 cpwm_tog;
	u8 rpwm;
	u8 alives;
	u8 _pad[3];
	u8 surprise_removed;
	u8 hw_init;
	u8 driver_stopped;
	u8 _pad2;
	u32 cmd_sem;
	u32 xmit_sem;
};

struct vector {
	char name[MAX_NAME];
	int null_adapter;
	int leave_only;
	int state;
	int has_state;
	int alives;
	int has_alives;
	int pwr_mode;
	int has_pwr_mode;
	int surprise_removed;
	int hw_init;
	int driver_stopped;
	int start_time;
	int advance_ms;
	int expect_cpwm;
	int has_expect_cpwm;
	int expect_cpwm_tog;
	int has_expect_cpwm_tog;
	int expect_cmd_sem;
	int has_expect_cmd_sem;
	int expect_xmit_sem;
	int has_expect_xmit_sem;
};

static char g_adapter[128];

static struct adapter_blob *blob(void)
{
	return (struct adapter_blob *)g_adapter;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "null_adapter", &v->null_adapter);
	host_json_parse_int_in(obj, len, "leave_only", &v->leave_only);
	if (host_json_parse_int_in(obj, len, "state", &v->state) == 0)
		v->has_state = 1;
	if (host_json_parse_int_in(obj, len, "alives", &v->alives) == 0)
		v->has_alives = 1;
	if (host_json_parse_int_in(obj, len, "pwr_mode", &v->pwr_mode) == 0)
		v->has_pwr_mode = 1;
	host_json_parse_int_in(obj, len, "surprise_removed", &v->surprise_removed);
	host_json_parse_int_in(obj, len, "hw_init", &v->hw_init);
	host_json_parse_int_in(obj, len, "driver_stopped", &v->driver_stopped);
	host_json_parse_int_in(obj, len, "start_time", &v->start_time);
	host_json_parse_int_in(obj, len, "advance_ms", &v->advance_ms);
	if (host_json_parse_int_in(obj, len, "expect_cpwm", &v->expect_cpwm) == 0)
		v->has_expect_cpwm = 1;
	if (host_json_parse_int_in(obj, len, "expect_cpwm_tog", &v->expect_cpwm_tog) == 0)
		v->has_expect_cpwm_tog = 1;
	if (host_json_parse_int_in(obj, len, "expect_cmd_sem", &v->expect_cmd_sem) == 0)
		v->has_expect_cmd_sem = 1;
	if (host_json_parse_int_in(obj, len, "expect_xmit_sem", &v->expect_xmit_sem) == 0)
		v->has_expect_xmit_sem = 1;
	return 0;
}

static void setup_adapter(struct vector *v)
{
	struct adapter_blob *b = blob();

	memset(g_adapter, 0, sizeof(g_adapter));
	b->lock = 1;
	if (v->has_pwr_mode)
		b->pwr_mode = (u8)v->pwr_mode;
	else
		b->pwr_mode = 2;
	b->hw_init = v->hw_init ? 1 : 1;
	b->surprise_removed = (u8)v->surprise_removed;
	b->driver_stopped = (u8)v->driver_stopped;
	if (v->has_alives)
		b->alives = (u8)v->alives;
}

static int run_vec(struct vector *v)
{
	struct reportpwrstate_parm report;
	void *adapter;

	host_pwrctrl_cpwm_int_reset();
	host_pwrctrl_cpwm_int_set_time((unsigned int)v->start_time);
	setup_adapter(v);

	if (v->leave_only) {
		if (v->advance_ms)
			host_pwrctrl_cpwm_int_set_time((unsigned int)(v->start_time + v->advance_ms));
		LPS_Leave_check(g_adapter);
		printf("PASS %s\n", v->name);
		return 0;
	}

	adapter = v->null_adapter ? NULL : g_adapter;
	memset(&report, 0, sizeof(report));
	if (v->has_state)
		report.state = (u8)v->state;

	cpwm_int_hdl(adapter, &report);

	if (v->has_expect_cpwm &&
	    host_pwrctrl_cpwm_int_get_cpwm(g_adapter) != (u8)v->expect_cpwm) {
		fprintf(stderr, "%s: cpwm mismatch\n", v->name);
		return -1;
	}
	if (v->has_expect_cpwm_tog &&
	    host_pwrctrl_cpwm_int_get_cpwm_tog(g_adapter) != (u8)v->expect_cpwm_tog) {
		fprintf(stderr, "%s: cpwm_tog mismatch\n", v->name);
		return -1;
	}
	if (v->has_expect_cmd_sem &&
	    host_pwrctrl_cpwm_int_cmd_sem_up(g_adapter) != (unsigned int)v->expect_cmd_sem) {
		fprintf(stderr, "%s: cmd sem mismatch\n", v->name);
		return -1;
	}
	if (v->has_expect_xmit_sem &&
	    host_pwrctrl_cpwm_int_xmit_sem_up(g_adapter) != (unsigned int)v->expect_xmit_sem) {
		fprintf(stderr, "%s: xmit sem mismatch\n", v->name);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "cpwm_int_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
