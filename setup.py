import os
import platform

from setuptools import Extension, setup

CPPSTD = "c++17"
CSTD = "c17"
extra_compile_args = ["-D_GNU_SOURCE"]
if platform.system() == "Linux":
    strip_flags = ["-Wl,--strip-all"]
    extra_compile_args += [f"-std={CPPSTD}", f"-std={CSTD}"]
elif platform.system() == "Windows":
    strip_flags = []
    extra_compile_args += [f"/std:{CPPSTD}", f"/std:{CSTD}"]
elif platform.system() == "Darwin":
    strip_flags = []
    extra_compile_args += [f"-std={CPPSTD}", f"-std={CSTD}"]
else:
    strip_flags = []
    extra_compile_args += [f"/std:{CPPSTD}", f"/std:{CSTD}"]


def find_src():
    srcs = []
    headers = []
    for root, _, files in os.walk("src"):
        for file in files:
            if file.endswith(".c") or file.endswith(".cpp"):
                srcs.append(os.path.join(root, file))
            elif file.endswith(".h"):
                if file != "version_template.h":
                    headers.append(os.path.join(root, file))
    srcs.append(os.path.abspath("./deps/double-conversion.cpp"))
    headers.append(os.path.abspath("./deps/double-conversion.hpp"))
    return srcs, headers


srcs, headers = find_src()

module1 = Extension(
    "cjson",
    sources=srcs,
    include_dirs=["./src", "./deps"],
    extra_compile_args=extra_compile_args,
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
