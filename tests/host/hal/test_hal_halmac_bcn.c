// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for beacon ctrl mappers (W4-10). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct halmac_bcn_ctrl {
	uint8_t dis_rx_bssid_fit;
	uint8_t en_txbcn_rpt;
	uint8_t dis_tsf_udt;
	uint8_t en_bcn;
	uint8_t en_rxbcn_rpt;
	uint8_t en_p2p_ctwin;
	uint8_t en_p2p_bcn_area;
};

struct rtw_halmac_bcn_ctrl {
	uint8_t rx_bssid_fit;
	uint8_t txbcn_rpt;
	uint8_t tsf_update;
	uint8_t enable_bcn;
	uint8_t rxbcn_rpt;
	uint8_t p2p_ctwin;
	uint8_t p2p_bcn_area;
};

struct vector {
	char name[MAX_NAME];
	int direction;
	struct halmac_bcn_ctrl expect_halmac;
	struct rtw_halmac_bcn_ctrl expect_drv;
};

void beacon_ctrl_halmac2drv(const struct halmac_bcn_ctrl *ctrl,
			    struct rtw_halmac_bcn_ctrl *drv_ctrl);
void beacon_ctrl_drv2halmac(const struct rtw_halmac_bcn_ctrl *drv_ctrl,
			    struct halmac_bcn_ctrl *ctrl);

static int parse_u8_field(const char *obj, size_t len, const char *key,
			  uint8_t *out)
{
	int tmp;

	if (host_json_parse_int_in(obj, len, key, &tmp))
		return -1;
	*out = (uint8_t)tmp;
	return 0;
}

static int parse_halmac_fields(const char *obj, size_t len,
			       struct halmac_bcn_ctrl *h)
{
	if (parse_u8_field(obj, len, "h_dis_rx_bssid_fit", &h->dis_rx_bssid_fit))
		return -1;
	if (parse_u8_field(obj, len, "h_en_txbcn_rpt", &h->en_txbcn_rpt))
		return -1;
	if (parse_u8_field(obj, len, "h_dis_tsf_udt", &h->dis_tsf_udt))
		return -1;
	if (parse_u8_field(obj, len, "h_en_bcn", &h->en_bcn))
		return -1;
	if (parse_u8_field(obj, len, "h_en_rxbcn_rpt", &h->en_rxbcn_rpt))
		return -1;
	if (parse_u8_field(obj, len, "h_en_p2p_ctwin", &h->en_p2p_ctwin))
		return -1;
	if (parse_u8_field(obj, len, "h_en_p2p_bcn_area", &h->en_p2p_bcn_area))
		return -1;
	return 0;
}

static int parse_drv_fields(const char *obj, size_t len,
			    struct rtw_halmac_bcn_ctrl *d)
{
	if (parse_u8_field(obj, len, "d_rx_bssid_fit", &d->rx_bssid_fit))
		return -1;
	if (parse_u8_field(obj, len, "d_txbcn_rpt", &d->txbcn_rpt))
		return -1;
	if (parse_u8_field(obj, len, "d_tsf_update", &d->tsf_update))
		return -1;
	if (parse_u8_field(obj, len, "d_enable_bcn", &d->enable_bcn))
		return -1;
	if (parse_u8_field(obj, len, "d_rxbcn_rpt", &d->rxbcn_rpt))
		return -1;
	if (parse_u8_field(obj, len, "d_p2p_ctwin", &d->p2p_ctwin))
		return -1;
	if (parse_u8_field(obj, len, "d_p2p_bcn_area", &d->p2p_bcn_area))
		return -1;
	return 0;
}

static int drv_eq(const struct rtw_halmac_bcn_ctrl *a,
		  const struct rtw_halmac_bcn_ctrl *b)
{
	return a->rx_bssid_fit == b->rx_bssid_fit &&
	       a->txbcn_rpt == b->txbcn_rpt && a->tsf_update == b->tsf_update &&
	       a->enable_bcn == b->enable_bcn &&
	       a->rxbcn_rpt == b->rxbcn_rpt && a->p2p_ctwin == b->p2p_ctwin &&
	       a->p2p_bcn_area == b->p2p_bcn_area;
}

static int halmac_eq(const struct halmac_bcn_ctrl *a,
		     const struct halmac_bcn_ctrl *b)
{
	return a->dis_rx_bssid_fit == b->dis_rx_bssid_fit &&
	       a->en_txbcn_rpt == b->en_txbcn_rpt &&
	       a->dis_tsf_udt == b->dis_tsf_udt && a->en_bcn == b->en_bcn &&
	       a->en_rxbcn_rpt == b->en_rxbcn_rpt &&
	       a->en_p2p_ctwin == b->en_p2p_ctwin &&
	       a->en_p2p_bcn_area == b->en_p2p_bcn_area;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "direction", &tmp))
		return -1;
	v->direction = tmp;
	if (parse_halmac_fields(obj, len, &v->expect_halmac))
		return -1;
	if (parse_drv_fields(obj, len, &v->expect_drv))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_halmac_bcn_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct vector *v = &vectors[i];

		if (v->direction == 0) {
			struct halmac_bcn_ctrl in = v->expect_halmac;
			struct rtw_halmac_bcn_ctrl got;

			beacon_ctrl_halmac2drv(&in, &got);
			if (!drv_eq(&got, &v->expect_drv)) {
				fprintf(stderr, "FAIL %s halmac2drv\n", v->name);
				return 1;
			}
		} else {
			struct rtw_halmac_bcn_ctrl in = v->expect_drv;
			struct halmac_bcn_ctrl got;

			beacon_ctrl_drv2halmac(&in, &got);
			if (!halmac_eq(&got, &v->expect_halmac)) {
				fprintf(stderr, "FAIL %s drv2halmac\n", v->name);
				return 1;
			}
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
