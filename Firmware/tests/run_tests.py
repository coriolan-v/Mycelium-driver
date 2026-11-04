"""Compile and run the actual portable receiver logic with a host C++ compiler."""
import os
from pathlib import Path
import shutil
import subprocess

root = Path(__file__).resolve().parents[1]
compiler = os.environ.get("CXX") or shutil.which("g++") or shutil.which("clang++")
if not compiler:
    local = root / ".pio/test-tools/toolchain-gccmingw32/bin/g++.exe"
    if local.is_file():
        compiler = str(local)
if not compiler:
    raise SystemExit("Install g++/clang++, or set CXX to a host C++ compiler.")

build = root / ".pio/receiver-tests"
build.mkdir(parents=True, exist_ok=True)
env = os.environ.copy()
env["PATH"] = str(Path(compiler).resolve().parent) + os.pathsep + env.get("PATH", "")
exe = build / ("receiver-tests.exe" if os.name == "nt" else "receiver-tests")
subprocess.run([
    compiler, "-std=c++14", "-O3", "-Wall", "-Wextra", "-Werror",
    "-static", "-I", str(root / "include"),
    str(root / "tests/test_receiver.cpp"), "-o", str(exe),
], check=True, env=env)
subprocess.run([str(exe)], check=True, env=env)
