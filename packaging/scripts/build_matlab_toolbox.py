from github import Github
import argparse
import os
from pathlib import Path
import shutil
import urllib
import subprocess
import re


def main():

    parser = argparse.ArgumentParser(
        description="Build Matlab toolbox and upload to release")
    parser.add_argument("--repository", type=str,
                        default=None, help="Repository to build for")
    parser.add_argument("--tag-name", type=str,
                        default=None, help="Release tag")
    parser.add_argument(
        "--token-file", type=argparse.FileType("r"), help="Load token from file")
    parser.add_argument("--matlab-dir", type=str,
                        default="/usr/local/MATLAB/R2020b", help="Matlab directory")
    parser.add_argument("--no-download", action="store_true")

    args = parser.parse_args()

    matlab_dir = Path(args.matlab_dir)
    matlab_exe = matlab_dir.joinpath("bin/matlab")
    assert matlab_exe.is_file()

    workdir = Path("build-matlab-toolbox").absolute()

    if not args.no_download:
        if workdir.is_dir():
            shutil.rmtree(workdir)

        workdir.mkdir()

    repo_path = args.repository

    if repo_path is None:
        repo_path = os.environ.get(
            "GITHUB_REPOSITORY", "robotraconteur/robotraconteur")

    token = None

    if args.token_file:
        token = args.token_file.read().strip()

    if token is None:
        token = os.environ.get("BOT_GITHUB_TOKEN", None)

    github = Github(token)
    repo = github.get_repo(repo_path)
    releases = repo.get_releases()

    release = None

    tag_name = args.tag_name
    input_tag_name = os.environ.get("INPUT_TAG_NAME", None)
    if input_tag_name is not None and len(input_tag_name) > 0:
        tag_name = input_tag_name
        print(f"Detected tag_name={tag_name} from input")

    for r in releases:
        if tag_name is None:
            if r.draft:
                release = r
                tag_name = r.tag_name
                break
        elif r.tag_name == tag_name:
            release = r
            break

    assert release, "Could not find release"

    print(release.tag_name)

    release_assets = release.get_assets()

    release_assets2 = filter(lambda a: "MATLAB" in a.name, release_assets)
    for a in release_assets2:
        print(a.name)
        print(a.browser_download_url)
        if not args.no_download:
            download_asset(a.url, a.name, token)

    build_dir = Path("build-matlab-toolbox/build")
    matlab_dir = build_dir.joinpath("matlab")
    matlab_dir.mkdir(exist_ok=True, parents=True)

    subprocess.check_call(
        "tar xf ../../RobotRaconteur-*-MATLAB-glnxa64.tar.gz --strip-components 1", shell=True, cwd=matlab_dir)
    subprocess.check_call(
        "tar xf ../../RobotRaconteur-*-MATLAB-maci64.tar.gz --strip-components 1 --wildcards RobotRaconteur-*-MATLAB-maci64/RobotRaconteurMex.*", shell=True, cwd=matlab_dir)
    subprocess.check_call(
        "unzip -j ../../RobotRaconteur-*-MATLAB-win64.zip RobotRaconteur-*-MATLAB-win64/RobotRaconteurMex.mexw64", shell=True, cwd=matlab_dir)

    semver_regex = r"^v((?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*))(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"

    print(tag_name)
    semver_match = re.match(semver_regex, tag_name)

    toolbox_ver = semver_match.group(1)

    with open("robotraconteur/packaging/matlab/RobotRaconteurMatlab.prj.in") as f1:
        s1 = f1.read()
    with open(build_dir.joinpath("RobotRaconteurMatlab.prj"), "w") as f2:
        f2.write(s1.replace("$(rr_version)", toolbox_ver))

    shutil.copy("robotraconteur/packaging/matlab/logo-icon.png", build_dir)

    subprocess.check_call(
        f"{matlab_exe} -nodesktop -batch \"matlab.addons.toolbox.packageToolbox('RobotRaconteurMatlab.prj','RobotRaconteurMatlab'); exit\"", shell=True, cwd=build_dir)

    shutil.move(f"{build_dir}/RobotRaconteurMatlab.mltbx",
                f"{build_dir}/RobotRaconteurMatlab-{toolbox_ver}.mltbx")

    release.upload_asset(f"{build_dir}/RobotRaconteurMatlab-{toolbox_ver}.mltbx",
                         content_type="application/octet-stream")


def download_asset(asset_url, name, token):
    print("Retrieving release asset %s from url: %s" % (name, asset_url))

    headers = {"Authorization": f"token {token}",
               "Accept": "application/octet-stream"}
    req = urllib.request.Request(asset_url, headers=headers)
    response = urllib.request.urlopen(req)
    headers = response.getheaders()
    with open(f"build-matlab-toolbox/{name}", 'wb') as f:
        f.write(response.read())


if __name__ == "__main__":
    main()
