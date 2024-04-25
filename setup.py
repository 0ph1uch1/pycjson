import os
import platform

from setuptools import Extension, setup

if platform.system() == "Linux":
    strip_flags = ["-Wl,--strip-all"]
else:
    strip_flags = []


def find_src():
    srcs = []
    headers = []
    dirs = ["src", "deps"]
    for d in dirs:
        for root, _, files in os.walk(d):
            for file in files:
                if file.endswith(".c") or file.endswith(".cpp"):
                    srcs.append(os.path.join(root, file))
                elif file.endswith(".h"):
                    if file != "version_template.h":
                        headers.append(os.path.join(root, file))
    return srcs, headers


srcs, headers = find_src()

module1 = Extension(
    "cjson",
    sources=srcs,
    include_dirs=["./src", "./deps"],
    extra_compile_args=["-D_GNU_SOURCE"],
    extra_link_args=["-lstdc++", "-lm"] + strip_flags,
)

with open("src/version_template.h", encoding='utf-8') as f:
    version_template = f.read()


setup(
    ext_modules=[module1],
    use_scm_version={
        "local_scheme": lambda x: "",
        "write_to": "src/version.h",
        "write_to_template": version_template,
    },
)
