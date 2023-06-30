import subprocess
from pathlib import Path
import re
import shutil
import platform

ios_triplets = [
    "arm-ios",
    "arm64-ios",
    "x64-ios"
]


archs = {
    "arm-ios": "armv7",
    "arm64-ios": "arm64",
    "x86-ios": "x86",
    "x64-ios": "x86_64",
    "arm64-iphonesimulator": "arm64"
}

src_root = Path(__file__).parent.parent.parent.absolute()

# Make sure we are in the right place!
rr_header = src_root / "RobotRaconteurCore/include/RobotRaconteur/RobotRaconteurConfig.h"
with open(rr_header) as f:
    f1 = f.read()
ver_str_m = re.search(
    "ROBOTRACONTEUR_VERSION_TEXT \"(\\d+\\.\\d+\\.\\d+)\"", f1)
ver_str = ver_str_m.group(1)
print("version " + ver_str)

build_dir = src_root / "build_xcframework"
build_dir.mkdir(exist_ok=True)

vcpkg_dir = build_dir / "vcpkg"

if not vcpkg_dir.is_dir():
    subprocess.check_call(["git", "clone", "--depth=1",
                          "https://github.com/microsoft/vcpkg.git"], cwd=build_dir)

if not (vcpkg_dir / "vcpkg").is_file():
    subprocess.check_call("./bootstrap-vcpkg.sh", cwd=vcpkg_dir)

# shutil.copy(src_root / "packaging/ios/CMakeLists-openssl-unix.txt",
#     build_dir / "vcpkg/ports/openssl/unix/CMakeLists.txt")

triplets_dir = src_root / "packaging/ios/triplets"
ports_in_dir = src_root / "packaging/ios/ports"
ports_dir = build_dir / "ports"
ports_rr_dir = ports_dir / "robotraconteur"
ports_rr_dir.mkdir(exist_ok=True, parents=True)

with open(ports_in_dir / "robotraconteur/vcpkg.json.in") as f:
    vcpkg_json_input = f.read()

vcpkg_json_file = vcpkg_json_input.replace("@{VERSION}", ver_str)
print(vcpkg_json_file)
with open(ports_rr_dir / "vcpkg.json", "w") as f:
    f.write(vcpkg_json_file)
shutil.copy(ports_in_dir / "robotraconteur/portfile.cmake",
            ports_rr_dir / "portfile.cmake")

combined_lib_dir = build_dir / "combined_lib"
combined_lib_dir_tmp = combined_lib_dir / "tmp"
combined_lib_dir.mkdir(exist_ok=True, parents=True)
combined_lib_dir_tmp.mkdir(exist_ok=True, parents=True)

for triplet in ios_triplets:
    subprocess.check_call(["./vcpkg", "install", f"--triplet={triplet}",
                           f"--overlay-ports={ports_dir}", f"--overlay-triplets={triplets_dir}", "robotraconteur"], cwd=vcpkg_dir)

    combined_lib_arch = archs[triplet]
    combined_lib_path = (combined_lib_dir_tmp /
                         f"robotraconteur_and_deps-{triplet}.a").absolute()
    subprocess.check_call([f"libtool -static -o {combined_lib_path} *.a"],
                          shell=True, cwd=vcpkg_dir / f"installed/{triplet}/debug/lib")

combined_lipo_ios = ("arm64-ios", "arm-ios")


combined_lipo_sim = ("x64-ios",)


lib_args = " ".join(
    [f"robotraconteur_and_deps-{c1}.a" for c1 in combined_lipo_ios])
subprocess.check_call(
    f"lipo {lib_args} -create -output ../robotraconteur_and_deps-ios.a", shell=True, cwd=combined_lib_dir_tmp)
sim_lib_args = " ".join(
    [f"robotraconteur_and_deps-{c1}.a" for c1 in combined_lipo_sim])
subprocess.check_call(
    f"lipo {sim_lib_args} -create -output ../robotraconteur_and_deps-iphonesimulator.a", shell=True, cwd=combined_lib_dir_tmp)

(build_dir / "robotraconteur.xcframework/ios").mkdir(exist_ok=True, parents=True)
(build_dir / "robotraconteur.xcframework/ios-simulator").mkdir(exist_ok=True, parents=True)

with open(src_root / "packaging/ios/Info.plist", "r") as f:
    plist_in = f.read()

if 'x64-ios' in triplet:
    plist = plist_in.replace(
        "<!--<string>x86_64</string>-->", "<string>x86_64</string>")
else:
    plist = plist_in.replace(
        "<!--<string>arm64</string>-->", "<string>arm64</string>")

with open(build_dir / "robotraconteur.xcframework/Info.plist", "w") as f:
    f.write(plist)

# shutil.copy(src_root / "packaging/ios/Info.plist", build_dir / "robotraconteur.xcframework/Info.plist")
