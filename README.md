> [!IMPORTANT]
> Remove this line to confirm you've reviewed this PR before submitting.

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://github.com/system-alphabet/assets/raw/refs/heads/main/icon-with-name-dark-transparent.svg">
  <source media="(prefers-color-scheme: light)" srcset="https://github.com/system-alphabet/assets/raw/refs/heads/main/icon-with-name-light-transparent.svg">
  <img alt="Fallback icon" src="https://github.com/system-alphabet/assets/raw/refs/heads/main/icon-with-name-light-transparent.svg" style="width: 404; max-width: 600px; height: auto;">
</picture>

![Systemd](http://brand.systemd.io/assets/page-logo.png)

System and Service Manager — **SystemA fork with bionic support**

## SysA forking utilities

- `fork-utils/check-hardcoded-paths.py` — Scans source and test files for hardcoded paths.
- `fork-utils/replace-hardcoded-paths.py` — Bulk-replaces hardcoded paths with compile-time macros.
- `fork-utils/generate-dbus-api.py` — Generate D-Bus API documentation (YAML) from systemd's --bus-introspect output.
- `fork-utils/android-cross.ini` — Meson cross file for building for Android (aarch64) with the NDK.
- `fork-utils/bootstrap-android-sysroot.sh` — Builds the sysroot dependencies bionic lacks (libucontext, libintl).

[![(upstream) OBS Packages Status](https://build.opensuse.org/projects/system:systemd/packages/systemd/badge.svg?type=default)](https://build.opensuse.org/project/show/system:systemd)<br/>
[![(upstream) Semaphore CI 2.0 Build Status](https://the-real-systemd.semaphoreci.com/badges/systemd/branches/main.svg?style=shields)](https://the-real-systemd.semaphoreci.com/projects/systemd)<br/>
[![(upstream) Coverity Scan Status](https://scan.coverity.com/projects/350/badge.svg)](https://scan.coverity.com/projects/systemd)<br/>
[![(upstream) OSS-Fuzz Status](https://oss-fuzz-build-logs.storage.googleapis.com/badges/systemd.svg)](https://oss-fuzz-build-logs.storage.googleapis.com/index.html#systemd)<br/>
[![(upstream) CIFuzz](https://github.com/systemd/systemd/actions/workflows/cifuzz.yml/badge.svg)](https://github.com/systemd/systemd/actions/workflows/cifuzz.yml)</br>
[![(upstream) CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/1369/badge)](https://bestpractices.coreinfrastructure.org/projects/1369)<br/>
[![(upstream) Fossies codespell report](https://fossies.org/linux/test/systemd-main.tar.gz/codespell.svg)](https://fossies.org/linux/test/systemd-main.tar.gz/codespell.html)</br>
[![(upstream) Translation status](https://translate.fedoraproject.org/widget/systemd/svg-badge.svg)](https://translate.fedoraproject.org/engage/systemd/)</br>
[![(upstream) Coverage Status](https://coveralls.io/repos/github/systemd/systemd/badge.svg?branch=main)](https://coveralls.io/github/systemd/systemd?branch=main)</br>
[![(upstream) Packaging status](https://repology.org/badge/tiny-repos/systemd.svg)](https://repology.org/project/systemd/versions)</br>
[![(upstream) OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/systemd/systemd/badge)](https://securityscorecards.dev/viewer/?platform=github.com&org=systemd&repo=systemd)

## About This Fork

This fork adapts systemd to compile and run on **Android/Termux** (bionic libc) without a functioning systemd as PID 1.

Key changes:

- **Bionic libc support** — Support for bionic libc.

- **Hardcoded path replacement** — all hardcoded `/run/`, `/tmp/`, `/var/`, `/etc/`, `/etc/systemd/` paths replaced with compile‑time macros (`RUNSTATEDIR`, `SYSTEM_TMPDIR`, `LOCALSTATEDIR`, `SYSCONF_DIR`, `PKGSYSCONFDIR`) so paths resolve to Termux‑writable directories

### Build

```text
meson setup build \
  -Dlibc=bionic \
  -Dsbat-distro=android \
  -Dtmpdir=$TMPDIR \
  -Drunstatedir=$TMPDIR/run \
  -Dlocalstatedir=$PREFIX/var \
  -Dsysconfdir=$PREFIX/etc
meson compile -C build
```

### Cross build (Android/aarch64)

Bionic provides neither ucontext APIs nor gettext/libintl. Populate the
(gitignored) `sysroot/` directory with them once, then build with the NDK
cross file:

```text
fork-utils/bootstrap-android-sysroot.sh
meson setup build-android --cross-file fork-utils/android-cross.ini -Dlibc=bionic
meson compile -C build-android
```

Requires the Android NDK in `/opt/android-ndk` (override with `ANDROID_NDK=`,
API level with `ANDROID_API=`), plus git, curl, tar, meson, ninja, python3 and
GNU make. Re-run the bootstrap script whenever `sysroot/` is missing.

## Details

Most documentation is available on [systemd's website](https://systemd.io/).

Assorted, older, general information about systemd can be found in the [systemd Wiki](https://www.freedesktop.org/wiki/Software/systemd).

Information about build requirements is provided in the [README file](README).

Consult our [NEWS file](NEWS) for information about what's new in the most recent systemd versions.

Please see the [Code Map](docs/ARCHITECTURE.md) for information about this repository's layout and content.

Please see the [Hacking guide](docs/HACKING.md) for information on how to hack on systemd and test your modifications.

Please see our [Contribution Guidelines](docs/CONTRIBUTING.md) for more information about filing GitHub Issues and posting GitHub Pull Requests.

When preparing patches for systemd, please follow our [Coding Style Guidelines](docs/CODING_STYLE.md).

If you are looking for support, please contact our [mailing list](https://lists.freedesktop.org/mailman/listinfo/systemd-devel), join our [IRC channel #systemd on libera.chat](https://web.libera.chat/#systemd) or [Matrix channel](https://matrix.to/#/#systemd-project:matrix.org)

Stable branches with backported patches are available in the [stable repo](https://github.com/systemd/systemd-stable).

We have a security bug bounty program sponsored by the [Sovereign Tech Fund](https://www.sovereigntechfund.de/) hosted on [YesWeHack](https://yeswehack.com/programs/systemd-bug-bounty-program)

Repositories with distribution packages built from git main are [available on OBS](https://software.opensuse.org//download.html?project=system%3Asystemd&package=systemd),
and also repositories with [packages built from the latest stable release](https://software.opensuse.org//download.html?project=system%3Asystemd%3Astable&package=systemd)
