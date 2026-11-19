#!/usr/bin/env bash
# Default RTL8822B builds must not inject feature macros that hal_ic_cfg.h
# leaves off. -DCONFIG_DYNAMIC_SOML grows registry_priv inside _ADAPTER and
# enables adaptive SOML (rtw_dynamic_soml_en defaults to 1).
# -DRTW_PER_CMD_SUPPORT_FW is defined by the header only under CONFIG_RTW_MESH.
set -euo pipefail

root=$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)
makefile="${root}/Makefile"

if grep -Eq '^[[:space:]]*ccflags-y[[:space:]]*\+=[[:space:]]*-DCONFIG_DYNAMIC_SOML\b' "$makefile"; then
	echo "test-no-forced-hal-defines: Makefile forces -DCONFIG_DYNAMIC_SOML" >&2
	exit 1
fi

if grep -Eq '^[[:space:]]*ccflags-y[[:space:]]*\+=[[:space:]]*-DRTW_PER_CMD_SUPPORT_FW\b' "$makefile"; then
	echo "test-no-forced-hal-defines: Makefile forces -DRTW_PER_CMD_SUPPORT_FW" >&2
	exit 1
fi

echo "test-no-forced-hal-defines: OK"
