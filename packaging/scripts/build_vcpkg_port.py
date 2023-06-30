from pathlib import Path
import hashlib
import urllib.request
import sys
import re
import os
import shutil

ref = os.environ["GITHUB_SHA"]
ver = ref

ref1 = os.environ.get("GITHUB_REF", None)

if ref1 is not None:
    ref_match = re.match(r"refs/tags/v(\d+\.\d+\.\d+.*)", ref1)
    if ref_match is not None:
        ref = ref1
        ver = ref_match.group(1)

print(ver)

token = os.environ["GITHUB_TOKEN"]

tarball_url = f"https://github.com/robotraconteur/robotraconteur/archive/{ref}.tar.gz"

print("Begin download")

req = urllib.request.Request(tarball_url, headers={
                             "Accept": "application/octet-stream", "Authorization": f"token {token}"})
response = urllib.request.urlopen(req)
file_data = response.read()

print("End download")

tarball_sha512 = hashlib.sha512(file_data).hexdigest()

print(tarball_sha512)

template_dict = {"VERSION": ver, "VERSION_REF": ref,
                 "TARBALL_SHA512": tarball_sha512}

with open('robotraconteur/packaging/vcpkg/CONTROL.in') as f:
    control_input = f.read()

with open('robotraconteur/packaging/vcpkg/portfile.cmake.in') as f:
    portfile_input = f.read()

control_file = control_input.replace("@{VERSION}", ver)
print(control_file)
portfile_file = portfile_input.replace(
    "@{VERSION_REF}", ref).replace("@{TARBALL_SHA512}", tarball_sha512)
print(portfile_file)

Path('vcpkg-robotraconteur/ports/robotraconteur').mkdir(exist_ok=True, parents=True)

with open('vcpkg-robotraconteur/ports/robotraconteur/CONTROL', 'w') as f:
    f.write(control_file)

with open('vcpkg-robotraconteur/ports/robotraconteur/portfile.cmake', 'w') as f:
    f.write(portfile_file)

shutil.copy('robotraconteur/packaging/vcpkg/static-build.patch',
            'vcpkg-robotraconteur/ports/robotraconteur/static-build.patch')
