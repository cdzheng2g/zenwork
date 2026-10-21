#!/usr/bin/env bash
# 打包RK3576交叉编译产物:递归收集HansAiCameraApp的运行时依赖库,
# 连同可执行文件一起打成一个tar包,附带启动脚本(设置LD_LIBRARY_PATH指向包内lib,
# 不依赖/覆盖板子系统库,避免版本不一致问题)。
#
# 用法: scripts/package_rk3576.sh [build目录] [输出目录]
#   默认build目录: build_rk3576
#   默认输出目录: dist

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-$PROJECT_ROOT/build_rk3576}"
OUT_DIR="${2:-$PROJECT_ROOT/dist}"
SYSROOT="/tronlong/sys_ub/sysroots/armv8a-linux"
BIN="$BUILD_DIR/bin/HansAiCameraApp"
PKG_NAME="HansAiCameraApp_rk3576"
PKG_DIR="$OUT_DIR/$PKG_NAME"
# rockit MPI SDK不在sysroot里(机器本地路径,板子系统也不自带librockit.so/
# librkaiq.so的辅助库),必须一起打包,否则板子上会因找不到.so启动失败
ROCKIT_LIB_DIR="/home/zen/下载/rk_mpi/mpi/sdk/lib/lib64"

if [ ! -f "$BIN" ]; then
    echo "错误: 找不到可执行文件 $BIN,请先完成交叉编译" >&2
    exit 1
fi

rm -rf "$PKG_DIR"
mkdir -p "$PKG_DIR/bin" "$PKG_DIR/lib" "$PKG_DIR/plugins/platforms" "$PKG_DIR/plugins/platforminputcontexts" \
    "$PKG_DIR/plugins/egldeviceintegrations" "$PKG_DIR/plugins/generic" "$PKG_DIR/plugins/xcbglintegrations" \
    "$PKG_DIR/resource"

echo "==> 复制Qt平台插件(eglfs,用于无X11环境下直接渲染到屏幕)..."
PLUGIN_SRC="$SYSROOT/usr/lib/aarch64-linux-gnu/qt5/plugins/platforms"
for p in libqeglfs.so libqxcb.so libqminimal.so; do
    if [ -f "$PLUGIN_SRC/$p" ]; then
        cp "$PLUGIN_SRC/$p" "$PKG_DIR/plugins/platforms/"
    fi
done

echo "==> 复制虚拟键盘输入法插件(Qt5SoftKeyboard,无桌面环境下点击输入框弹出软键盘)..."
IM_PLUGIN="$BUILD_DIR/plugins/platforminputcontexts/Qt5SoftKeyboard.so"
if [ -f "$IM_PLUGIN" ]; then
    cp "$IM_PLUGIN" "$PKG_DIR/plugins/platforminputcontexts/"
else
    echo "警告: 找不到 $IM_PLUGIN,软键盘将无法弹出" >&2
fi

echo "==> 复制eglfs/xcb渲染与触摸输入依赖的其他插件类别..."
# egldeviceintegrations: eglfs后端选择具体EGL设备集成方式必需;
# generic: 触摸/手势等通用输入设备插件(evdevtouch等)常在这里;
# xcbglintegrations: xcb后端下OpenGL渲染集成。
# 之前只打了platforms/platforminputcontexts两类,板子上一度靠不设QT_PLUGIN_PATH时
# Qt退回sysroot默认路径蒙对,一旦显式设置QT_PLUGIN_PATH指向包内目录,这些类别就
# 因为包里没有而找不到,导致渲染/触摸大面积失效——这里连同打包,run.sh不再依赖
# 开发机sysroot路径。
for category in egldeviceintegrations generic xcbglintegrations; do
    SRC_DIR="$SYSROOT/usr/lib/aarch64-linux-gnu/qt5/plugins/$category"
    if [ -d "$SRC_DIR" ]; then
        cp "$SRC_DIR"/*.so "$PKG_DIR/plugins/$category/" 2>/dev/null || true
    fi
done

echo "==> 递归解析运行时依赖库..."
# 依赖解析的起点除了可执行文件本身,还要包含刚复制进包内的所有插件.so——
# 插件文件存在不代表能加载,它们自己的NEEDED依赖(比如libqeglfs-kms-integration.so
# 依赖libdrm/libgbm等)如果没被解析进来,会导致插件在dlopen时因缺库静默失败。
PLUGIN_SEEDS=$(find "$PKG_DIR/plugins" -name '*.so' 2>/dev/null)
python3 - "$SYSROOT" "$BIN" "$PROJECT_ROOT/build_rk3576/third_party/ElaWidgetTools" "$ROCKIT_LIB_DIR" "$PKG_DIR/lib" $PLUGIN_SEEDS <<'PYEOF'
import subprocess, sys, os, collections, shutil

sysroot, bin_path, elawidget_dir, rockit_lib_dir, dest_lib_dir = (
    sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
plugin_seeds = sys.argv[6:]

search_dirs = [
    os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(elawidget_dir))), "third_party/rkaiq/lib"),
    os.path.join(sysroot, "usr/lib/aarch64-linux-gnu"),
    os.path.join(sysroot, "lib/aarch64-linux-gnu"),
    os.path.join(sysroot, "usr/lib"),
    os.path.join(sysroot, "lib"),
    elawidget_dir,
    rockit_lib_dir,
]

def get_needed(path):
    out = subprocess.run(["readelf", "-d", path], capture_output=True, text=True).stdout
    needed = []
    for line in out.splitlines():
        if "NEEDED" in line:
            needed.append(line.split("[")[-1].split("]")[0])
    return needed

def find_lib(name):
    for d in search_dirs:
        p = os.path.join(d, name)
        if os.path.isfile(p) or os.path.islink(p):
            real = os.path.realpath(p)
            if os.path.isfile(real):
                return real
    return None

resolved = {}
queue = collections.deque(get_needed(bin_path) + ["librga.so"])
for seed in plugin_seeds:
    queue.extend(get_needed(seed))
seen = set(queue)

# These libraries form the target operating system's glibc runtime.  They
# must be loaded as one matched set with the board's dynamic loader, never
# overridden through this bundle's LD_LIBRARY_PATH.  Bundling a sysroot copy
# made librkaiq crash in rk_aiq_uapi2_sysctl_init on the RK3576 board.
board_runtime_libraries = {
    "ld-linux-aarch64.so.1",
    "libanl.so.1",
    "libc.so.6",
    "libdl.so.2",
    "libm.so.6",
    "libpthread.so.0",
    "libresolv.so.2",
    "librt.so.1",
    "libutil.so.1",
}

while queue:
    name = queue.popleft()
    if name in resolved:
        continue
    if name in board_runtime_libraries:
        print(f"  {name}  <- board system runtime (not bundled)")
        continue
    real = find_lib(name)
    if not real:
        print(f"警告: 找不到 {name}", file=sys.stderr)
        continue
    resolved[name] = real
    for dep in get_needed(real):
        if dep not in seen:
            seen.add(dep)
            queue.append(dep)

# Qt modules using private APIs must match the bundled Qt Core build.
qt_core = resolved.get("libQt5Core.so.5")
if not qt_core:
    raise SystemExit("Missing Qt Core runtime")
qt_version = os.path.basename(qt_core).split(".so.", 1)[-1]
for soname, real_path in resolved.items():
    if soname.startswith("libQt5"):
        module_version = os.path.basename(real_path).split(".so.", 1)[-1]
        if module_version != qt_version:
            raise SystemExit(f"Incompatible Qt runtime: {soname} is {module_version}, Qt Core is {qt_version}")

os.makedirs(dest_lib_dir, exist_ok=True)
for soname, real_path in sorted(resolved.items()):
    dest = os.path.join(dest_lib_dir, soname)
    shutil.copy2(real_path, dest)
    os.chmod(dest, 0o755)
    print(f"  {soname}  <-  {real_path}")

print(f"共复制 {len(resolved)} 个运行时库", file=sys.stderr)
PYEOF

echo "==> 复制可执行文件..."
cp "$BIN" "$PKG_DIR/bin/"
aarch64-linux-gnu-strip --strip-unneeded "$PKG_DIR/bin/HansAiCameraApp" || true
PROBE_BIN="$BUILD_DIR/bin/CamVoViProbe"
if [ -f "$PROBE_BIN" ]; then
    cp "$PROBE_BIN" "$PKG_DIR/bin/"
    aarch64-linux-gnu-strip --strip-unneeded "$PKG_DIR/bin/CamVoViProbe" || true
fi

# Rockit在运行时通过dlopen按名称加载该图形/缓冲辅助库，不会出现在ELF的
# NEEDED列表中，递归readelf扫描无法自动发现。缺失时会出现
# "failed to load library(libgraphic_lsf.so)"，并可能导致VI任务图无法申请
# V4L2缓冲。与cam_vo参考SDK保持一致，显式随包部署。
if [ -f "$ROCKIT_LIB_DIR/libgraphic_lsf.so" ]; then
    echo "==> 复制Rockit运行时动态加载库(libgraphic_lsf.so)..."
    cp "$ROCKIT_LIB_DIR/libgraphic_lsf.so" "$PKG_DIR/lib/"
else
    echo "警告: 找不到$ROCKIT_LIB_DIR/libgraphic_lsf.so" >&2
fi

# libgraphic_lsf.so由Rockit在运行时dlopen，并依赖Mali GPU用户态库；由于它在
# 依赖扫描之后才复制，不能依赖上面的递归扫描自动带入该二级依赖。
MALI_LIBRARY="$SYSROOT/usr/lib/aarch64-linux-gnu/libmali.so.1.9.0"
if [ -f "$MALI_LIBRARY" ]; then
    echo "==> 复制Rockit图形依赖(libmali.so.1)..."
    cp "$MALI_LIBRARY" "$PKG_DIR/lib/libmali.so.1"
else
    echo "警告: 找不到$MALI_LIBRARY，libgraphic_lsf.so将无法加载" >&2
fi

if [ -d "$PROJECT_ROOT/resource" ]; then
    echo "==> 复制resource目录..."
    cp -r "$PROJECT_ROOT/resource/." "$PKG_DIR/resource/"
fi

cp "$PROJECT_ROOT/scripts/setup_camera_permissions.sh" "$PKG_DIR/setup_camera_permissions.sh"
chmod +x "$PKG_DIR/setup_camera_permissions.sh"
echo "==> 生成启动脚本..."
cat > "$PKG_DIR/run.sh" <<'RUNEOF'
#!/usr/bin/env bash
# 板子上运行:进入本目录后执行 ./run.sh
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$DIR/lib:${LD_LIBRARY_PATH:-}"
export QT_QPA_PLATFORM_PLUGIN_PATH="$DIR/plugins/platforms"
# 只用包内plugins目录,不依赖板子系统/开发机sysroot下的Qt插件路径(egldeviceintegrations/
# generic/xcbglintegrations/platforminputcontexts等类别子目录都已在打包时一并复制进来,
# 详见package_rk3576.sh)
export QT_PLUGIN_PATH="$DIR/plugins"
# 板子跑的是X11桌面(Xorg),用xcb后端让程序作为普通X客户端,才能通过X server
# 拿到触摸/键盘事件并弹出软键盘。之前默认eglfs会去抢DRM设备导致DRI2认证失败、
# 回退软件渲染且收不到输入事件(桌面能触屏但程序里点不动)。
# 若某台板子确实无桌面环境(纯framebuffer),运行时用 QT_QPA_PLATFORM=eglfs ./run.sh 覆盖。
export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-xcb}"
# xcb客户端需要知道连哪个X server;板子实测Xorg跑在:1(见/tmp/.X11-unix/X1)。
# 用户已有DISPLAY时不覆盖,方便在别的显示号上调试。
export DISPLAY="${DISPLAY:-:1}"
# RKAIQ reports start success even if its SCHED_RR statistics thread fails.
# Ensure priority 20 is available before opening the camera.
camera_rtprio="$(ulimit -r)"
if [[ "$camera_rtprio" != unlimited ]] && (( camera_rtprio < 20 )); then
    if ! ulimit -S -r 20 2>/dev/null; then
        echo "ISP requires rtprio 20. Run sudo \"$DIR/setup_camera_permissions.sh\", then log out and log in again." >&2
        exit 1
    fi
fi
exec "$DIR/bin/HansAiCameraApp" "$@"
RUNEOF
chmod +x "$PKG_DIR/run.sh"

cat > "$PKG_DIR/debug_run.sh" <<'DEBUGEOF'
#!/usr/bin/env bash
# 一键抓取段错误调用栈：执行后在触摸屏复现问题，退出后查看 debug_new_scheme.log。
set -u
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOG="$DIR/debug_new_scheme.log"
export LD_LIBRARY_PATH="$DIR/lib:${LD_LIBRARY_PATH:-}"
export QT_QPA_PLATFORM_PLUGIN_PATH="$DIR/plugins/platforms"
export QT_PLUGIN_PATH="$DIR/plugins"
export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-xcb}"
export DISPLAY="${DISPLAY:-:1}"
# RKAIQ reports start success even if its SCHED_RR statistics thread fails.
# Ensure priority 20 is available before opening the camera.
camera_rtprio="$(ulimit -r)"
if [[ "$camera_rtprio" != unlimited ]] && (( camera_rtprio < 20 )); then
    if ! ulimit -S -r 20 2>/dev/null; then
        echo "ISP requires rtprio 20. Run sudo \"$DIR/setup_camera_permissions.sh\", then log out and log in again." >&2
        exit 1
    fi
fi
if ! command -v gdb >/dev/null 2>&1; then
  echo "未安装 gdb；请执行 sudo apt install gdb" | tee "$LOG"
  exit 1
fi
echo "调试已启动：请在触摸屏复现“新建方案 → 输入 1”。" | tee "$LOG"
gdb --batch --quiet \
  -ex run \
  -ex 'thread apply all bt' \
  --args "$DIR/bin/HansAiCameraApp" 2>&1 | tee -a "$LOG"
echo "日志已保存：$LOG"
DEBUGEOF
chmod +x "$PKG_DIR/debug_run.sh"

echo "==> 打包为tar.gz..."
mkdir -p "$OUT_DIR"
tar -C "$OUT_DIR" -czf "$OUT_DIR/${PKG_NAME}.tar.gz" "$PKG_NAME"

echo ""
echo "打包完成: $OUT_DIR/${PKG_NAME}.tar.gz"
echo "板子上部署方式:"
echo "  scp $OUT_DIR/${PKG_NAME}.tar.gz <板子>:/path/to/somewhere/"
echo "  ssh <板子> 'cd /path/to/somewhere && tar xzf ${PKG_NAME}.tar.gz && cd ${PKG_NAME} && ./run.sh'"
