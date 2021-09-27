import shutil
import pathlib

asset_dirs = ["artifacts/main", "artifacts/build_python_version"]

pathlib.Path("distfiles").mkdir(exist_ok=True)

for asset_dir in asset_dirs:
    for fname in list(pathlib.Path(asset_dir).glob('**/RobotRaconteur-*-MATLAB*')):
        print(fname)
        dest = pathlib.Path(fname)
        shutil.copy(str(fname),"distfiles/" + dest.name)