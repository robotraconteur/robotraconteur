#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path
import string
import re
import os
from datetime import datetime, timezone
import shutil
import pwd

_containers = {
    "debian": "docker.io/debian:sid",
    "gentoo": "docker.io/gentoo/stage3:latest",
    "fedora": "docker.io/fedora:latest",
    "arch": "docker.io/archlinux:latest"
}

# debian

_debian_control = \
    """\
Source: robotraconteur
Section: contrib/misc
Priority: optional
Maintainer: John Wason <wason@wasontech.com>
Build-Depends: cmake (>=3.5.1),
               debhelper (>=9),
               libbluetooth-dev,
               libboost-all-dev (>=1.58.0),
               libdbus-1-dev,
               libssl-dev (>=1),
               libusb-1.0-0-dev (>=1),
               zlib1g-dev,
               python3-dev,
               python3-numpy,
               python3-setuptools,
               libgtest-dev
Standards-Version: 4.7.0
Homepage: https://github.com/robotraconteur/robotraconteur

Package: librobotraconteurcore1
Architecture: any
Multi-Arch: same
Section: contrib/libs
Depends: ${misc:Depends}, ${shlibs:Depends}, libbluetooth3, libdbus-1-3, libusb-1.0-0
Pre-Depends: ${misc:Pre-Depends}
Description: Robot Raconteur is a communication framework for
 Robotics and Automation
 .
 This package provides run-time library of robotraconteur.

Package: librobotraconteur-dev
Architecture: any
Section: contrib/libdevel
Depends: librobotraconteurcore1, robotraconteurgen, ${misc:Depends}, ${shlibs:Depends}, libboost-all-dev (>=1.58.0)
Description: Robot Raconteur is a communication framework for
 Robotics and Automation
 .
 This package provides development files of robotraconteur.

Package: python3-robotraconteur
Architecture: any
Section: contrib/python
Depends: ${misc:Depends}, ${shlibs:Depends}, libbluetooth3,
         libdbus-1-3, libusb-1.0-0, python3-numpy, python3
Description: Robot Raconteur Python module
 Robot Raconteur Python module. Use with python 3

Package: robotraconteurgen
Architecture: any
Section: contrib/libdevel
Depends: librobotraconteurcore1, ${misc:Depends}, ${shlibs:Depends}
Description: Robot Raconteur is a communication framework for
 Robotics and Automation
 .
 This package provides the RobotRaconteurGen tool.
"""

_debian_copyright = \
    """\
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: robotraconteur
Source: http://github.com/robotraconteur/robotraconteur

Files: *
Copyright: 2018 John Wason (wason@wasontech.com)
License: Apache
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

Files: debian/*
Copyright: 2018 John Wason <wason@wasontech.com>
License: GPL-2+
 This package is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>
 .
 On Debian systems, the complete text of the GNU General
 Public License version 2 can be found in "/usr/share/common-licenses/GPL-2".
"""

_debian_python3_robotraconteur_install = \
    """\
/usr/lib/python3*/dist-packages/RobotRaconteur/*.py
/usr/lib/python3*/dist-packages/RobotRaconteur/__pycache__/*.pyc*
/usr/lib/python3*/dist-packages/RobotRaconteur/*.so
/usr/lib/python3*/dist-packages/RobotRaconteur-*.egg-info/*
"""

_debian_librobotraconteur_dev_install = \
    """\
/usr/include/RobotRaconteur.h
/usr/include/RobotRaconteur/*
/usr/lib/*/*.so
/usr/lib/*/cmake/
"""

_debian_librobotraconteurcore1_install = \
    """\
/usr/lib/*/*.so.*
"""

_debian_robotraconteurgen_install = \
    """\
/usr/bin/RobotRaconteurGen
/usr/share/man/*/robotraconteurgen*
"""

_debian_changelog = \
    """
robotraconteur (${TEMPLATE_VERSION}-${TEMPLATE_PACKAGE_VERSION}${TEMPLATE_DEBIAN_VERSION_SUFFIX}) ${TEMPLATE_DEBIAN_DIST}; urgency=medium

  * Test Build

 -- John Wason <wason@wasontech.com>  ${TEMPLATE_TIMESTAMP}
"""

_debian_rules = \
    """\
#!/usr/bin/make -f
# See debhelper(7) (uncomment to enable)
# output every command that modifies files on the build system.
#export DH_VERBOSE = 1

export DH_OPTIONS

include /usr/share/dpkg/architecture.mk

CPPFLAGS :=$(shell dpkg-buildflags --get CPPFLAGS)
CFLAGS   :=$(shell dpkg-buildflags --get CFLAGS)
CXXFLAGS :=$(shell dpkg-buildflags --get CXXFLAGS)
LDFLAGS  :=$(shell dpkg-buildflags --get LDFLAGS)

export DEB_HOST_MULTIARCH CPPFLAGS CFLAGS CXXFLAGS LDFLAGS

COMMON_CMAKE_FLAGS = \\
    -DCMAKE_INSTALL_PREFIX=/usr \\
	-DLIB_INSTALL_DIR=/usr/lib/$(DEB_HOST_MULTIARCH) \\
    -DCMAKE_LIBRARY_ARCHITECTURE="$(DEB_HOST_MULTIARCH)" \\
    -DBUILD_TESTING=$(if $(filter nocheck,$(DEB_BUILD_OPTIONS)),OFF,ON) \\
    -DBUILD_GEN=ON \\
    -DBUILD_PYTHON=OFF \\
    -DBUILD_PYTHON3=ON \\
    -DUSE_PREGENERATED_SOURCE=ON \\
    -DPREGENERATED_SOURCE_DIR:PATH=$(CURDIR)/generated_src \\
    -DPYTHON3_EXECUTABLE=/usr/bin/python3 \\
    -DINSTALL_PYTHON3=ON \\
    -DSETUPTOOLS_DEB_LAYOUT=ON \\
    -DBUILD_DOCUMENTATION=OFF \\
    -DROBOTRACONTEUR_TESTING_DISABLE_DISCOVERY_LOOPBACK=ON \\
    -DBUILD_SHARED_LIBS=ON \\
    -DROBOTRACONTEURCORE_SOVERSION_MAJOR_ONLY=ON \\
    -DCMAKE_SKIP_RPATH=ON \\
    -DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE=PRE_TEST


%:
	dh $@ --parallel

override_dh_auto_configure:
	dh_auto_configure -- $(COMMON_CMAKE_FLAGS)

override_dh_auto_build:
	LD_LIBRARY_PATH=$(CURDIR)/obj-$(DEB_HOST_GNU_TYPE)/out/lib:$$LD_LIBRARY_PATH dh_auto_build

override_dh_makeshlibs:
	LD_LIBRARY_PATH=$(CURDIR)/obj-$(DEB_HOST_GNU_TYPE)/out/lib:$$LD_LIBRARY_PATH dh_makeshlibs -- -c4

override_dh_auto_test:
	LD_LIBRARY_PATH=$(CURDIR)/obj-$(DEB_HOST_GNU_TYPE)/out/lib:$(CURDIR)/obj-$(DEB_HOST_GNU_TYPE)/test/out/lib:$$LD_LIBRARY_PATH dh_auto_test --no-parallel

"""


# gentoo

_gentoo_ebuild = \
    """
EAPI=8

PYTHON_COMPAT=( python3_{10..13} )


inherit cmake python-r1

DESCRIPTION="Robot Raconteur C++ library with Python bindings"
HOMEPAGE="https://github.com/robotraconteur/robotraconteur"
SRC_URI="${TEMPLATE_SRC_URI}"

LICENSE="Apache-2.0"
SLOT="0"
KEYWORDS="amd64 ~x86 ~arm arm64"
IUSE="python"

DEPEND="dev-libs/boost
    dev-libs/openssl
    dev-libs/libusb
    sys-apps/dbus
    net-wireless/bluez
    dev-build/cmake
    python? ( dev-python/numpy[$${PYTHON_USEDEP}] dev-python/setuptools[$${PYTHON_USEDEP}] dev-python/pip[$${PYTHON_USEDEP}] )
"
RDEPEND="$${DEPEND}"

S="$${WORKDIR}/RobotRaconteur-$${PV}-Source"

python_configure() {
    local mycmakeargs=(
        -DCMAKE_SKIP_RPATH=ON
        -DBUILD_GEN=ON
        -DBUILD_TESTING=OFF
        -DBUILD_DOCUMENTATION=OFF
        -DBUILD_PYTHON3=ON
        -DINSTALL_PYTHON3_PIP=ON
        -DINSTALL_PYTHON3_PIP_EXTRA_ARGS="--compile --use-pep517 --no-build-isolation --no-deps --root-user-action=ignore"
        -DROBOTRACONTEURCORE_SOVERSION_MAJOR_ONLY=ON
    )
    cmake_src_configure
}

src_configure() {
    if use python; then
        python_foreach_impl python_configure
    else
        local mycmakeargs=(
            -DCMAKE_SKIP_RPATH=ON
            -DBUILD_GEN=ON
            -DBUILD_TESTING=OFF
            -DBUILD_DOCUMENTATION=OFF
            -DROBOTRACONTEURCORE_SOVERSION_MAJOR_ONLY=ON
        )
        cmake_src_configure
    fi
}

src_compile() {
    if use python; then
        python_foreach_impl cmake_src_compile
    else
        cmake_src_compile
    fi
}

python_install(){
    cmake_src_install
    python_optimize "$${D}$$(python_get_sitedir)/RobotRaconteur" || die "Failed to optimize Python files"
}

src_install() {
    if use python; then
        python_foreach_impl python_install
    else
        cmake_src_install
    fi
}

${TEMPLATE_LOCAL_EXTRAS}

"""

_gentoo_github_src_uri = "https://github.com/robotraconteur/robotraconteur/releases/download/v${PV}/RobotRaconteur-${PV}-Source.tar.gz"

_gentoo_local_src = \
    """
# Remove for real use
src_unpack() {
    cp -r ${TEMPLATE_LOCAL_SRC_DIR} $${S} || die

}
"""

_gentoo_metadata = \
    """
<?xml version="1.0"?>
<pkgmetadata>
  <maintainer type="person">
    <name>John Wason</name>
    <email>wason@wasontech.com</email>
  </maintainer>
  <upstream>
    <remote-id type="github">robotraconteur/robotraconteur</remote-id>
    <bugs-to>https://github.com/robotraconteur/robotraconteur/issues</bugs-to>
    <doc>http://robotraconteur.com</doc>
  </upstream>
  <longdescription lang="en">
    Robot Raconteur C++ library with Python bindings. See http://robotraconteur.com for documentation.
  </longdescription>
  <use>
    <flag name="python" description="Enable Python bindings" default="false"/>
  </use>
</pkgmetadata>

"""

# rpm
_rpm_spec = \
    """\
Name:           robotraconteur
Version:        ${TEMPLATE_VERSION}
Release:        ${TEMPLATE_PACKAGE_VERSION}%{?dist}
Summary:        Robot Raconteur is a communication framework for Robotics and Automation

License:        Apache-2.0
URL:            https://github.com/robotraconteur/robotraconteur
Source0:        RobotRaconteur-%{version}-Source.tar.gz

BuildRequires:  cmake >= 3.5.1
BuildRequires:  boost-devel >= 1.58.0
BuildRequires:  bluez-libs-devel
BuildRequires:  dbus-devel
BuildRequires:  openssl-devel
BuildRequires:  libusb1-devel
BuildRequires:  zlib-devel
BuildRequires:  python3-devel
BuildRequires:  python3-numpy
BuildRequires:  python3-setuptools
BuildRequires:  python3-pip
BuildRequires:  gtest-devel
BuildRequires:  g++
BuildRequires:  gcc
BuildRequires:  make

Requires:       bluez-libs
Requires:       dbus
Requires:       libusb1
Requires:       python3-numpy
Requires:       python3

%description
Robot Raconteur is a communication framework for Robotics and Automation.

%package -n librobotraconteurcore1
Summary:        Robot Raconteur runtime library
Requires:       bluez-libs, dbus, libusb1

%description -n librobotraconteurcore1
This package provides the run-time library of Robot Raconteur.

%package -n librobotraconteur-devel
Summary:        Robot Raconteur development files
Requires:       librobotraconteurcore1, robotraconteurgen boost-devel >= 1.58.0, cmake, g++, gcc, make, openssl-devel

%description -n librobotraconteur-devel
This package provides development files for Robot Raconteur.

%package -n python3-robotraconteur
Summary:        Robot Raconteur Python 3 module
Requires:       bluez-libs, dbus, libusb1, python3-numpy, python3

%description -n python3-robotraconteur
Robot Raconteur Python module. Use with python 3.

%package -n robotraconteurgen
Summary:        RobotRaconteurGen tool
Requires:       librobotraconteurcore1

%description -n robotraconteurgen
This package provides the RobotRaconteurGen tool.

%prep
%autosetup -n RobotRaconteur-%{version}-Source

%build
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=%{_prefix} \\
         -DBUILD_GEN=ON \\
         -DBUILD_PYTHON=OFF \\
         -DBUILD_PYTHON3=ON \\
         -DUSE_PREGENERATED_SOURCE=ON \\
         -DPYTHON3_EXECUTABLE=%{__python3} \\
         -DINSTALL_PYTHON3_PIP=ON \\
         -DBUILD_DOCUMENTATION=OFF \\
         -DBUILD_SHARED_LIBS=ON \\
         -DROBOTRACONTEURCORE_SOVERSION_MAJOR_ONLY=ON \\
         -DCMAKE_SKIP_RPATH=ON \\
         -DROBOTRACONTEUR_TESTING_DISABLE_DISCOVERY_LOOPBACK=ON \\
         -DBUILD_TESTING=OFF \\
         -DINSTALL_PYTHON3_PIP_EXTRA_ARGS="--compile --no-build-isolation \\
            --no-deps --root-user-action=ignore"

make %{?_smp_mflags}

%install
cd build
make install DESTDIR=%{buildroot}

# Move files to match Fedora packaging guidelines if needed

%files -n librobotraconteurcore1
%license LICENSE.txt
%{_libdir}/libRobotRaconteurCore.so.*

%files -n librobotraconteur-devel
%license LICENSE.txt
%{_includedir}/RobotRaconteur.h
%{_includedir}/RobotRaconteur/
%{_libdir}/libRobotRaconteurCore.so
%{_libdir}/cmake/

%files -n python3-robotraconteur
%license LICENSE.txt
%{python3_sitearch}/RobotRaconteur/
%{python3_sitearch}/RobotRaconteur-*.dist-info/

%files -n robotraconteurgen
%license LICENSE.txt
%{_bindir}/RobotRaconteurGen
%{_mandir}/man1/robotraconteurgen*

%changelog
* ${TIMESTAMP} John Wason <wason@wasontech.com> - ${TEMPLATE_VERSION}-${TEMPLATE_PACKAGE_VERSION}
- Test RPM Build
"""

# pacman
_pacman_srcinfo = \
    """\
pkgbase = robotraconteur
	pkgdesc = Robot Raconteur is a communication framework for Robotics and Automation
	pkgver = %{TEMPLATE_VERSION}
	pkgrel = %{TEMPLATE_PACKAGE_VERSION}
	url = https://github.com/robotraconteur/robotraconteur
	arch = x86_64
    arch = aarch64
	license = Apache-2.0
	makedepends = cmake>=3.5.1
	makedepends = boost>=1.58.0
	makedepends = bluez-libs
	makedepends = dbus
	makedepends = openssl
	makedepends = libusb
	makedepends = zlib
	makedepends = python
	makedepends = python-numpy
	makedepends = python-setuptools
	makedepends = python-pip
	makedepends = gtest
	makedepends = base-devel
	source = RobotRaconteur-${pkgver}-Source.tar.gz
	sha256sums = SKIP

pkgname = librobotraconteurcore1
	pkgdesc = Robot Raconteur runtime library
	depends = bluez-libs
	depends = dbus
	depends = libusb

pkgname = librobotraconteur-devel
	pkgdesc = Robot Raconteur development files
	depends = librobotraconteurcore1
    depends = robotraconteurgen
	depends = boost>=1.58.0
	depends = cmake
	depends = base-devel
	depends = openssl

pkgname = python-robotraconteur
	pkgdesc = Robot Raconteur Python 3 module
	depends = bluez-libs
	depends = dbus
	depends = libusb
	depends = python
	depends = python-numpy

pkgname = robotraconteurgen
	pkgdesc = RobotRaconteurGen tool
	depends = librobotraconteurcore1
"""

_pacman_pkgbuild =\
    """\
# Maintainer: John Wason <wason@wasontech.com>
pkgbase=robotraconteur
pkgname=(librobotraconteurcore1 librobotraconteur-devel python-robotraconteur robotraconteurgen)
pkgver=%{TEMPLATE_VERSION}
pkgrel=%{TEMPLATE_PACKAGE_VERSION}
pkgdesc="Robot Raconteur is a communication framework for Robotics and Automation"
arch=('x86_64' 'aarch64')
url="https://github.com/robotraconteur/robotraconteur"
license=('Apache-2.0')
source=("RobotRaconteur-${pkgver}-Source.tar.gz")
sha256sums=('SKIP') # Replace with actual sha256sum for release
makedepends=(
  'cmake>=3.5.1'
  'boost>=1.58.0'
  'bluez-libs'
  'dbus'
  'openssl'
  'libusb'
  'zlib'
  'python'
  'python-numpy'
  'python-setuptools'
  'python-pip'
  'gtest'
  'base-devel'
)
_libdepends=('bluez-libs' 'dbus' 'libusb')
_pythondepends=('python' 'python-numpy')

prepare() {
  cd "RobotRaconteur-${pkgver}-Source"
  rm -rf build
  mkdir build
}

build() {
  cd "RobotRaconteur-${pkgver}-Source/build"
  cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_GEN=ON \
    -DBUILD_PYTHON=OFF \
    -DBUILD_PYTHON3=ON \
    -DUSE_PREGENERATED_SOURCE=ON \
    -DPYTHON3_EXECUTABLE=/usr/bin/python3 \
    -DINSTALL_PYTHON3_PIP=ON \
    -DBUILD_DOCUMENTATION=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DROBOTRACONTEURCORE_SOVERSION_MAJOR_ONLY=ON \
    -DCMAKE_SKIP_RPATH=ON \
    -DROBOTRACONTEUR_TESTING_DISABLE_DISCOVERY_LOOPBACK=ON \
    -DBUILD_TESTING=OFF \
    -DINSTALL_PYTHON3_PIP_EXTRA_ARGS="--compile --use-pep517 --no-build-isolation --no-deps --root-user-action=ignore"
  cmake --build . -- -j$(nproc)
}

package_librobotraconteurcore1() {
  pkgdesc="Robot Raconteur runtime library"
  depends=("${_libdepends[@]}")
  cd "$srcdir/RobotRaconteur-${pkgver}-Source/build"
  DESTDIR="$pkgdir" cmake --install .
  # Prune unrelated files
  find "$pkgdir/usr/lib" -maxdepth 1 -type d -name "python*" -exec rm -r {} +
  rm -rf "$pkgdir/usr/include" "$pkgdir/usr/lib/cmake" "$pkgdir/usr/bin" "$pkgdir/usr/share" "$pkgdir/usr/lib/pkgconfig"
  find "$pkgdir/usr/lib" -type f,l ! -name 'libRobotRaconteurCore.so.*' -delete
  install -Dm644 ../LICENSE.txt "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

package_librobotraconteur-devel() {
  pkgdesc="Robot Raconteur development files"
  depends=('librobotraconteurcore1' 'boost>=1.58.0' 'cmake' 'base-devel' 'openssl' 'robotraconteurgen')
  cd "$srcdir/RobotRaconteur-${pkgver}-Source/build"
  DESTDIR="$pkgdir" cmake --install .
  # Keep only headers, development symlinks, and cmake files
  find "$pkgdir/usr/lib" -maxdepth 1 -type d -name "python*" -exec rm -r {} +
  find "$pkgdir/usr/lib" -type f,l ! -name 'libRobotRaconteurCore.so' ! -name '*.cmake' -delete
  rm -rf "$pkgdir/usr/bin" "$pkgdir/usr/share" "$pkgdir/usr/lib/pkgconfig"
  find "$pkgdir/usr/lib" -type f,l -name 'libRobotRaconteurCore.so.*' -delete
  install -Dm644 ../LICENSE.txt "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

package_python-robotraconteur() {
  pkgdesc="Robot Raconteur Python 3 module"
  depends=("${_libdepends[@]}" "${_pythondepends[@]}")
  cd "$srcdir/RobotRaconteur-${pkgver}-Source/build"
  DESTDIR="$pkgdir" cmake --install .
  # Prune to only python site-packages
  pyver=$(python3 -c 'import sys; print("{}.{}".format(sys.version_info.major, sys.version_info.minor))')
  sitepkg="usr/lib/python${pyver}/site-packages"
  find "$pkgdir/usr" -mindepth 1 -maxdepth 1 ! -name "lib" -exec rm -rf {} +
  find "$pkgdir/usr/lib" -mindepth 1 -maxdepth 1 ! -name "python${pyver}" -exec rm -rf {} +
  install -Dm644 ../LICENSE.txt "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

package_robotraconteurgen() {
  pkgdesc="RobotRaconteurGen tool"
  depends=('librobotraconteurcore1')
  cd "$srcdir/RobotRaconteur-${pkgver}-Source/build"
  DESTDIR="$pkgdir" cmake --install .
  # Keep only the generator binary and man page (if present)
  find "$pkgdir/usr" -mindepth 1 -maxdepth 1 ! -name "bin" -a ! -name "share" -exec rm -rf {} +
  find "$pkgdir/usr/bin" -type f ! -name 'RobotRaconteurGen' -delete
  if [ -d "$pkgdir/usr/share/man/man1" ]; then
    find "$pkgdir/usr/share/man/man1" -type f ! -name 'robotraconteurgen*' -delete
  fi
  install -Dm644 ../LICENSE.txt "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
"""


def get_src_dir(src_dir_arg):
    if src_dir_arg is not None:
        if not Path(src_dir_arg).is_dir():
            raise Exception("Provided src dir is not valid")
        return Path(src_dir_arg).absolute()

    repo_src_dir = (Path(__file__).parent.parent.parent.parent).absolute()
    return repo_src_dir


def get_out_dir(out_dir_arg, out_dir_default_override=None):
    if out_dir_arg is not None:
        if not Path(out_dir_arg).is_dir():
            raise Exception("Provided out dir is not valid")
        return Path(out_dir_arg).absolute()

    if out_dir_default_override is not None:
        return Path(out_dir_default_override)

    repo_out_dir = (Path(__file__).parent.parent.parent.parent).absolute()
    return repo_out_dir


def get_version(src_dir, args_version):
    if args_version is not None:
        return args_version
    config_h_path = src_dir / "RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h"
    with open(config_h_path, "r") as f:
        config_h = f.read()

    config_h_ver_str_m = re.search(
        "ROBOTRACONTEUR_VERSION_TEXT \"(\\d+\\.\\d+\\.\\d+)\"", config_h)
    config_h_ver_str = config_h_ver_str_m.group(1)

    config_h_ver_regex = r"^((?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*))?"
    if not re.match(config_h_ver_regex, config_h_ver_str):
        raise Exception(f"Invalid config header file version {config_h_ver_str}")

    return config_h_ver_str


def do_gen_debian(args):
    src_dir = get_src_dir(args.src_dir)
    ver = get_version(src_dir, args.version)
    print(f"src_dir: {src_dir}")
    print(f"ver: {ver}")
    out_dir = get_out_dir(args.out_dir, src_dir)
    print(f"out_dir: {out_dir}")

    debian_dir = out_dir / "debian"
    debian_dir.mkdir(parents=True, exist_ok=True)
    (debian_dir / "source").mkdir(parents=True, exist_ok=True)

    data = {
        "TEMPLATE_TIMESTAMP": datetime.now(timezone.utc).strftime("%a, %d %b %Y %H:%M:%S +0000"),
        "TEMPLATE_VERSION": ver,
        "TEMPLATE_PACKAGE_VERSION": args.package_version or "1",
        "TEMPLATE_DEBIAN_VERSION_SUFFIX": args.debian_version_suffix,
        "TEMPLATE_DEBIAN_DIST": args.debian_dist
    }
    deb_changelog_filled = string.Template(_debian_changelog).substitute(data)

    deb_files_to_write = {
        "control": _debian_control,
        "python3-robotraconteur.install": _debian_python3_robotraconteur_install,
        "librobotraconteur-dev.install": _debian_librobotraconteur_dev_install,
        "librobotraconteurcore1.install": _debian_librobotraconteurcore1_install,
        "robotraconteurgen.install": _debian_robotraconteurgen_install,
        "changelog": deb_changelog_filled.strip(' \t\n\r') + "\n\n",
        "rules": _debian_rules,
        "source/format": "3.0 (quilt)\n",
        "compat": "10\n",
        "copyright": _debian_copyright
    }

    if args.stdout:
        for k, v in deb_files_to_write.items():
            print(f"=== debian/{k} ===")
            print(v)
            print("")
    else:
        for k, v in deb_files_to_write.items():
            print(f"writing file {str(debian_dir / k)}")
            with open(debian_dir / k, "w") as f:
                f.write(v)

            rules_path = debian_dir / "rules"
            if rules_path.exists():
                rules_path.chmod(0o755)


def do_gen_gentoo(args):
    src_dir = get_src_dir(args.src_dir)
    ver = get_version(src_dir, args.version)
    print(f"src_dir: {src_dir}")
    print(f"ver: {ver}")
    out_dir = get_out_dir(args.out_dir, "/var/db/repos/local")
    print(f"out_dir: {out_dir}")

    gentoo_metadata = _gentoo_metadata

    gentoo_ebuild_template = string.Template(_gentoo_ebuild)
    if not args.gentoo_local_src:
        data = {
            "TEMPLATE_SRC_URI": _gentoo_github_src_uri,
            "TEMPLATE_LOCAL_EXTRAS": ""
        }
    else:
        data = {
            "TEMPLATE_SRC_URI": "",
            "TEMPLATE_LOCAL_EXTRAS": string.Template(_gentoo_local_src).substitute({"TEMPLATE_LOCAL_SRC_DIR": str(args.gentoo_local_src)})
        }

    gentoo_ebuild = gentoo_ebuild_template.substitute(data)

    gentoo_ebuild_fname = f"robotraconteur-{ver}.ebuild"

    if args.stdout:
        print(f"=== {gentoo_ebuild_fname} ===")
        print(gentoo_ebuild)
        print("")

        print("=== metadata.xml ===")
        print(gentoo_metadata)
        print("")
    else:
        pkg_dir = out_dir / "dev-cpp/robotraconteur"

        pkg_dir.mkdir(parents=True, exist_ok=True)

        with open(pkg_dir / gentoo_ebuild_fname, "w") as f:
            f.write(gentoo_ebuild)

        with open(pkg_dir / "metadata.xml", "w") as f:
            f.write(gentoo_metadata)


def do_gen_fedora(args):
    src_dir = get_src_dir(args.src_dir)
    ver = get_version(src_dir, args.version)
    print(f"src_dir: {src_dir}")
    print(f"ver: {ver}")
    out_dir = get_out_dir(args.out_dir, Path.home() / "rpmbuild")
    print(f"out_dir: {out_dir}")

    rpm_spec_template = string.Template(_rpm_spec)
    timestamp = datetime.now().strftime("%a %b %d %Y")

    data = {
        "TEMPLATE_VERSION": ver,
        "TEMPLATE_PACKAGE_VERSION": args.package_version or "1",
        "TIMESTAMP": timestamp
    }

    rpm_spec = rpm_spec_template.substitute(data)

    if args.stdout:
        print(f"=== robotraconteur.spec ===")
        print(rpm_spec)
        print("")

    else:
        spec_dir = out_dir / "SPECS/"

        spec_dir.mkdir(parents=True, exist_ok=True)

        with open(spec_dir / "robotraconteur.spec", "w") as f:
            f.write(rpm_spec)


class PercentTemplate(string.Template):
    delimiter = "%"

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)


def do_gen_arch(args):
    src_dir = get_src_dir(args.src_dir)
    ver = get_version(src_dir, args.version)
    print(f"src_dir: {src_dir}")
    print(f"ver: {ver}")
    out_dir = get_out_dir(args.out_dir, "/testbuild")
    print(f"out_dir: {out_dir}")

    data = {
        "TEMPLATE_TIMESTAMP": datetime.now(timezone.utc).strftime("%a, %d %b %Y %H:%M:%S +0000"),
        "TEMPLATE_VERSION": ver,
        "TEMPLATE_PACKAGE_VERSION": args.package_version or "1",
    }

    deb_files_to_write = {
        ".SRCINFO": PercentTemplate(_pacman_srcinfo).substitute(data),
        "PKGBUILD": PercentTemplate(_pacman_pkgbuild).substitute(data)
    }

    if args.stdout:
        for k, v in deb_files_to_write.items():
            print(f"=== debian/{k} ===")
            print(v)
            print("")
    else:
        out_dir.mkdir(parents=True, exist_ok=True)
        for k, v in deb_files_to_write.items():
            print(f"writing file {str(out_dir / k)}")
            with open(out_dir / k, "w") as f:
                f.write(v)


def do_gen(args):

    if args.dist == "gentoo":
        do_gen_gentoo(args)
    elif args.dist == "debian":
        do_gen_debian(args)
    elif args.dist == "fedora":
        do_gen_fedora(args)
    elif args.dist == "arch":
        do_gen_arch(args)
    else:
        raise Exception(f"Unknown dist {args.dist}")


def do_podman_build(args):
    src_dir = get_src_dir(args.src_dir)
    if (args.container):
        container = args.container
    else:
        container = _containers[args.dist]

    passthrough_args = [
        "--src-dir=/src",
        f"--dist={args.dist}"
    ]

    podman_args = []

    if args.debian_dist and len(args.debian_dist) > 0:
        passthrough_args.append(f"--debian-dist={args.debian_dist}")

    if args.debian_version_suffix and len(args.debian_version_suffix) > 0:
        passthrough_args.append(f"--debian-version-suffix={args.debian_version_suffix}")

    if args.package_version:
        passthrough_args.append(f"--package-version={args.package_version}")

    if args.gentoo_local_src:
        passthrough_args.append(f"--gentoo-local-src")

    if args.version:
        passthrough_args.append(f"--version={args.version}")

    if args.out_dir:
        podman_args.append(f"-v {str(Path(args.out_dir).absolute())}:/out")
        Path(args.out_dir).absolute().mkdir(parents=True, exist_ok=True)

    rm_str = ""
    if not args.no_rm:
        rm_str = "--rm"

    if not args.shell:
        subprocess.check_call(
            f"podman run {rm_str} {' '.join(podman_args)} -v {src_dir}:/src -v {__file__}:/packaging_test.py -v {str(Path(__file__).parent / 'packaging_test_entry.sh')}:/packaging_test_entry.sh {container} /packaging_test_entry.sh podman-build-impl {' '.join(passthrough_args)}", shell=True)
    else:
        print(f"Command to run: python3 packaging_test.py podman-build-impl {' '.join(passthrough_args)}")
        subprocess.check_call(
            f"podman run {rm_str} -it {' '.join(podman_args)} -v {src_dir}:/src -v {__file__}:/packaging_test.py -v {str(Path(__file__).parent / 'packaging_test_entry.sh')}:/packaging_test_entry.sh {container}", shell=True)


def podman_build_copy_src():
    Path("/src2").mkdir(exist_ok=True)
    subprocess.check_call("git ls-files -z | xargs -0 -I{} cp --parents {} /src2/", shell=True, cwd="/src")
    subprocess.check_call("cp -r /src/generated_src /src2/", shell=True)


def do_podman_build_impl_gentoo(args):
    if args.steps is None or args.steps == "all":
        steps = ["setup", "gen", "build", "setup-test", "test"]
    else:
        steps = args.steps.split(",")

    if "setup" in steps:
        subprocess.check_call("gpg --no-tty --batch --import /usr/share/openpgp-keys/gentoo-release.asc", shell=True)
        subprocess.check_call("getuto")
        print("Begin emerge sync", flush=True)
        subprocess.check_call("emerge-webrsync --quiet", shell=True)
        subprocess.check_call("mkdir -p /etc/portage/repos.conf", shell=True)
        subprocess.check_call("cp /usr/share/portage/config/repos.conf /etc/portage/repos.conf/gentoo.conf", shell=True)
        subprocess.check_call(
            "emerge --getbinpkg app-eselect/eselect-repository dev-util/pkgdev dev-vcs/git", shell=True)
        subprocess.check_call("eselect repository create local", shell=True)
    if "gen" in steps:
        podman_build_copy_src()

        if not args.gentoo_local_src:
            subprocess.check_call("python3 /packaging_test.py gen --dist=gentoo --src-dir=/src2", shell=True)
        else:
            subprocess.check_call(
                "python3 /packaging_test.py gen --dist=gentoo --src-dir=/src2 --gentoo-local-src=/src2", shell=True)

    if "build" in steps:
        subprocess.check_call("ls /var/db/repos/local/dev-cpp/robotraconteur", shell=True)
        subprocess.check_call("USE='python' emerge --onlydeps --getbinpkg dev-cpp/robotraconteur", shell=True)
        subprocess.check_call("USE='python' emerge dev-cpp/robotraconteur", shell=True)

        if Path("/out").is_dir():
            Path("/out/dev-cpp/robotraconteur").mkdir(parents=True, exist_ok=True)
            subprocess.check_call(
                "cp /var/db/repos/local/dev-cpp/robotraconteur/*.ebuild /out/dev-cpp/robotraconteur/", shell=True)
            subprocess.check_call(
                "cp /var/db/repos/local/dev-cpp/robotraconteur/metadata.xml /out/dev-cpp/robotraconteur/", shell=True)

    if "setup-test" in steps:
        subprocess.check_call("emerge --getbinpkg dev-cpp/gtest dev-python/pytest", shell=True)
    if "test" in steps:
        subprocess.check_call(
            "cmake -S /src2 -B /build_test -DBUILD_CORE=OFF -DBUILD_GEN=OFF -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTEST_CORE=ON", shell=True)
        subprocess.check_call(f"cmake --build /build_test --config RelWithDebInfo -- -j{os.cpu_count()}", shell=True)
        subprocess.check_call(
            "ctest . -C RelWithDebInfo -E \"robotraconteur_test_discovery_loopback|RobotRaconteurService.DiscoveryLoopback\" --output-on-failure", cwd="build_test", shell=True)
        subprocess.check_call("pytest /src2/test/python/RobotRaconteurTest/test_service.py", shell=True)


def _save_installed_apt_packages():
    with open("/installed_packages.txt", "w") as f:
        subprocess.run(
            ["dpkg-query", "-f", "${binary:Package}\n", "-W"],
            stdout=f,
            check=True
        )


def _revert_to_saved_packages():
    if not Path("/installed_packages.txt").exists():
        print("No saved packages to revert to.")
        return
    with open("/installed_packages.txt") as f:
        keep = set(line.strip() for line in f)

    result = subprocess.run(
        ["dpkg-query", "-f", "${binary:Package}\n", "-W"],
        capture_output=True, text=True, check=True
    )
    current = set(result.stdout.strip().splitlines())

    to_remove = current - keep

    if to_remove:
        print("Packages to remove:", " ".join(to_remove))
        subprocess.run(["apt-get remove --purge -y"] + list(to_remove), shell=True)
    else:
        print("No extra packages to remove.")


def do_podman_build_impl_debian(args):
    if args.steps is None or args.steps == "all":
        steps = ["setup", "gen", "build", "setup-test", "test"]
    else:
        steps = args.steps.split(",")

    if "setup" in steps:
        _save_installed_apt_packages()
        subprocess.check_call("apt-get update", shell=True)
        # install debian build tools
        subprocess.check_call(
            "apt-get install -y devscripts equivs git tree git-buildpackage debian-keyring  debian-archive-keyring dh-make", shell=True)
    if "gen" in steps:
        podman_build_copy_src()
        subprocess.check_call("python3 /packaging_test.py gen --dist=debian --src-dir=/src2", shell=True)

    if "build" in steps:
        subprocess.check_call("mk-build-deps -i -r -t \"apt-get -y\" /src2/debian/control", shell=True, cwd="/src2")
        subprocess.check_call(
            "mk-build-deps --install --tool='apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends --yes' /src2/debian/control", shell=True, cwd="/src2")
        subprocess.check_call("debuild -us -uc -b", shell=True, cwd="/src2")

        if Path("/out").is_dir():
            Path("/out/debian").mkdir(parents=True, exist_ok=True)
            subprocess.check_call("cp /*.deb /out/debian/", shell=True)

    if "setup-test" in steps:
        _revert_to_saved_packages()
        subprocess.check_call("apt-get install -y libgtest-dev python3-pytest", shell=True)
        subprocess.call("dpkg -i /*.deb", shell=True)
        subprocess.check_call("apt-get install -f -y", shell=True)
    if "test" in steps:
        subprocess.check_call(
            "cmake -S /src2 -B /build_test -DBUILD_CORE=OFF -DBUILD_GEN=OFF -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTEST_CORE=ON", shell=True)
        subprocess.check_call(f"cmake --build /build_test --config RelWithDebInfo -- -j{os.cpu_count()}", shell=True)
        subprocess.check_call(
            "ctest . -C RelWithDebInfo -E \"robotraconteur_test_discovery_loopback|RobotRaconteurService.DiscoveryLoopback\" --output-on-failure", cwd="build_test", shell=True)
        subprocess.check_call("pytest /src2/test/python/RobotRaconteurTest/test_service.py", shell=True)


def _save_installed_rpm_packages():
    with open("/installed_packages.txt", "w") as f:
        subprocess.run(
            ["rpm", "-qa", "--qf", "%{NAME}\n"],
            stdout=f,
            check=True
        )


def _revert_to_saved_rpm_packages():
    if not Path("/installed_packages.txt").exists():
        print("No saved packages to revert to.")
        return
    with open("/installed_packages.txt") as f:
        keep = set(line.strip() for line in f)

    result = subprocess.run(
        ["rpm", "-qa", "--qf", "%{NAME}\n"],
        capture_output=True, text=True, check=True
    )
    current = set(result.stdout.strip().splitlines())

    to_remove = current - keep

    if to_remove:
        print("Packages to remove:", " ".join(to_remove))
        subprocess.run(["dnf", "remove", "-y"] + list(to_remove), check=True)
    else:
        print("No extra packages to remove.")


def do_podman_build_impl_fedora(args):
    if args.steps is None or args.steps == "all":
        steps = ["setup", "gen", "build", "setup-test", "test"]
    else:
        steps = args.steps.split(",")

    src_dir = get_src_dir(args.src_dir)
    ver = get_version(src_dir, None)

    if "setup" in steps:
        _save_installed_rpm_packages()
        subprocess.check_call("yum install rpmbuild git -y", shell=True)
        subprocess.check_call("dnf install dnf-plugins-core -y", shell=True)

    if "gen" in steps:
        podman_build_copy_src()
        subprocess.check_call(
            f"tar czvf RobotRaconteur-{ver}-Source.tar.gz --transform 's,^,RobotRaconteur-{ver}-Source/,' *", shell=True, cwd="/src2")
        sources_path = Path("/root/rpmbuild/SOURCES")
        sources_path.mkdir(parents=True, exist_ok=True)
        shutil.move(f"/src2/RobotRaconteur-{ver}-Source.tar.gz",
                    str(sources_path / f"RobotRaconteur-{ver}-Source.tar.gz"))
        subprocess.check_call("python3 /packaging_test.py gen --dist=fedora --src-dir=/src2", shell=True)

    if "build" in steps:
        subprocess.check_call("dnf builddep robotraconteur.spec -y", shell=True, cwd="/root/rpmbuild/SPECS")
        subprocess.check_call("rpmbuild -ba robotraconteur.spec", shell=True, cwd="/root/rpmbuild/SPECS")

        if Path("/out").is_dir():
            Path("/out/fedora").mkdir(parents=True, exist_ok=True)
            subprocess.check_call("cp /root/rpmbuild/RPMS/*/*.rpm /out/fedora/", shell=True)

    if "setup-test" in steps:
        _revert_to_saved_rpm_packages()
        subprocess.check_call("dnf install -y gtest-devel python3-pytest cmake g++ make", shell=True)
        rpm_fnames = []
        for p in Path("/root/rpmbuild/RPMS").rglob("*.rpm"):
            if "debug" in str(p):
                continue
            rpm_fnames.append(str(p))
        print(f"dnf install -y {' '.join(rpm_fnames)}")
        subprocess.check_call(f"dnf install -y {' '.join(rpm_fnames)}", shell=True, cwd="/root")
    if "test" in steps:
        subprocess.check_call(
            "cmake -S /src2 -B /build_test -DBUILD_CORE=OFF -DBUILD_GEN=OFF -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTEST_CORE=ON", shell=True)
        subprocess.check_call(f"cmake --build /build_test --config RelWithDebInfo -- -j{os.cpu_count()}", shell=True)
        subprocess.check_call(
            "ctest . -C RelWithDebInfo -E \"robotraconteur_test_discovery_loopback|RobotRaconteurService.DiscoveryLoopback\" --output-on-failure", cwd="build_test", shell=True)
        subprocess.check_call("pytest /src2/test/python/RobotRaconteurTest/test_service.py", shell=True)


def _user_exists(username):
    try:
        pwd.getpwnam(username)
        return True
    except KeyError:
        return False


def _add_user(username):
    subprocess.run(["useradd", "-m", username], check=True)
    subprocess.run(["passwd", "-d", username], check=True)
    subprocess.run(["usermod", "-aG", "wheel", username], check=True)


def _ensure_wheel_nopasswd():
    sudoers_line = "%wheel ALL=(ALL) NOPASSWD: ALL"
    with open("/etc/sudoers", "r") as f:
        if sudoers_line in f.read():
            return
    with open("/etc/sudoers", "a") as f:
        f.write(sudoers_line + "\n")


def _save_installed_pacman_packages():
    with open("/installed_packages.txt", "w") as f:
        subprocess.run(
            ["pacman", "-Qq"],
            stdout=f,
            check=True
        )


def _revert_to_saved_pacman_packages():
    if not Path("/installed_packages.txt").exists():
        print("No saved packages to revert to.")
        return
    with open("/installed_packages.txt") as f:
        keep = set(line.strip() for line in f)

    result = subprocess.run(
        ["pacman", "-Qq"],
        capture_output=True, text=True, check=True
    )
    current = set(result.stdout.strip().splitlines())

    to_remove = current - keep

    if to_remove:
        print("Packages to remove:", " ".join(to_remove))
        subprocess.run(["pacman", "-Rns", "--noconfirm"] + list(to_remove), check=True)
    else:
        print("No extra packages to remove.")


def do_podman_build_impl_arch(args):
    username = "testuser"
    if args.steps is None or args.steps == "all":
        steps = ["setup", "gen", "build", "setup-test", "test"]
    else:
        steps = args.steps.split(",")

    src_dir = get_src_dir(args.src_dir)
    ver = get_version(src_dir, None)
    out_dir = Path("/testbuild")
    out_dir.mkdir(parents=True, exist_ok=True)

    if "setup" in steps:
        subprocess.check_call("pacman -S --needed --noconfirm base-devel sudo git", shell=True)
        if not _user_exists(username):
            _add_user(username)
        _ensure_wheel_nopasswd()
        _save_installed_pacman_packages()

    if "gen" in steps:
        podman_build_copy_src()
        subprocess.check_call(
            f"tar czvf RobotRaconteur-{ver}-Source.tar.gz --transform 's,^,RobotRaconteur-{ver}-Source/,' *", shell=True, cwd="/src2")

        shutil.move(f"/src2/RobotRaconteur-{ver}-Source.tar.gz",
                    str(out_dir / f"RobotRaconteur-{ver}-Source.tar.gz"))
        subprocess.check_call("python3 /packaging_test.py gen --dist=arch --src-dir=/src2", shell=True)

    if "build" in steps:
        subprocess.check_call(f"chown {username} /testbuild", shell=True)
        subprocess.check_call(
            f"su - {username} -c \"export MAKEFLAGS='-j$(nproc)'; cd {str(out_dir)} && makepkg -s --noconfirm\"", shell=True, cwd=out_dir)

        if Path("/out").is_dir():
            Path("/out/arch").mkdir(parents=True, exist_ok=True)
            subprocess.check_call("cp /testbuild/*.pkg.tar.zst /out/arch/", shell=True)

    if "setup-test" in steps:
        _revert_to_saved_pacman_packages()
        subprocess.check_call("pacman -S --noconfirm gtest python-pytest", shell=True)
        rpm_fnames = []
        for p in Path(out_dir).rglob("*.pkg.tar.zst"):
            if "debug" in str(p):
                continue
            rpm_fnames.append(str(p))
        print(f"pacman -U --noconfigm {' '.join(rpm_fnames)}")
        subprocess.check_call(f"pacman -U --noconfirm {' '.join(rpm_fnames)}", shell=True, cwd=out_dir)
    if "test" in steps:
        subprocess.check_call(
            "cmake -S /src2 -B /build_test -DBUILD_CORE=OFF -DBUILD_GEN=OFF -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DTEST_CORE=ON", shell=True)
        subprocess.check_call(f"cmake --build /build_test --config RelWithDebInfo -- -j{os.cpu_count()}", shell=True)
        subprocess.check_call(
            "ctest . -C RelWithDebInfo -E \"robotraconteur_test_discovery_loopback|RobotRaconteurService.DiscoveryLoopback\" --output-on-failure", cwd="build_test", shell=True)
        subprocess.check_call("pytest /src2/test/python/RobotRaconteurTest/test_service.py", shell=True)


def do_podman_build_impl(args):
    if args.dist == "gentoo":
        do_podman_build_impl_gentoo(args)
    elif args.dist == "debian":
        do_podman_build_impl_debian(args)
    elif args.dist == "fedora":
        do_podman_build_impl_fedora(args)
    elif args.dist == "arch":
        do_podman_build_impl_arch(args)
    else:
        raise Exception(f"Unknown dist {args.dist}")


def main():
    parser = argparse.ArgumentParser(prog="packaging_test.py")

    subparsers = parser.add_subparsers(help='subcommand help', dest="command")

    parser_gen = subparsers.add_parser("gen", help="generate packaging files")
    parser_gen.add_argument("--dist", type=str, default="debian", help="Linux target dist")
    parser_gen.add_argument("--src-dir", type=str, default=None, help="Directory of source")
    parser_gen.add_argument("--out-dir", type=str, default=None, help="Output directory")
    parser_gen.add_argument("--version", type=str, default=None, help="Package version")
    parser_gen.add_argument("--package-version", type=str, default=None, help="Package version")
    parser_gen.add_argument("--stdout", action="store_true", help="Send generated files to stdout")
    parser_gen.add_argument("--debian-dist", type=str, default="unstable", help="Debian target distribution")
    parser_gen.add_argument("--debian-version-suffix", type=str, default="", help="Debian package version suffix")
    parser_gen.add_argument("--gentoo-local-src", type=str, default=None, help="Use local src with gentoo")

    parser_gen = subparsers.add_parser("podman-build")
    parser_gen.add_argument("--dist", type=str, default="debian", help="Linux target dist")
    parser_gen.add_argument("--container", type=str, default=None, help="podman container name")
    parser_gen.add_argument("--src-dir", type=str, default=None, help="Directory of source")
    parser_gen.add_argument("--version", type=str, default=None, help="Package version")
    parser_gen.add_argument("--package-version", type=str, default=None, help="Package version")
    parser_gen.add_argument("--debian-dist", type=str, default="unstable", help="Debian target distribution")
    parser_gen.add_argument("--debian-version-suffix", type=str, default="", help="Debian package version suffix")
    parser_gen.add_argument("--gentoo-local-src", action="store_true", help="Use local src with gentoo")
    parser_gen.add_argument("--no-rm", action="store_true", help="Don't add --rm to podman command")
    parser_gen.add_argument("--shell", action="store_true", help="Start an interactive shell")
    parser_gen.add_argument("--out-dir", type=str, default=None, help="Output directory for packages")

    parser_gen = subparsers.add_parser("podman-build-impl")
    parser_gen.add_argument("--dist", type=str, default="debian", help="Linux target dist")
    parser_gen.add_argument("--src-dir", type=str, default=None, help="Directory of source")
    parser_gen.add_argument("--package-version", type=str, default=None, help="Package version")
    parser_gen.add_argument("--debian-dist", type=str, default="unstable", help="Debian target distribution")
    parser_gen.add_argument("--debian-version-suffix", type=str, default="", help="Debian package version suffix")
    parser_gen.add_argument("--gentoo-local-src", action="store_true", help="Use local src with gentoo")
    parser_gen.add_argument("--steps", type=str, default="all", help="Steps to run. Defaults to \"all\"")

    args = parser.parse_args()

    if args.command == "gen":
        do_gen(args)
    elif args.command == "podman-build":
        do_podman_build(args)
    elif args.command == "podman-build-impl":
        do_podman_build_impl(args)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
