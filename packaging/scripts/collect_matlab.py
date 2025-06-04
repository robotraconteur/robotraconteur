import shutil
import pathlib

asset_dirs = ["artifacts/main", "artifacts/build_python_version"]
glob_patterns = [
    '**/RobotRaconteur-*-MATLAB*',
    '**/*.mltbx'
]

pathlib.Path("distfiles").mkdir(exist_ok=True)

for asset_dir in asset_dirs:
    for pattern in glob_patterns:
        for fname in list(pathlib.Path(asset_dir).glob(pattern)):
            if fname.is_dir():
                continue
            print(fname)
            dest = pathlib.Path(fname)
            shutil.copy(str(fname), "distfiles/" + dest.name)
