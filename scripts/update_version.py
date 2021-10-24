from pathlib import Path
import argparse
import re
import urllib
import urllib.request
import hashlib
import os
from github import Github
import email


def main():
    
    parser = argparse.ArgumentParser(description="Update package new version")
    parser.add_argument("--src-repository", type=str, default=None, help="The source repository")
    parser.add_argument("--tag-name", type=str, default=None, help="The tag name. Must be semver based")
    parser.add_argument("--token-file", type=argparse.FileType("r"), help="Load token from file")

    args = parser.parse_args()

    assert Path("scripts/update_version.py").is_file(), "Must run script from repo root"

    token = None

    if args.token_file:        
        token = args.token_file.read().strip()

    if token is None:
        token = os.environ.get("BOT_GITHUB_TOKEN", None)

    assert token, "Invalid bot token"

    tag_name = args.tag_name
    if tag_name is None:
        tag_name = os.environ["INPUT_TAG_NAME"]

    semver_regex = r"^v((?:0|[1-9]\d*)\.(?:0|[1-9]\d*)\.(?:0|[1-9]\d*))(?:(-(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"

    tag_name_r = re.match(semver_regex, tag_name)

    if tag_name_r.group(2) is None:
        deb_ver = tag_name_r.group(1) + "-1"
        orig_ver = tag_name_r.group(1)
    else:
        deb_ver = tag_name_r.group(1) + tag_name_r.group(2).replace("-","~") + "-1"
        orig_ver = tag_name_r.group(1) + tag_name_r.group(2).replace("-","~")

    print(f"tag_name: {tag_name}")
    print(f"deb_ver: {deb_ver}")
    print(f"orig_ver: {deb_ver}")

    repo = args.src_repository
    if repo is None:
        repo = os.environ["INPUT_SOURCE_REPOSITORY"]

    github = Github(token)
    repo = github.get_repo(repo)
    releases = repo.get_releases()
    
    release = None

    for r in releases:
        if r.tag_name == tag_name:
            release = r
            break
    
    assert release, "Could not find release"

    release_assets = release.get_assets()

    release_assets2 = list(filter(lambda a: re.match(r"^RobotRaconteur\-.*\-Source\.tar\.gz$",a.name) is not None, release_assets))
    assert len(release_assets2) == 1, "Could not find release source tarball"
    a = release_assets2[0]
    print(a.url)
    
    old_orig = Path(".").glob("*.orig.tar.gz")
    for o in old_orig:
        print(f"deleting old orig {o}")
        o.unlink()

    download_asset(a.url, a.name, f"robotraconteur_{orig_ver}.orig.tar.gz", token)

    with open("upstream_tag_name", "w") as f:
        f.write(tag_name)

    dists_dir = Path("dists")

    for d in dists_dir.iterdir():
        distro = d.name
        print(f"Updating dist {distro}")

        datetime_now = email.utils.formatdate(localtime=True)

        changelog_text = (
            f"robotraconteur ({deb_ver}~{distro}) {distro}; urgency=medium\n\n"
            f"  * Version {tag_name} release\n\n"
            f" -- John Wason <wason@wasontech.com>  {datetime_now}\n"
        )

        changelog_p = Path(d.joinpath("debian/changelog"))
        with open(changelog_p, 'r+') as f:        
            content = f.read()
            assert f"robotraconteur ({deb_ver}~{distro})" not in content, "Already ran package build script!"
            f.seek(0, 0)
            f.write(changelog_text + "\n" + content)
        


def download_asset(asset_url, name, out_name, token):
    print("Retrieving release asset %s from url: %s" % (name,asset_url))
    
    headers={"Authorization": f"token {token}", "Accept": "application/octet-stream"}
    req = urllib.request.Request(asset_url, headers=headers)
    response = urllib.request.urlopen(req)
    headers = response.getheaders()
    with open(f"{out_name}", 'wb') as f:
        f.write(response.read())

if __name__ == "__main__":
    main()

