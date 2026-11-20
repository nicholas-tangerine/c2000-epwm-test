#!/usr/bin/env bash
set -e

#
# Directory setup
#
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

#
# Paths to TI toolchain and C2000Ware
#
CGT_BIN="${CGT_BIN:-/home/wqnderalone/ti/ti-cgt-c2000_25.11.1.LTS/bin}"
C2000WARE="${C2000WARE:-/home/wqnderalone/ti/C2000Ware_26_01_00_00}"
DEVICE_C="${DEVICE_C:-$C2000WARE/device_support/f28p65x/common/source/device.c}"
CMD_FILE="${CMD_FILE:-$C2000WARE/device_support/f28p65x/common/cmd/28p65x_generic_flash_lnk_cpu1.cmd}"
CODESTART="${CODESTART:-$C2000WARE/device_support/f28p65x/common/source/f28p65x_codestartbranch.asm}"

echo "Using Compiler: $CGT_BIN/cl2000"
echo "Using C2000Ware: $C2000WARE"
echo "Building epwm_test.out..."

"$CGT_BIN/cl2000" \
  -v28 -ml -mt --abi=eabi --float_support=fpu64 --tmu_support=tmu1 --fp_mode=relaxed \
  --define=CPU1 --define=_FLASH --define=USE_PLL_SRC_INTOSC \
  -I"$SCRIPT_DIR" \
  -I"$C2000WARE/driverlib/f28p65x/driverlib" \
  -I"$C2000WARE/driverlib/f28p65x" \
  -I"$C2000WARE/device_support/f28p65x/common/include" \
  -I"$CGT_BIN/../include" \
  epwm_ex2_updown_aq.c \
  "$DEVICE_C" \
  "$CODESTART" \
  "$C2000WARE/driverlib/f28p65x/driverlib/epwm.c" \
  "$C2000WARE/driverlib/f28p65x/driverlib/gpio.c" \
  "$C2000WARE/driverlib/f28p65x/driverlib/sysctl.c" \
  "$C2000WARE/driverlib/f28p65x/driverlib/asysctl.c" \
  "$C2000WARE/driverlib/f28p65x/driverlib/interrupt.c" \
  "$C2000WARE/driverlib/f28p65x/driverlib/dcc.c" \
  "$C2000WARE/driverlib/f28p65x/driverlib/flash.c" \
  "$C2000WARE/driverlib/f28p65x/driverlib/ipc.c" \
  -z -o epwm_test.out -m epwm_test.map \
  --stack_size=0x400 \
  --entry_point=code_start \
  -i"$CGT_BIN/../lib" \
  --reread_libs --warn_sections --xml_link_info=epwm_test_linkInfo.xml --rom_model \
  "$CMD_FILE" -llibc.a

echo "Build complete: epwm_test.out successfully created."
