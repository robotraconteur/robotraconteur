import sys
import shutil
import subprocess
import re
from pathlib import Path
import os
import glob
import argparse

def main():

    parser = argparse.ArgumentParser("Build MATLAB locally")
    parser.add_argument("--matlab-version", type=str, default="R2010a", help="MATLAB version to use")
    parser.add_argument("--matlab-root", type=str, default=None, help="MATLAB root location (default is searched)")
    parser.add_argument("--semver-full", type=str, default=None, help="Full semver of release")

    config_header = Path("RobotRaconteurCore\include\RobotRaconteur\RobotRaconteurConfig.h").absolute()
    assert config_header.is_file()

    build_path = Path("build").absolute()
    if build_path.is_dir():
        shutil.rmtree(str(build_path))

    vcpkg_path = Path("vcpkg").absolute()
    if vcpkg_path.is_dir():
        shutil.rmtree(str(vcpkg_path))

    vcpkg_triplet = "x64-windows-static-md"

    subprocess.check_call("git clone --depth=1 https://github.com/microsoft/vcpkg.git", shell=True)
    subprocess.check_call("bootstrap-vcpkg.bat", shell=True, cwd=vcpkg_path)
    subprocess.check_call(f"vcpkg install --triplet {vcpkg_triplet} boost-algorithm " \
        "boost-array boost-asio "\
        "boost-assign boost-atomic boost-bind boost-config boost-container boost-date-time " \
        "boost-smart-ptr boost-filesystem boost-foreach boost-format boost-function boost-interprocess " \
        "boost-intrusive boost-lexical-cast boost-locale boost-random boost-range boost-regex " \
        "boost-scope-exit boost-signals2 boost-thread boost-tuple boost-unordered " \
        "boost-utility boost-uuid boost-program-options", shell=True, cwd=vcpkg_path)

if __name__ == "__main__":
    main()