// SPDX-License-Identifier: GPL-2.0
/* FX-18 host L2 runner for debug proc input validation. */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_input_validation_types.h"

static int failures;

static void expect_int(const char *name, int got, int want)
{
	if (got != want) {
		fprintf(stderr, "FAIL %s: got %d want %d\n", name, got, want);
		failures++;
	}
}

static void test_nul_terminate_before_sscanf_ipv4_pattern(void)
{
	char tmp[128];
	const char input[] = "1 a b c d e f";
	int index, ip, subnet, gw, num;

	memset(tmp, 0xab, sizeof(tmp));
	memcpy(tmp, input, strlen(input));
	host_fx18_nul_terminate(tmp, sizeof(tmp), strlen(input));
	num = sscanf(tmp, "%d %x %x %x", &index, &ip, &subnet, &gw);
	expect_int("ipv4_pattern_sscanf_fields", num, 4);
	expect_int("ipv4_pattern_index", index, 1);
}

static void test_copy_user_nul_terminates(void)
{
	char tmp[16];
	const char input[] = "5 1 2 3";
	ssize_t copied;
	int index, a, b, c, num;

	memset(tmp, 0xff, sizeof(tmp));
	copied = rtw_rust_debug_proc_copy_user_input(input, strlen(input), tmp,
						    sizeof(tmp));
	expect_int("copy_user_len", (int)copied, (int)strlen(input));
	num = sscanf(tmp, "%d %d %d %d", &index, &a, &b, &c);
	expect_int("copy_user_sscanf_fields", num, 4);
	expect_int("copy_user_index", index, 5);
}

static void test_copy_user_oversize(void)
{
	char tmp[8];
	const char input[] = "123456789";

	expect_int("copy_user_oversize",
		   (int)rtw_rust_debug_proc_copy_user_input(input, strlen(input),
							   tmp, sizeof(tmp)),
		   -EFAULT);
}

static void test_ipv4_index(void)
{
	expect_int("ipv4_index_0", host_fx18_validate_war_ipv4_index(0), -EINVAL);
	expect_int("ipv4_index_5", host_fx18_validate_war_ipv4_index(5), -EINVAL);
	expect_int("ipv4_index_1", host_fx18_validate_war_ipv4_index(1), 0);
}

static void test_ipv6_index(void)
{
	expect_int("ipv6_index_0", host_fx18_validate_war_ipv6_index(0), -EINVAL);
	expect_int("ipv6_index_9", host_fx18_validate_war_ipv6_index(9), -EINVAL);
	expect_int("ipv6_index_8", host_fx18_validate_war_ipv6_index(8), 0);
}

static void test_mdns_service(void)
{
	expect_int("service_slot_full",
		   host_fx18_validate_mdns_service_slot(MAX_MDNS_SERVICE_NUM, 0),
		   -EINVAL);
	expect_int("service_txt_neg",
		   host_fx18_validate_mdns_service_slot(0, -1), -EINVAL);
	expect_int("service_ok", host_fx18_validate_mdns_service_slot(0, 0), 0);
}

static void test_txt_rsp_count_and_empty_str(void)
{
	const uint16_t max_input_size =
		(uint16_t)(1 + 6 + MAX_MDNS_TXT_SINGLE_LEN + 2);

	expect_int("txt_rsp_count_too_large",
		   host_fx18_validate_mdns_txt_rsp_count(max_input_size + 1,
							 max_input_size),
		   -EINVAL);
	expect_int("txt_rsp_empty_len", (int)host_fx18_mdns_txt_single_len(""), 0);
	expect_int("txt_rsp_nonempty_len",
		   (int)host_fx18_mdns_txt_single_len("ab"), 2);
}

int main(void)
{
	test_nul_terminate_before_sscanf_ipv4_pattern();
	test_copy_user_nul_terminates();
	test_copy_user_oversize();
	test_ipv4_index();
	test_ipv6_index();
	test_mdns_service();
	test_txt_rsp_count_and_empty_str();

	if (failures) {
		fprintf(stderr, "input_validation: %d failure(s)\n", failures);
		return 1;
	}
	printf("input_validation: ok\n");
	return 0;
}
