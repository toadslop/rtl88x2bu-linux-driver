// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for phy parameter struct mapper (W4-26). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal_halmac_phy_parameter.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	uint8_t cmd;
	uint32_t value;
	uint32_t msk;
	uint16_t offset;
	uint8_t msk_en;
	uint8_t size;
	uint8_t rf_path;
	uint8_t delay_unit;
	uint16_t delay_value;
	int expect_ret;
	uint32_t expect_cmd;
	uint32_t expect_value;
	uint16_t expect_delay;
};

static int parse_u8(const char *obj, size_t len, const char *key, uint8_t *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (uint8_t)tmp;
	return 0;
}

static int parse_u16(const char *obj, size_t len, const char *key, uint16_t *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (uint16_t)tmp;
	return 0;
}

static int parse_u32(const char *obj, size_t len, const char *key, uint32_t *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (uint32_t)tmp;
	return 0;
}

static int parse_int(const char *obj, size_t len, const char *key, int *out)
{
	return host_json_parse_int_in(obj, len, key, out);
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_u8(obj, len, "cmd", &v->cmd))
		return -1;
	parse_u32(obj, len, "value", &v->value);
	parse_u32(obj, len, "msk", &v->msk);
	parse_u16(obj, len, "offset", &v->offset);
	parse_u8(obj, len, "msk_en", &v->msk_en);
	parse_u8(obj, len, "size", &v->size);
	parse_u8(obj, len, "rf_path", &v->rf_path);
	parse_u8(obj, len, "delay_unit", &v->delay_unit);
	parse_u16(obj, len, "delay_value", &v->delay_value);
	if (parse_int(obj, len, "expect_ret", &v->expect_ret))
		return -1;
	parse_u32(obj, len, "expect_cmd", &v->expect_cmd);
	parse_u32(obj, len, "expect_value", &v->expect_value);
	parse_u16(obj, len, "expect_delay", &v->expect_delay);
	return 0;
}

static void fill_para(const struct vector *v, struct rtw_phy_parameter *para)
{
	memset(para, 0, sizeof(*para));
	para->cmd = v->cmd;
	switch (v->cmd) {
	case 0:
		para->data.mac.value = v->value;
		para->data.mac.msk = v->msk;
		para->data.mac.offset = v->offset;
		para->data.mac.msk_en = v->msk_en;
		para->data.mac.size = v->size;
		break;
	case 1:
		para->data.bb.value = v->value;
		para->data.bb.msk = v->msk;
		para->data.bb.offset = v->offset;
		para->data.bb.msk_en = v->msk_en;
		para->data.bb.size = v->size;
		break;
	case 2:
		para->data.rf.value = v->value;
		para->data.rf.msk = v->msk;
		para->data.rf.offset = (uint8_t)v->offset;
		para->data.rf.msk_en = v->msk_en;
		para->data.rf.path = v->rf_path;
		break;
	case 3:
		para->data.delay.unit = v->delay_unit;
		para->data.delay.value = v->delay_value;
		break;
	default:
		break;
	}
}

static int check_expect(const struct vector *v,
			const struct halmac_phy_parameter_info *info)
{
	if ((uint32_t)info->cmd_id != v->expect_cmd)
		return -1;

	switch (v->cmd) {
	case 0:
		if (info->content.MAC_REG_W.value != v->expect_value ||
		    info->content.MAC_REG_W.msk != v->msk ||
		    info->content.MAC_REG_W.offset != v->offset ||
		    info->content.MAC_REG_W.msk_en != v->msk_en)
			return -1;
		break;
	case 1:
		if (info->content.BB_REG_W.value != v->expect_value ||
		    info->content.BB_REG_W.msk != v->msk ||
		    info->content.BB_REG_W.offset != v->offset ||
		    info->content.BB_REG_W.msk_en != v->msk_en)
			return -1;
		break;
	case 2:
		if (info->content.RF_REG_W.value != v->expect_value ||
		    info->content.RF_REG_W.msk != v->msk ||
		    info->content.RF_REG_W.offset != (uint8_t)v->offset ||
		    info->content.RF_REG_W.msk_en != v->msk_en ||
		    info->content.RF_REG_W.rf_path != v->rf_path)
			return -1;
		break;
	case 3:
		if (info->content.DELAY_TIME.delay_time != v->expect_delay)
			return -1;
		break;
	case 0xFF:
		break;
	default:
		break;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] :
			   "hal_halmac_phy_param_drv2halmac_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct vector *v = &vectors[i];
		struct rtw_phy_parameter para;
		struct halmac_phy_parameter_info info;
		int ret;

		fill_para(v, &para);
		memset(&info, 0xAA, sizeof(info));
		ret = phy_parameter_drv2halmac(&para, &info);
		if (ret != v->expect_ret) {
			fprintf(stderr, "FAIL %s ret: got=%d expect=%d\n",
				v->name, ret, v->expect_ret);
			return 1;
		}
		if (ret == 0 && check_expect(v, &info)) {
			fprintf(stderr, "FAIL %s output mismatch\n", v->name);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
