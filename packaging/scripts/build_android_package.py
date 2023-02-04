import shutil
import subprocess
from pathlib import Path
import glob
import tempfile
import re
import zipfile

asset_dir = "artifacts/main"
android_abis = ["x86", "x86_64", "armeabi-v7a", "arm64-v8a"]

with open(asset_dir + '/out-android-x86_64/rrversion.txt') as f:
    ver_str1 = f.read().strip()

ver_str_m = re.match(r"^(\d+\.\d+.\d+)", ver_str1)
assert ver_str_m
ver_str = ver_str_m.group(1)

with zipfile.ZipFile('distfiles/RobotRaconteurCoreAndroid-v1(%s).aar' % ver_str, 'w', zipfile.ZIP_DEFLATED) as zip:
    zip.write('robotraconteur/packaging/android_aar/AndroidManifest.xml',
              arcname="AndroidManifest.xml")
    zip.write('robotraconteur/packaging/android_aar/R.txt', arcname="R.txt")
    zip.write('robotraconteur/packaging/android_aar/aar-metadata.properties',
              arcname='META-INF/com/android/build/gradle/aar-metadata.properties')
    zip.write(f'{asset_dir}/out-android-x86_64/Java/RobotRaconteurAndroid.jar',
              arcname='libs/RobotRaconteurAndroid.jar')
    for abi in android_abis:
        zip.write(f'{asset_dir}/out-android-{abi}/Java/libRobotRaconteurJavaNative-android.so',
                  arcname=f'jni/{abi}/libRobotRaconteurJavaNative-android.so')
