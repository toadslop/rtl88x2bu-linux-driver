// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

struct _adapter;

void host_sec_cam_reset(void);
void host_sec_cam_set_slot(unsigned char addr, unsigned int val);
extern int host_sec_cam_bad_write;
void rtw_sec_read_cam_ent_c_oracle(struct _adapter *adapter, unsigned char id,
				   unsigned char *ctrl, unsigned char *mac,
				   unsigned char *key);
#ifdef RUST_HAL_COM_SEC_CAM_ENT_ORACLE
void rtw_sec_read_cam_ent(struct _adapter *, unsigned char, unsigned char *,
			  unsigned char *, unsigned char *);
#endif

int main(void)
{
	struct _adapter *adapter = NULL;
	const unsigned char id = 3;
	unsigned short ctrl_c, ctrl_r;
	unsigned char key_c[16], key_r[16];
	unsigned char i;

	host_sec_cam_reset();
	for (i = 0; i <= 5; i++)
		host_sec_cam_set_slot((unsigned char)((id << 3) | i),
				      0xA0B0C0D0u + i);
	memset(&ctrl_c, 0, sizeof(ctrl_c));
	memset(key_c, 0xaa, sizeof(key_c));
	host_sec_cam_bad_write = 0;
	rtw_sec_read_cam_ent_c_oracle(adapter, id, (unsigned char *)&ctrl_c, NULL,
				      key_c);
	if (host_sec_cam_bad_write) {
		fprintf(stderr, "FAIL: C oracle low-address write\n");
		return 1;
	}
#ifdef RUST_HAL_COM_SEC_CAM_ENT_ORACLE
	host_sec_cam_reset();
	for (i = 0; i <= 5; i++)
		host_sec_cam_set_slot((unsigned char)((id << 3) | i),
				      0xA0B0C0D0u + i);
	memset(&ctrl_r, 0, sizeof(ctrl_r));
	memset(key_r, 0xbb, sizeof(key_r));
	host_sec_cam_bad_write = 0;
	rtw_sec_read_cam_ent(adapter, id, (unsigned char *)&ctrl_r, NULL, key_r);
	if (host_sec_cam_bad_write) {
		fprintf(stderr, "FAIL: Rust low-address write (NULL+offset)\n");
		return 1;
	}
	if (memcmp(&ctrl_c, &ctrl_r, sizeof(ctrl_c)) ||
	    memcmp(key_c, key_r, sizeof(key_c))) {
		fprintf(stderr, "FAIL: Rust differs from C oracle\n");
		return 1;
	}
#else
	(void)ctrl_r;
	(void)key_r;
#endif
	printf("PASS: ctrl-nonnull mac-null key-nonnull (FX-12)\n");
	return 0;
}
