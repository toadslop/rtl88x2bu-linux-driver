// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-222 MCC proc registry setters. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_proc_mcc_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_INPUT 64
#define MAX_FIELD 32

ssize_t proc_set_mcc_enable(void *file, const void *buffer, size_t count,
			    long *pos, void *data);
ssize_t proc_set_mcc_duration(void *file, const void *buffer, size_t count,
			      long *pos, void *data);
#ifdef CONFIG_MCC_PHYDM_OFFLOAD
ssize_t proc_set_mcc_phydm_offload_enable(void *file, const void *buffer,
					  size_t count, long *pos, void *data);
#endif
ssize_t proc_set_mcc_single_tx_criteria(void *file, const void *buffer,
					size_t count, long *pos, void *data);
ssize_t proc_set_mcc_ap_bw20_target_tp(void *file, const void *buffer,
				     size_t count, long *pos, void *data);
ssize_t proc_set_mcc_ap_bw40_target_tp(void *file, const void *buffer,
				     size_t count, long *pos, void *data);
ssize_t proc_set_mcc_ap_bw80_target_tp(void *file, const void *buffer,
				     size_t count, long *pos, void *data);
ssize_t proc_set_mcc_sta_bw20_target_tp(void *file, const void *buffer,
				      size_t count, long *pos, void *data);
ssize_t proc_set_mcc_sta_bw40_target_tp(void *file, const void *buffer,
				      size_t count, long *pos, void *data);
ssize_t proc_set_mcc_sta_bw80_target_tp(void *file, const void *buffer,
				      size_t count, long *pos, void *data);

struct vector {
	char name[MAX_NAME];
	u8 iface_nums;
	u8 iface_null_mask;
	u8 expect_en_mcc[MAX_IFACE];
	u8 expect_runtime_duration;
	int expect_duration_cmd;
	u8 expect_duration_type;
	u8 expect_duration_val;
	int expect_phydm_cmd;
	u8 expect_phydm_enable;
	u32 expect_single_tx[MAX_IFACE];
	u32 expect_target_tp;
	int has_expect_target_tp;
	char expect_tp_field[MAX_FIELD];
	char set_field[MAX_FIELD];
	char set_input[MAX_INPUT];
	int set_expect_ret;
	int has_set_expect_ret;
};

static int parse_u8_array(const char *obj, size_t len, const char *key,
			  u8 *out, int max)
{
	char search[64];
	int idx = 0;

	while (idx < max) {
		snprintf(search, sizeof(search), "%s_%d", key, idx);
		if (!host_json_find_key_in(obj, len, search))
			break;
		{
			int tmp;

			if (host_json_parse_int_in(obj, len, search, &tmp))
				return -1;
			out[idx] = (u8)tmp;
		}
		idx++;
	}
	return 0;
}

static int parse_u32_array(const char *obj, size_t len, const char *key,
			   u32 *out, int max)
{
	char search[64];
	int idx = 0;

	while (idx < max) {
		snprintf(search, sizeof(search), "%s_%d", key, idx);
		if (!host_json_find_key_in(obj, len, search))
			break;
		{
			int tmp;

			if (host_json_parse_int_in(obj, len, search, &tmp))
				return -1;
			out[idx] = (u32)tmp;
		}
		idx++;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	memset(v->expect_en_mcc, 0xFF, sizeof(v->expect_en_mcc));

	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	{
		int tmp;

		if (host_json_parse_int_in(obj, len, "iface_nums", &tmp))
			return -1;
		v->iface_nums = (u8)tmp;
	}
	if (host_json_find_key_in(obj, len, "iface_null_mask")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "iface_null_mask", &tmp))
			return -1;
		v->iface_null_mask = (u8)tmp;
	}
	parse_u8_array(obj, len, "expect_en_mcc", v->expect_en_mcc, MAX_IFACE);
	parse_u32_array(obj, len, "expect_single_tx", v->expect_single_tx,
			MAX_IFACE);
	{
		int tmp;

		if (host_json_find_key_in(obj, len, "expect_runtime_duration")) {
			if (host_json_parse_int_in(obj, len,
						   "expect_runtime_duration",
						   &tmp))
				return -1;
			v->expect_runtime_duration = (u8)tmp;
		}
		if (host_json_find_key_in(obj, len, "expect_duration_cmd")) {
			if (host_json_parse_int_in(obj, len, "expect_duration_cmd",
						   &tmp))
				return -1;
			v->expect_duration_cmd = tmp;
		}
		if (host_json_find_key_in(obj, len, "expect_duration_type")) {
			if (host_json_parse_int_in(obj, len,
						   "expect_duration_type",
						   &tmp))
				return -1;
			v->expect_duration_type = (u8)tmp;
		}
		if (host_json_find_key_in(obj, len, "expect_duration_val")) {
			if (host_json_parse_int_in(obj, len, "expect_duration_val",
						   &tmp))
				return -1;
			v->expect_duration_val = (u8)tmp;
		}
		if (host_json_find_key_in(obj, len, "expect_phydm_cmd")) {
			if (host_json_parse_int_in(obj, len, "expect_phydm_cmd",
						   &tmp))
				return -1;
			v->expect_phydm_cmd = tmp;
		}
		if (host_json_find_key_in(obj, len, "expect_phydm_enable")) {
			if (host_json_parse_int_in(obj, len,
						   "expect_phydm_enable", &tmp))
				return -1;
			v->expect_phydm_enable = (u8)tmp;
		}
	}
	if (host_json_parse_string_in(obj, len, "set", v->set_field,
				      sizeof(v->set_field)))
		return -1;
	if (host_json_parse_string_in(obj, len, "set_input", v->set_input,
				      sizeof(v->set_input)))
		return -1;
	if (host_json_find_key_in(obj, len, "set_expect_ret")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "set_expect_ret", &tmp))
			return -1;
		v->set_expect_ret = tmp;
		v->has_set_expect_ret = 1;
	}
	if (host_json_find_key_in(obj, len, "expect_target_tp")) {
		int tmp;

		if (host_json_parse_int_in(obj, len, "expect_target_tp", &tmp))
			return -1;
		v->expect_target_tp = (u32)tmp;
		v->has_expect_target_tp = 1;
	}
	return 0;
}

static void setup_fixture(const struct vector *v, struct dvobj_priv *dvobj,
			  struct _adapter adapters[MAX_IFACE],
			  struct net_device *dev)
{
	u8 i;

	memset(dvobj, 0, sizeof(*dvobj));
	memset(adapters, 0, sizeof(adapters[0]) * MAX_IFACE);
	dvobj->iface_nums = v->iface_nums;
	for (i = 0; i < v->iface_nums && i < MAX_IFACE; i++) {
		if (v->iface_null_mask & (1U << i)) {
			dvobj->padapters[i] = NULL;
			continue;
		}
		adapters[i].dvobj = dvobj;
		dvobj->padapters[i] = &adapters[i];
	}
	adapters[0].dvobj = dvobj;
	dev->priv = &adapters[0];
}

static ssize_t invoke_set(const struct vector *v, const char *input,
			  struct net_device *dev)
{
	/* Match /proc write count: include the terminating NUL the driver would see. */
	size_t count = strlen(input) + 1;
	long pos = 0;

	if (!strcmp(v->set_field, "mcc_enable"))
		return proc_set_mcc_enable(NULL, input, count, &pos, dev);
	if (!strcmp(v->set_field, "mcc_duration"))
		return proc_set_mcc_duration(NULL, input, count, &pos, dev);
#ifdef CONFIG_MCC_PHYDM_OFFLOAD
	if (!strcmp(v->set_field, "mcc_phydm_offload"))
		return proc_set_mcc_phydm_offload_enable(NULL, input, count, &pos,
							 dev);
#endif
	if (!strcmp(v->set_field, "mcc_single_tx"))
		return proc_set_mcc_single_tx_criteria(NULL, input, count, &pos,
						      dev);
	if (!strcmp(v->set_field, "mcc_ap_bw20_tp"))
		return proc_set_mcc_ap_bw20_target_tp(NULL, input, count, &pos,
						      dev);
	if (!strcmp(v->set_field, "mcc_ap_bw40_tp"))
		return proc_set_mcc_ap_bw40_target_tp(NULL, input, count, &pos,
						      dev);
	if (!strcmp(v->set_field, "mcc_ap_bw80_tp"))
		return proc_set_mcc_ap_bw80_target_tp(NULL, input, count, &pos,
						      dev);
	if (!strcmp(v->set_field, "mcc_sta_bw20_tp"))
		return proc_set_mcc_sta_bw20_target_tp(NULL, input, count, &pos,
						       dev);
	if (!strcmp(v->set_field, "mcc_sta_bw40_tp"))
		return proc_set_mcc_sta_bw40_target_tp(NULL, input, count, &pos,
						       dev);
	if (!strcmp(v->set_field, "mcc_sta_bw80_tp"))
		return proc_set_mcc_sta_bw80_target_tp(NULL, input, count, &pos,
						       dev);
	return -999;
}

static u32 *target_tp_field(struct _adapter *padapter, const char *set_field)
{
	if (!strcmp(set_field, "mcc_ap_bw20_tp"))
		return &padapter->registrypriv.rtw_mcc_ap_bw20_target_tx_tp;
	if (!strcmp(set_field, "mcc_ap_bw40_tp"))
		return &padapter->registrypriv.rtw_mcc_ap_bw40_target_tx_tp;
	if (!strcmp(set_field, "mcc_ap_bw80_tp"))
		return &padapter->registrypriv.rtw_mcc_ap_bw80_target_tx_tp;
	if (!strcmp(set_field, "mcc_sta_bw20_tp"))
		return &padapter->registrypriv.rtw_mcc_sta_bw20_target_tx_tp;
	if (!strcmp(set_field, "mcc_sta_bw40_tp"))
		return &padapter->registrypriv.rtw_mcc_sta_bw40_target_tx_tp;
	if (!strcmp(set_field, "mcc_sta_bw80_tp"))
		return &padapter->registrypriv.rtw_mcc_sta_bw80_target_tx_tp;
	return NULL;
}

static int check_vector(const struct vector *v)
{
	struct dvobj_priv dvobj;
	struct _adapter adapters[MAX_IFACE];
	struct net_device dev;
	ssize_t ret;
	const struct host_mcc_cmd_log *log;
	u8 i;

	host_debug_proc_mcc_reset_log();
	setup_fixture(v, &dvobj, adapters, &dev);

	ret = invoke_set(v, v->set_input, &dev);
	if (v->has_set_expect_ret) {
		if (ret != v->set_expect_ret) {
			fprintf(stderr, "%s: ret %zd != expect %d\n", v->name,
				ret, v->set_expect_ret);
			return -1;
		}
		return 0;
	}

	if (ret < 0) {
		fprintf(stderr, "%s: unexpected negative ret %zd\n", v->name,
			ret);
		return -1;
	}

	if (!strcmp(v->set_field, "mcc_enable")) {
		for (i = 0; i < v->iface_nums && i < MAX_IFACE; i++) {
			if (v->expect_en_mcc[i] == 0xFF)
				continue;
			if (!dvobj.padapters[i])
				continue;
			if (dvobj.padapters[i]->registrypriv.en_mcc !=
			    v->expect_en_mcc[i]) {
				fprintf(stderr,
					"%s: iface %u en_mcc %u != %u\n",
					v->name, i,
					dvobj.padapters[i]->registrypriv.en_mcc,
					v->expect_en_mcc[i]);
				return -1;
			}
		}
	}

	log = host_debug_proc_mcc_get_log();
	if (!strcmp(v->set_field, "mcc_duration")) {
		if (dvobj.mcc_objpriv.enable_runtime_duration !=
		    v->expect_runtime_duration) {
			fprintf(stderr, "%s: runtime_duration %u != %u\n",
				v->name,
				dvobj.mcc_objpriv.enable_runtime_duration,
				v->expect_runtime_duration);
			return -1;
		}
		if (log->duration_cmd != (unsigned)v->expect_duration_cmd) {
			fprintf(stderr, "%s: duration_cmd %u != %d\n", v->name,
				log->duration_cmd, v->expect_duration_cmd);
			return -1;
		}
		if (v->expect_duration_type &&
		    log->last_type != v->expect_duration_type) {
			fprintf(stderr, "%s: duration type %u != %u\n", v->name,
				log->last_type, v->expect_duration_type);
			return -1;
		}
		if (v->expect_duration_val &&
		    log->last_val != v->expect_duration_val) {
			fprintf(stderr, "%s: duration val %u != %u\n", v->name,
				log->last_val, v->expect_duration_val);
			return -1;
		}
	}

	if (!strcmp(v->set_field, "mcc_phydm_offload")) {
		if (log->phydm_cmd != (unsigned)v->expect_phydm_cmd) {
			fprintf(stderr, "%s: phydm_cmd %u != %d\n", v->name,
				log->phydm_cmd, v->expect_phydm_cmd);
			return -1;
		}
		if (log->last_phydm_enable != v->expect_phydm_enable) {
			fprintf(stderr, "%s: phydm enable %u != %u\n", v->name,
				log->last_phydm_enable,
				v->expect_phydm_enable);
			return -1;
		}
	}

	if (!strcmp(v->set_field, "mcc_single_tx")) {
		for (i = 0; i < v->iface_nums && i < MAX_IFACE; i++) {
			if (!dvobj.padapters[i])
				continue;
			if (dvobj.padapters[i]->registrypriv.rtw_mcc_single_tx_cri !=
			    v->expect_single_tx[i]) {
				fprintf(stderr,
					"%s: iface %u single_tx %u != %u\n",
					v->name, i,
					dvobj.padapters[i]
						->registrypriv.rtw_mcc_single_tx_cri,
					v->expect_single_tx[i]);
				return -1;
			}
		}
	}

	if (v->has_expect_target_tp) {
		struct _adapter *padapter = rtw_netdev_priv(&dev);
		u32 *field = target_tp_field(padapter, v->set_field);

		if (!field) {
			fprintf(stderr, "%s: unknown tp set %s\n", v->name,
				v->set_field);
			return -1;
		}
		if (*field != v->expect_target_tp) {
			fprintf(stderr, "%s: target_tp %u != %u\n", v->name,
				*field, v->expect_target_tp);
			return -1;
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	const char *path = argc > 1 ? argv[1] : "debug_proc_mcc_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	nvec = 0;
	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "load vectors failed\n");
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (check_vector(&vectors[i])) {
			fprintf(stderr, "FAIL %s\n", vectors[i].name);
			failed++;
		}
	}

	if (failed)
		return 1;
	printf("OK %zu vectors\n", nvec);
	return 0;
}
