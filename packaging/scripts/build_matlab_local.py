import sys
import shutil
import subprocess
import re
from pathlib import Path
import os
import glob
import argparse

def main():

    parser = argparse.ArgumentParser(description="Build MATLAB locally")
    parser.add_argument("--matlab-version", type=str, default="R2020a", help="MATLAB version to use")
    parser.add_argument("--matlab-root", type=str, default=None, help="MATLAB root location (default is searched)")
    parser.add_argument("--semver-full", type=str, default=None, help="Full semver of release")
    parser.add_argument("--no-vcpkg", action='store_true')
    parser.add_argument("--no-clean", action='store_true')

    args = parser.parse_args()

    config_header = Path("RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h").absolute()
    assert config_header.is_file()

    if args.semver_full is not None:
        ver_str = args.semver_full
        semver_tag_regex = r"^(((?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*))(-(?:alpha|beta|rc)\d+)?)"
        m = re.match(semver_tag_regex,ver_str)
        assert m, f"Invalid semver-full {ver_str}"
    else:
        with open(config_header) as f:
            f1 = f.read()
        ver_str_m = re.search("ROBOTRACONTEUR_VERSION_TEXT \"(\\d+\\.\\d+\\.\\d+)\"", f1)
        ver_str = ver_str_m.group(1)
        print("version " + ver_str)

    build_path = Path("build").absolute()

    if not args.no_clean:
        if build_path.is_dir():
            shutil.rmtree(str(build_path))

    if sys.platform == "win32":
        vcpkg_triplet = "x64-windows-static-md"
    elif sys.platform == "darwin":
        vcpkg_triplet = "x64-osx"
    else:
        vcpkg_triplet = "x64-linux"

    vcpkg_path = Path("vcpkg").absolute()
    if not args.no_vcpkg:        

        vcpkg_libs = "boost-algorithm " \
                     "boost-array boost-asio "\
                     "boost-assign boost-atomic boost-bind boost-config boost-container boost-date-time " \
                     "boost-smart-ptr boost-filesystem boost-foreach boost-format boost-function boost-interprocess " \
                     "boost-intrusive boost-lexical-cast boost-locale boost-random boost-range boost-regex " \
                     "boost-scope-exit boost-signals2 boost-thread boost-tuple boost-unordered " \
                     "boost-utility boost-uuid boost-program-options"

        if vcpkg_path.is_dir():
            assert vcpkg_path.joinpath(".git").is_dir()
            subprocess.check_call("git pull", shell=True, cwd=vcpkg_path)
        else:
            subprocess.check_call("git clone --depth=1 https://github.com/microsoft/vcpkg.git", shell=True)
        if sys.platform == "win32":
            subprocess.check_call("bootstrap-vcpkg.bat", shell=True, cwd=vcpkg_path)
            subprocess.check_call(f"vcpkg install --triplet {vcpkg_triplet} {vcpkg_libs}" , shell=True, cwd=vcpkg_path)
        else:
            subprocess.check_call("./bootstrap-vcpkg.sh", shell=True, cwd=vcpkg_path)
            subprocess.check_call(f"./vcpkg install --triplet {vcpkg_triplet} {vcpkg_libs}" , shell=True, cwd=vcpkg_path)

    matlab_root = None

    if args.matlab_root is not None:
        matlab_root = Path(args.matlab_root)
    else:
        matlab_ver = args.matlab_version

        assert re.match(r"^R\d{4}[a-z]$", matlab_ver)

        if sys.platform == "win32":
            import winreg
            mw_key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, f"SOFTWARE\\MathWorks\\{matlab_ver}\\MATLAB")
            matlab_path1 = winreg.QueryValue(mw_key,None)
            mw_key.Close()
            matlab_path_m = re.match(r"^(.*)\\bin\\.*$", matlab_path1)
            assert matlab_path_m
            matlab_path = Path(matlab_path_m.group(1))
            assert matlab_path.joinpath("bin/matlab.exe").exists()
        elif sys.platform == "darwin":
            matlab_path = Path(f"/Applications/MATLAB_{matlab_ver}.app")
            assert matlab_path.joinpath("bin/matlab").exists()
        else:
            matlab_path = Path(os.environ("HOME")).joinpath(f"/MATLAB/{matlab_ver}")
            assert matlab_path.joinpath("bin/matlab").exists()

    vcpkg_toolchain_file = vcpkg_path.joinpath("scripts/buildsystems/vcpkg.cmake").absolute()
    assert vcpkg_toolchain_file.exists()
    
    subprocess.check_call("cmake -G \"Ninja\" -DBUILD_GEN=ON -DBUILD_TEST=ON -DBoost_USE_STATIC_LIBS=ON " \
      "-DCMAKE_BUILD_TYPE=Release -DBUILD_MATLAB_MEX=ON Boost_NO_SYSTEM_PATHS=ON " \
      f"-DVCPKG_TARGET_TRIPLET={vcpkg_triplet} " \
      f"-DCMAKE_TOOLCHAIN_FILE={vcpkg_toolchain_file} " \
      f"-DROBOTRACONTEUR_VERSION_SEMVER=\"{ver_str}\" "\
      "-S . -B build", shell=True)

    subprocess.check_call("cmake --build . --config Release", shell=True, cwd=build_path)
    
    if sys.platform == "linux":
        machine="glnxa64"
    elif sys.platform == "darwin":
        machine="maci64"
    elif sys.platform=="win32":
        machine="win64"
    else:
        machine=f"UNKNOWN-{sys.platform}"
    
    try:
        os.unlink(glob.glob(f"build/out/Matlab/*.lib")[0])
        os.unlink(glob.glob(f"build/out/Matlab/*.exp")[0])
        os.unlink(glob.glob(f"build/out/Matlab/*.pdb")[0])
    except:
        pass

    shutil.move(glob.glob("build/out/Matlab")[0],f"build/out/RobotRaconteur-{ver_str}-MATLAB-{machine}")
    shutil.copy("LICENSE.txt", f"build/out/RobotRaconteur-{ver_str}-MATLAB-{machine}")

    if sys.platform == "win32":
        subprocess.check_call(["zip", f"RobotRaconteur-{ver_str}-MATLAB-{machine}.zip", "-FSr", f"RobotRaconteur-{ver_str}-MATLAB-{machine}"], cwd="build/out")
    else:
        subprocess.check_call(["tar" ,"cvzf", f"RobotRaconteur-{ver_str}-MATLAB-{machine}.tar.gz", f"RobotRaconteur-{ver_str}-MATLAB-{machine}"], cwd="build/out")


if __name__ == "__main__":
    main()