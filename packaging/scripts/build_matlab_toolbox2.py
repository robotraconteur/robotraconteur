import argparse
import os
from pathlib import Path
import shutil
import urllib
import subprocess
import re

asset_dirs = ["artifacts/main", "artifacts/build_python_version"]


def main():

    workdir = Path("build-matlab-toolbox").absolute()

    workdir.mkdir()

    with open("artifacts/main/out-matlab/rrversion.txt", "r") as f:
        tag_name = f.read().strip()

    print(f"tag_name: {tag_name}")

    Path("distfiles").mkdir(exist_ok=True)

    for asset_dir in asset_dirs:
        for fname in list(Path(asset_dir).glob('**/RobotRaconteur-*-MATLAB*')):
            if fname.is_dir():
                continue
            print(fname)
            dest = Path(fname)
            shutil.copy(str(fname), "distfiles/" + dest.name)

    build_dir = Path("build-matlab-toolbox/build")
    matlab_dir = build_dir.joinpath("matlab")
    matlab_dir.mkdir(exist_ok=True, parents=True)

    subprocess.check_call(
        "tar xf ../../../distfiles/RobotRaconteur-*-MATLAB-glnxa64.tar.gz --strip-components 1", shell=True, cwd=matlab_dir)
    subprocess.check_call(
        "tar xf ../../../distfiles/RobotRaconteur-*-MATLAB-maci64.tar.gz --strip-components 1 --wildcards RobotRaconteur-*-MATLAB-maci64/RobotRaconteurMex.*", shell=True, cwd=matlab_dir)
    subprocess.check_call(
        "tar xf ../../../distfiles/RobotRaconteur-*-MATLAB-maca64.tar.gz --strip-components 1 --wildcards RobotRaconteur-*-MATLAB-maca64/RobotRaconteurMex.*", shell=True, cwd=matlab_dir)
    subprocess.check_call(
        "unzip -j ../../../distfiles/RobotRaconteur-*-MATLAB-win64.zip RobotRaconteur-*-MATLAB-win64/RobotRaconteurMex.mexw64", shell=True, cwd=matlab_dir)

    semver_regex = r"^((?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*))(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"

    print(tag_name)
    semver_match = re.match(semver_regex, tag_name)

    toolbox_ver = semver_match.group(1)

    with open("robotraconteur/packaging/matlab/RobotRaconteurMatlab.prj.in") as f1:
        s1 = f1.read()
    with open(build_dir.joinpath("RobotRaconteurMatlab.prj"), "w") as f2:
        f2.write(s1.replace("$(rr_version)", toolbox_ver))

    shutil.copy("robotraconteur/packaging/matlab/logo-icon.png", build_dir)


if __name__ == "__main__":
    main()
