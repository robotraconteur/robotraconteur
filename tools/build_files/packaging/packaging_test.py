#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path
import string
import re
import os

_containers = {
    "gentoo": "docker.io/gentoo/stage3:latest"
}

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
            -DBUILD_GEN=ON
            -DBUILD_TESTING=OFF
            -DBUILD_DOCUMENTATION=OFF
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


def do_gen(args):

    if args.dist == "gentoo":
        do_gen_gentoo(args)
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

    if args.debian_dist and len(args.debian_dist) > 0:
        passthrough_args.append(f"--debian-dist={args.debian_dist}")

    if args.debian_version_suffix and len(args.debian_version_suffix) > 0:
        passthrough_args.append(f"--debian-version-suffix=${args.debian_version_suffix}")

    if args.package_version:
        passthrough_args.append(f"--package-version=${args.package_version}")

    if args.gentoo_local_src:
        passthrough_args.append(f"--gentoo-local-src")

    rm_str = ""
    if not args.no_rm:
        rm_str = "--rm"

    if not args.shell:
        subprocess.check_call(
            f"podman run {rm_str} -v {src_dir}:/src -v {__file__}:/packaging_test.py {container} python3 packaging_test.py podman-build-impl {' '.join(passthrough_args)}", shell=True)
    else:
        print(f"Command to run: python3 packaging_test.py podman-build-impl {' '.join(passthrough_args)}")
        subprocess.check_call(
            f"podman run {rm_str} -it -v {src_dir}:/src -v {__file__}:/packaging_test.py {container}", shell=True)


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
        subprocess.check_call("emerge --sync --quiet", shell=True)
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
        subprocess.check_call("USE='python' emerge dev-cpp/robotraconteur", shell=True)

    if "setup-test" in steps:
        subprocess.check_call("emerge --getbinpkg dev-cpp/gtest dev-python/pytest", shell=True)
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
    parser_gen.add_argument("--package-version", type=str, default=None, help="Package version")
    parser_gen.add_argument("--debian-dist", type=str, default="unstable", help="Debian target distribution")
    parser_gen.add_argument("--debian-version-suffix", type=str, default="", help="Debian package version suffix")
    parser_gen.add_argument("--gentoo-local-src", action="store_true", help="Use local src with gentoo")
    parser_gen.add_argument("--no-rm", action="store_true", help="Don't add --rm to podman command")
    parser_gen.add_argument("--shell", action="store_true", help="Start an interactive shell")

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
