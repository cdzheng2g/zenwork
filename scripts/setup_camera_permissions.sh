#!/usr/bin/env bash
# Run once on the board: sudo ./setup_camera_permissions.sh [login-user]
set -euo pipefail
camera_user="${1:-${SUDO_USER:-}}"
if [[ $EUID -ne 0 || -z "$camera_user" || "$camera_user" == root || ! "$camera_user" =~ ^[a-zA-Z_][a-zA-Z0-9_.-]*\$?$ ]]; then
    echo 'Usage: sudo ./setup_camera_permissions.sh <camera-login-user>' >&2
    exit 1
fi
id "$camera_user" >/dev/null
limits_file="/etc/security/limits.d/90-hans-camera-${camera_user}.conf"
if [[ -f "$limits_file" ]]; then
    cp -p "$limits_file" "${limits_file}.bak-$(date +%Y%m%d-%H%M%S)"
fi
# RKAIQ requests SCHED_RR priority 20 for the ISP statistics thread.
printf '# HansAiCameraApp: allow RKAIQ ISP statistics thread (SCHED_RR 20).\n%s soft rtprio 20\n%s hard rtprio 20\n' "$camera_user" "$camera_user" > "$limits_file"
chmod 644 "$limits_file"
echo "Configured rtprio 20 for $camera_user. Log out and log in again before ./run.sh."
