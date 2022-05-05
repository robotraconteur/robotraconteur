import zipfile
import sys
from pathlib import Path
import shutil
import yaml
import os
import re

asset_dir = f'artifacts/main'

with open(asset_dir + '/out-win-x86/rrversion.txt') as f:
    ver_str = f.read().strip()

with zipfile.ZipFile('distfiles/RobotRaconteur-Java-%s-Java.zip' % ver_str, 'w', zipfile.ZIP_DEFLATED) as zip:
    zip.write(asset_dir + "/out-win-x86/Java/RobotRaconteur.jar",
              arcname="RobotRaconteur.jar")
    zip.write(asset_dir + "/out-win-x86/Java/RobotRaconteurJavaNative-win32.dll",
              arcname="RobotRaconteurJavaNative-win32.dll")
    zip.write(asset_dir + "/out-win-x86/bin/RobotRaconteurGen.exe",
              arcname="RobotRaconteurGen.exe")
    zip.write(asset_dir + "/out-win-x86/LICENSE.txt", arcname="LICENCE.txt")
    zip.write(asset_dir + "/out-win-x64/Java/RobotRaconteurJavaNative-win64.dll",
              arcname="RobotRaconteurJavaNative-win64.dll")
    zip.write(asset_dir + "/out-osx/Java/libRobotRaconteurJavaNative-darwin.jnilib",
              arcname="libRobotRaconteurJavaNative-darwin.jnilib")
