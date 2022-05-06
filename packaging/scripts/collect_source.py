import shutil
import pathlib

asset_dirs = ["artifacts/main/out-ubuntu-18.04"]

pathlib.Path("distfiles").mkdir(exist_ok=True)

for asset_dir in asset_dirs:
    for fname in list(pathlib.Path(asset_dir).glob('**/RobotRaconteur-*-Source*')):
        print(fname)
        dest = pathlib.Path(fname)
        shutil.copy(str(fname), "distfiles/" + dest.name)
