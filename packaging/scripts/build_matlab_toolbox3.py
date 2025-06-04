import argparse
import os
from pathlib import Path
import shutil
import urllib
import subprocess
import re


def main():

    with open("artifacts/main/out-matlab/rrversion.txt", "r") as f:
        tag_name = f.read().strip()

    print(f"tag_name: {tag_name}")

    build_dir = Path("build-matlab-toolbox/build")
    matlab_dir = build_dir.joinpath("matlab")

    semver_regex = r"^((?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*))(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"

    semver_match = re.match(semver_regex, tag_name)

    toolbox_ver = semver_match.group(1)

    shutil.move(f"{build_dir}/RobotRaconteurMatlab.mltbx",
                f"{build_dir}/RobotRaconteurMatlab-{toolbox_ver}.mltbx")


if __name__ == "__main__":
    main()
