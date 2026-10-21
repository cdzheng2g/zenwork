#!/bin/sh
set -eu
JOBS="${JOBS:-2}"
cd "$(dirname "$0")/.."
ROCKIT_MPI_SDK_DIR="${ROCKIT_MPI_SDK_DIR:-/home/zen/下载/rk_mpi/mpi/sdk}"
if [ ! -f "$ROCKIT_MPI_SDK_DIR/include/rk_mpi_sys.h" ]; then
  echo "Rockit SDK not found. Set ROCKIT_MPI_SDK_DIR to your MPI SDK directory." >&2
  exit 1
fi
cmake -S . -B build_rk3576 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_aarch64_rk3576.cmake \
  -DQT_SDK_DIR=/tronlong/sys_ub/sysroots/armv8a-linux/usr \
  -DHOST_QT_BIN_DIR=/tronlong/sys_ub/sysroots/x86_64-linux/bin \
  -DROCKIT_MPI_SDK_DIR="$ROCKIT_MPI_SDK_DIR" \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_POSITION_CORRECTION_TEST=ON
cmake --build build_rk3576 --parallel "$JOBS"
