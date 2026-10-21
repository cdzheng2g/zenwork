#!/usr/bin/env bash
# Regression: deployment bundles must not replace the target board's glibc
# runtime.  Doing so makes librkaiq crash during camera initialization.
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TEMP_DIR"' EXIT

"$PROJECT_ROOT/scripts/package_rk3576.sh" \
    "$PROJECT_ROOT/build_rk3576" "$TEMP_DIR"

PACKAGE_LIB_DIR="$TEMP_DIR/HansAiCameraApp_rk3576/lib"
for system_library in \
    libc.so.6 ld-linux-aarch64.so.1 libpthread.so.0 libdl.so.2 librt.so.1 \
    libm.so.6 libutil.so.1 libanl.so.1 libresolv.so.2; do
    if [[ -e "$PACKAGE_LIB_DIR/$system_library" ]]; then
        echo "deployment bundle incorrectly contains board system runtime: $system_library" >&2
        exit 1
    fi
done

if [[ ! -e "$PACKAGE_LIB_DIR/libmali.so.1" ]]; then
    echo "deployment bundle is missing libmali.so.1 required by libgraphic_lsf.so" >&2
    exit 1
fi

# The runtime must stay paired with the board IQ, not the older sysroot AIQ.
cmp "$PROJECT_ROOT/third_party/rkaiq/lib/librkaiq.so" "$PACKAGE_LIB_DIR/librkaiq.so"
test -s "$PACKAGE_LIB_DIR/librga.so"
