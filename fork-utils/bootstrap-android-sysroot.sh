#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-2.1-or-later
#
# Bootstrap the sysroot/ directory with the third-party dependencies that
# bionic (Android's libc) does not provide, so systemd can be cross-compiled
# for Android/aarch64 with fork-utils/android-cross.ini:
#
#   1. libucontext — bionic has no ucontext APIs (getcontext/setcontext/...),
#                    systemd needs them for cooperative threading helpers.
#   2. libintl     — bionic has no gettext; systemd links gettext's intl
#                    library because locale-util.h uses the _() macro.
#
# Everything is installed into <repo>/sysroot/usr, the directory
# fork-utils/android-cross.ini points PKG_CONFIG_LIBDIR and the include
# search path at. sysroot/ is gitignored; re-run this script whenever it is
# missing or should be refreshed (remove it first for a from-scratch build).
#
# Requirements: git, curl, tar, meson, ninja, python3, GNU make, and the
# Android NDK (default /opt/android-ndk, override with ANDROID_NDK=; API
# level defaults to 35, override with ANDROID_API=).
#
# Usage: fork-utils/bootstrap-android-sysroot.sh
set -euo pipefail

REPO_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
SYSROOT=${SYSROOT:-"$REPO_ROOT/sysroot"}
NDK=${ANDROID_NDK:-/opt/android-ndk}
API=${ANDROID_API:-35}
NDK_BIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin
CC="$NDK_BIN/aarch64-linux-android$API-clang"
JOBS=$(nproc)

LIBUCONTEXT_COMMIT=a530eed9e7e6872e10fe92efaf1e9739471c30ca
GETTEXT_VERSION=0.26
CACHE=${XDG_CACHE_HOME:-"$HOME/.cache"}/systemd-android-sysroot

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

if ! test -x "$CC"; then
        echo "Android NDK compiler not found: $CC (set ANDROID_NDK=)" >&2
        exit 1
fi

mkdir -p "$SYSROOT/usr" "$CACHE"

echo "=== [1/3] Building libucontext ($LIBUCONTEXT_COMMIT) ==="
git clone -q https://github.com/kraj/libucontext.git "$WORK/libucontext"
git -C "$WORK/libucontext" checkout -q "$LIBUCONTEXT_COMMIT"

# The unit tests use unprefixed ucontext names (getcontext et al.) that
# bionic's <ucontext.h> does not declare, so they cannot compile for
# Android. Drop them, keeping the rest of the file intact.
python3 - "$WORK/libucontext/meson.build" <<'PY'
import sys

path = sys.argv[1]
lines = open(path).read().splitlines(keepends=True)
for i, line in enumerate(lines):
        if line.rstrip("\n") == "# Unit Tests":
                # Also drop the "# ==========" line opening the section.
                del lines[max(i - 1, 0):]
                lines.append("# Unit tests removed by bootstrap-android-sysroot.sh:\n")
                lines.append("# they use unprefixed ucontext names that bionic does not\n")
                lines.append("# declare in <ucontext.h>.\n")
                open(path, "w").writelines(lines)
                break
else:
        sys.exit("marker '# Unit Tests' not found in libucontext meson.build")
PY

meson setup "$WORK/libucontext-build" "$WORK/libucontext" \
        --cross-file "$REPO_ROOT/fork-utils/android-cross.ini" \
        -Dprefix="$SYSROOT/usr" \
        -Dlibdir=lib \
        -Ddefault_library=both
meson compile -C "$WORK/libucontext-build"
meson install -C "$WORK/libucontext-build"

echo "=== [2/3] Building gettext libintl ($GETTEXT_VERSION) ==="
TARBALL="gettext-$GETTEXT_VERSION.tar.xz"
if ! test -f "$CACHE/$TARBALL"; then
        curl -fL --retry 3 -o "$CACHE/$TARBALL.partial" \
                "https://ftp.gnu.org/gnu/gettext/$TARBALL"
        mv "$CACHE/$TARBALL.partial" "$CACHE/$TARBALL"
fi
tar -xf "$CACHE/$TARBALL" -C "$WORK"

# Only the intl/ sub-library is needed. Configure gettext-runtime (which in
# turn configures intl/), then build and install intl/ in isolation so no
# gettext tools — which would have to run on the build host — are involved.
cd "$WORK/gettext-$GETTEXT_VERSION/gettext-runtime"
./configure \
        --host=aarch64-linux-android \
        CC="$CC" \
        --prefix="$SYSROOT/usr" \
        --libdir="$SYSROOT/usr/lib" \
        --disable-static \
        --disable-java \
        --disable-csharp
make -C intl -j"$JOBS"
make -C intl install

echo "=== [3/3] Writing pkg-config metadata ==="
# gettext does not install a pkg-config file for libintl; write one that
# resolves its prefix relative to the .pc file so it stays relocatable.
cat > "$SYSROOT/usr/lib/pkgconfig/libintl.pc" <<'PC'
prefix=${pcfiledir}/../..
exec_prefix=${prefix}
libdir=${prefix}/lib
includedir=${prefix}/include

Name: libintl
Description: GNU gettext runtime library (libintl), cross-built for Android/bionic
Version: 0.26
Libs: -L${libdir} -lintl
Cflags: -I${includedir}
PC

echo "Sysroot ready: $SYSROOT/usr"
