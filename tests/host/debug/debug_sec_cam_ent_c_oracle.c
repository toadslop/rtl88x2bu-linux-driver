// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-133 sec_cam_ent dump formatters (from core/rtw_debug_rest.c). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rtw_debug_sec_cam_types.h"

#define _SEC_TYPE_256_ 0x10

#define SEC_CAM_ENT_ID_TITLE_FMT "%-2s"
#define SEC_CAM_ENT_ID_TITLE_ARG "id"
#define SEC_CAM_ENT_ID_VALUE_FMT "%2u"
#define SEC_CAM_ENT_ID_VALUE_ARG(id) (id)

#define SEC_CAM_ENT_TITLE_FMT "%-6s %-17s %-32s %-3s %-8s %-2s %-2s %-5s"
#define SEC_CAM_ENT_TITLE_ARG "ctrl", "addr", "key", "kid", "type", "MK", "GK", "valid"
#define SEC_CAM_ENT_VALUE_FMT "0x%04x %02x:%02x:%02x:%02x:%02x:%02x %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x %3u %-8s %2u %2u %5u"
#define SEC_CAM_ENT_VALUE_ARG(ent) \
	(ent)->ctrl, \
	(ent)->mac[0], (ent)->mac[1], (ent)->mac[2], (ent)->mac[3], (ent)->mac[4], (ent)->mac[5], \
	(ent)->key[0], (ent)->key[1], (ent)->key[2], (ent)->key[3], \
	(ent)->key[4], (ent)->key[5], (ent)->key[6], (ent)->key[7], \
	(ent)->key[8], (ent)->key[9], (ent)->key[10], (ent)->key[11], \
	(ent)->key[12], (ent)->key[13], (ent)->key[14], (ent)->key[15], \
	((ent)->ctrl) & 0x03, \
	(((ent)->ctrl) & 0x200) ? \
	security_type_str((((ent)->ctrl) >> 2 & 0x7) | _SEC_TYPE_256_) : \
	security_type_str(((ent)->ctrl) >> 2 & 0x7), \
	(((ent)->ctrl) >> 5) & 0x01, \
	(((ent)->ctrl) >> 6) & 0x01, \
	(((ent)->ctrl) >> 15) & 0x01

void dump_sec_cam_ent(void *sel, struct sec_cam_ent *ent, int id)
{
	if (id >= 0) {
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_ID_VALUE_FMT " " SEC_CAM_ENT_VALUE_FMT "\n",
			      SEC_CAM_ENT_ID_VALUE_ARG(id), SEC_CAM_ENT_VALUE_ARG(ent));
	} else {
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_VALUE_FMT "\n", SEC_CAM_ENT_VALUE_ARG(ent));
	}
}

void dump_sec_cam_ent_title(void *sel, u8 has_id)
{
	if (has_id) {
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_ID_TITLE_FMT " " SEC_CAM_ENT_TITLE_FMT "\n",
			      SEC_CAM_ENT_ID_TITLE_ARG, SEC_CAM_ENT_TITLE_ARG);
	} else {
		RTW_PRINT_SEL(sel, SEC_CAM_ENT_TITLE_FMT "\n", SEC_CAM_ENT_TITLE_ARG);
	}
}

int dump_sec_cam_ent_format(struct sec_cam_ent *ent, int id, char *buf, size_t buflen)
{
	FILE *mem;
	char *out = NULL;
	size_t len = 0;
	int ret;

	if (!buf || buflen == 0)
		return -1;

	mem = open_memstream(&out, &len);
	if (!mem)
		return -1;

	dump_sec_cam_ent(mem, ent, id);
	fclose(mem);

	if (len == 0 || !out) {
		free(out);
		return -1;
	}
	if (out[len - 1] != '\n') {
		free(out);
		return -1;
	}
	if (len == 0 || out[len - 1] != '\n') {
		free(out);
		return -1;
	}
	out[len - 1] = '\0';
	ret = snprintf(buf, buflen, "%s", out);
	free(out);
	return ret > 0 ? (int)strlen(buf) : -1;
}

int dump_sec_cam_ent_title_format(u8 has_id, char *buf, size_t buflen)
{
	FILE *mem;
	char *out = NULL;
	size_t len = 0;
	int ret;

	if (!buf || buflen == 0)
		return -1;

	mem = open_memstream(&out, &len);
	if (!mem)
		return -1;

	dump_sec_cam_ent_title(mem, has_id);
	fclose(mem);

	if (len == 0 || !out) {
		free(out);
		return -1;
	}
	if (out[len - 1] != '\n') {
		free(out);
		return -1;
	}
	out[len - 1] = '\0';
	ret = snprintf(buf, buflen, "%s", out);
	free(out);
	return ret > 0 ? (int)strlen(buf) : -1;
}
