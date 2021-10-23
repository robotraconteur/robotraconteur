# For dist.yml github action

import shutil
import pathlib

asset_dirs = ["artifacts/main"]

pathlib.Path("distfiles").mkdir(exist_ok=True)

for asset_dir in asset_dirs:
    for fname in list(pathlib.Path(asset_dir).glob('**/*.whl')):
        if "linux" in str(fname):
            continue
        print(fname)
        dest = pathlib.Path(fname)
        shutil.copy(str(fname),"distfiles/" + dest.name)
