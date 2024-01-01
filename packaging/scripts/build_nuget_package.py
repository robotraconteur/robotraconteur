import shutil
import subprocess
from pathlib import Path
import glob
import tempfile

asset_dir = "artifacts/main"

with open(asset_dir + '/out-win-x86/rrversion.txt') as f:
    ver_str = f.read().strip()

with open('robotraconteur/packaging/nuget/nuspec.in') as f:
    nuspec_input = f.read()

nuspec_file = nuspec_input.replace("@{VERSION}", ver_str)
print(nuspec_file)

with tempfile.TemporaryDirectory() as tmpdirname:

    tmpdir = Path(tmpdirname)

    with open(tmpdir.joinpath('.nuspec'), 'w') as f:
        f.write(nuspec_file)

    win32dir = f'{asset_dir}/out-win-x86'
    win64dir = f'{asset_dir}/out-win-x64'

    tmpdir.joinpath('build/net45').mkdir(exist_ok=True, parents=True)
    tmpdir.joinpath('build/netstandard2.0').mkdir(exist_ok=True, parents=True)
    tmpdir.joinpath('lib/net45').mkdir(exist_ok=True, parents=True)
    tmpdir.joinpath('lib/netstandard2.0').mkdir(exist_ok=True, parents=True)
    tmpdir.joinpath(
        'runtimes/win-x64/native').mkdir(exist_ok=True, parents=True)
    tmpdir.joinpath(
        'runtimes/win-x86/native').mkdir(exist_ok=True, parents=True)
    tmpdir.joinpath('tools').mkdir(exist_ok=True, parents=True)

    shutil.copyfile('robotraconteur/packaging/nuget/RobotRaconteurNET-net45.targets.in',
                    tmpdir.joinpath('build/net45/RobotRaconteurNET.targets'))
    shutil.copyfile('robotraconteur/packaging/nuget/RobotRaconteurNET-netstandard2.0.targets.in',
                    tmpdir.joinpath('build/netstandard2.0/RobotRaconteurNET.targets'))

    shutil.copyfile(f'{win32dir}/NET/RobotRaconteurNET.dll',
                    tmpdir.joinpath('lib/net45/RobotRaconteurNET.dll').absolute())
    shutil.copyfile(f'{win64dir}/NET/netstandard/RobotRaconteurNET.dll',
                    tmpdir.joinpath('lib/netstandard2.0/RobotRaconteurNET.dll').absolute())
    shutil.copyfile(f'{win32dir}/NET/Native/RobotRaconteurNETNative.dll',
                    tmpdir.joinpath('runtimes/win-x86/native/RobotRaconteurNETNative.dll').absolute())
    shutil.copyfile(f'{win64dir}/NET/Native/RobotRaconteurNETNative.dll',
                    tmpdir.joinpath('runtimes/win-x64/native/RobotRaconteurNETNative.dll').absolute())
    shutil.copyfile(f'{win32dir}/bin/RobotRaconteurGen.exe',
                    tmpdir.joinpath('tools/RobotRaconteurGen.exe').absolute())

    subprocess.check_call('nuget pack', cwd=tmpdir, shell=True)

    for file in glob.glob(str(tmpdir.joinpath('*.nupkg').absolute())):
        print(file)
        shutil.move(file, 'distfiles/')
