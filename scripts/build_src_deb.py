from pathlib import Path
import subprocess
import shutil
import os

def main():
    repo_dir = Path("robotraconteur_debian_build").absolute()
    orig1 = list(repo_dir.glob("robotraconteur_*.orig.tar.gz"))
    assert len(orig1) > 0, "orig tarball not found"
    orig = orig1[0]
    print(orig)

    dists_dir = repo_dir.joinpath("dists")
    work_dir = Path("work")
    work_dir.mkdir()
    out_dir = Path("out")
    out_dir.mkdir()

    for d in dists_dir.iterdir():
        dist = d.name
        e_dir = work_dir.joinpath(f"{dist}/robotraconteur_debian")
        e_dir.mkdir(parents=True,)
        subprocess.check_call(f"tar xf {orig} --strip-components=1", shell=True, cwd=e_dir)
        shutil.copytree(d.joinpath("debian"),e_dir.joinpath("debian"))
        orig_name = orig.name
        #if "~" in orig_name:
        #    orig_name = orig_name.replace(".orig.tar.gz", f"~{dist}.orig.tar.gz")
        os.link(orig,work_dir.joinpath(f"{dist}/{orig_name}"))
        subprocess.check_call("debuild -S --no-sign --no-check-builddeps", shell=True, cwd=work_dir.joinpath(f"{dist}/robotraconteur_debian"))

    for d in work_dir.iterdir():
        dist = d.name
        distout_dir = out_dir.joinpath(dist)
        distout_dir.mkdir(parents=True)
        for f in d.iterdir():
            if not f.is_file():
                continue
            shutil.copyfile(f,f"{distout_dir}/{f.name}")


if __name__ == "__main__":
    main()