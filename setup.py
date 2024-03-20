import os

from setuptools import Extension, setup


strip_flags = ["-Wl,--strip-all"]


def find_src():
    srcs = []
    headers = []
    for root, _, files in os.walk("src"):
        for file in files:
            if file.endswith(".c"):
                srcs.append(os.path.join(root, file))
            elif file.endswith(".h"):
                if file != "version_template.h":
                    headers.append(os.path.join(root, file))
    return srcs, headers


srcs, headers = find_src()

module1 = Extension(
    "cjson",
    sources=srcs,
    include_dirs=["./src"],
    extra_compile_args=["-D_GNU_SOURCE"],
    extra_link_args=["-lstdc++", "-lm"] + strip_flags,
)

with open("src/version_template.h") as f:
    version_template = f.read()


setup(
    ext_modules=[module1],
    use_scm_version={
        "local_scheme": lambda x: "",
        "write_to": "src/version.h",
        "write_to_template": version_template,
    },
)
