// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-216 proc rf4ce_state + sreset. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_sreset_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 32
#define MAX_FIELD 32
#define MAX_EXPECT 512

struct vector {
	char name[MAX_NAME];
	u8 rf4ce_state;
	u8 dbg_sreset_ctrl;
	u64 self_dect_tx_cnt;
	u64 self_dect_rx_cnt;
	u64 self_dect_fw_cnt;
	u64 tx_dma_status_cnt;
	u64 rx_dma_status_cnt;
	u8 self_dect_case;
	u32 dbg_sreset_cnt;
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	char get_field[MAX_FIELD];
	char expect[MAX_EXPECT];
	int check_trigger;
	int check_reset_calls;
};

static void unescape_expect(char *s)
{
	char *r = s;
	char *w = s;

	while (*r) {
		if (r[0] == '\\' && r[1] == 'n') {
			*w++ = '\n';
			r += 2;
		} else {
			*w++ = *r++;
		}
	}
	*w = '\0';
}

static int parse_u8_field(const char *obj, size_t len, const char *key, u8 *out)
{
	int tmp;

	if (!host_json_find_key_in(obj, len, key))
		return 0;
	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u8)tmp;
	return 0;
}

static int parse_u64_field(const char *obj, size_t len, const char *key, u64 *out)
{
	int tmp;

	if (!host_json_find_key_in(obj, len, key))
		return 0;
	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (u64)tmp;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8_field(obj, len, "rf4ce_state", &v->rf4ce_state))
		return -1;
	if (parse_u8_field(obj, len, "dbg_sreset_ctrl", &v->dbg_sreset_ctrl))
		return -1;
	if (parse_u64_field(obj, len, "self_dect_tx_cnt", &v->self_dect_tx_cnt))
		return -1;
	if (parse_u64_field(obj, len, "self_dect_rx_cnt", &v->self_dect_rx_cnt))
		return -1;
	if (parse_u64_field(obj, len, "self_dect_fw_cnt", &v->self_dect_fw_cnt))
		return -1;
	if (parse_u64_field(obj, len, "tx_dma_status_cnt", &v->tx_dma_status_cnt))
		return -1;
	if (parse_u64_field(obj, len, "rx_dma_status_cnt", &v->rx_dma_status_cnt))
		return -1;
	if (parse_u8_field(obj, len, "self_dect_case", &v->self_dect_case))
		return -1;
	if (host_json_find_key_in(obj, len, "dbg_sreset_cnt")) {
		if (host_json_parse_int_in(obj, len, "dbg_sreset_cnt", &tmp))
			return -1;
		v->dbg_sreset_cnt = (u32)tmp;
	}
	if (host_json_find_key_in(obj, len, "check_trigger")) {
		if (host_json_parse_int_in(obj, len, "check_trigger", &tmp))
			return -1;
		v->check_trigger = tmp;
	}
	if (host_json_find_key_in(obj, len, "check_reset_calls")) {
		if (host_json_parse_int_in(obj, len, "check_reset_calls", &tmp))
			return -1;
		v->check_reset_calls = tmp;
	}
	if (host_json_find_key_in(obj, len, "set")) {
		if (host_json_parse_string_in(obj, len, "set", v->set_field,
					      sizeof(v->set_field)))
			return -1;
		if (host_json_parse_string_in(obj, len, "set_input", v->set_input,
					      sizeof(v->set_input)))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "get")) {
		if (host_json_parse_string_in(obj, len, "get", v->get_field,
					      sizeof(v->get_field)))
			return -1;
	}
	if (host_json_find_key_in(obj, len, "expect")) {
		if (host_json_parse_string_in(obj, len, "expect", v->expect,
					      sizeof(v->expect)))
			return -1;
		unescape_expect(v->expect);
	}
	return 0;
}

static void apply_fixture(const struct vector *v, struct _adapter *adapter,
			  struct dvobj_priv *dvobj)
{
	struct hal_data *hal = GET_HAL_DATA(adapter);
	struct sreset_priv *sreset = &hal->srestpriv;

	adapter->dvobj = dvobj;
	host_debug_proc_sreset_set_rf4ce(v->rf4ce_state);
	sreset->dbg_sreset_ctrl = v->dbg_sreset_ctrl ? _TRUE : _FALSE;
	sreset->self_dect_tx_cnt = v->self_dect_tx_cnt;
	sreset->self_dect_rx_cnt = v->self_dect_rx_cnt;
	sreset->self_dect_fw_cnt = v->self_dect_fw_cnt;
	sreset->tx_dma_status_cnt = v->tx_dma_status_cnt;
	sreset->rx_dma_status_cnt = v->rx_dma_status_cnt;
	sreset->self_dect_case = v->self_dect_case;
	dvobj->drv_dbg.dbg_sreset_cnt = v->dbg_sreset_cnt;
}

static int dispatch_set(const struct vector *v, struct net_device *dev)
{
	if (!strcmp(v->set_field, "rf4ce_state")) {
		return proc_set_rf4ce_state(NULL, v->set_input, strlen(v->set_input),
					    NULL, dev) < 0;
	}
	if (!strcmp(v->set_field, "sreset")) {
		return proc_set_sreset(NULL, v->set_input, strlen(v->set_input),
				       NULL, dev) < 0;
	}
	return 1;
}

static int dispatch_get(const struct vector *v, struct seq_file *seq)
{
	if (!strcmp(v->get_field, "rf4ce_state"))
		return proc_get_rf4ce_state(seq, NULL);
	if (!strcmp(v->get_field, "sreset"))
		return proc_get_sreset(seq, NULL);
	return 1;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct dvobj_priv dvobj;
	struct net_device dev;
	struct seq_file seq;
	char got[MAX_EXPECT];
	FILE *capture;
	size_t nread;

	host_debug_proc_sreset_reset_mocks();
	memset(&adapter, 0, sizeof(adapter));
	memset(&dvobj, 0, sizeof(dvobj));
	apply_fixture(v, &adapter, &dvobj);
	dev.priv = &adapter;
	seq.private = &dev;

	if (v->set_field[0] && dispatch_set(v, &dev))
		return -1;

	if (v->check_trigger) {
		if (host_debug_proc_sreset_last_trigger() != SRESET_TGP_XMIT_STATUS) {
			fprintf(stderr, "FAIL %s: trigger_point=%d expect %d\n",
				v->name, (int)host_debug_proc_sreset_last_trigger(),
				SRESET_TGP_XMIT_STATUS);
			return -1;
		}
		return 0;
	}

	if (v->check_reset_calls) {
		if (host_debug_proc_sreset_reset_calls() != v->check_reset_calls) {
			fprintf(stderr, "FAIL %s: reset_calls=%d expect %d\n",
				v->name, host_debug_proc_sreset_reset_calls(),
				v->check_reset_calls);
			return -1;
		}
		return 0;
	}

	if (!v->get_field[0])
		return 0;

	capture = tmpfile();
	if (!capture)
		return -1;
	seq.out = capture;

	if (dispatch_get(v, &seq)) {
		fclose(capture);
		return -1;
	}

	rewind(capture);
	nread = fread(got, 1, sizeof(got) - 1, capture);
	fclose(capture);
	got[nread] = '\0';

	if (strcmp(got, v->expect) != 0) {
		fprintf(stderr, "FAIL %s:\n  got:    %s\n  expect: %s\n", v->name,
			got, v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "debug_proc_sreset_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			return 1;
	}

	printf("ok: %zu proc sreset/rf4ce vectors\n", count);
	return 0;
}
