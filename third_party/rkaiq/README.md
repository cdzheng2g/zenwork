# RK3576 AIQ runtime

Pinned AIQ v6.0x9.0 (2024-12-13) from the board vendor WaterMeter_dvsdk,
`system_rk3576/rootfs/usr/lib/librkaiq.so`.
SHA-256: 3a199c16a0f4e00ebe1ed57933123510dde03632a63887cdb290839a3b515a56

The board SC130GS IQ file matches this SDK (SHA-256:
18ef425560dd95326422a2ef952d25067e970f44c0632967251e6e1d8c42e75c).
The older sysroot AIQ v6.0x6.3-rc1 produced all-zero ISP/VI output on first
stream start. This paired runtime passed two 100-frame VI/VPSS captures
under Qt/X11, including stop/restart. The original IQ file is unchanged.

## ISP statistics scheduling requirement

RKAIQ starts `isp_3a_stats_poll` with SCHED_RR priority 20. With the board
login user's default RLIMIT_RTPRIO=0, thread creation fails even though
sysctl_start returns success. Streaming continues with initial ISP settings:
AE/manual exposure writes do not reach the sensor and AWB gains remain fixed.

Run `sudo ./setup_camera_permissions.sh zen` once in the deployment directory,
then start a new login session. This installs only the named user's rtprio 20
soft/hard limits through PAM; the app continues to run as that user. run.sh and
debug_run.sh now check the limit before launch.

Verified on the board: 2 ms/1x -> sensor exposure 98/gain 32;
10 ms/4x -> exposure 494/gain 64; switching back to auto -> exposure 74/gain 32.
AWB gains update, and the running app's statistics thread is SCHED_RR/20.
This fixes the frozen 3A controls; final perceived color is checked on the board.
