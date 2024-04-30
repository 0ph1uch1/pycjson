import os
import platform
import subprocess
import sys

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


with open("src/version_template.h", encoding='utf-8') as f:
    version_template = f.read()


class CMakeExtension(Extension):
    def __init__(self, name, cmake_lists_dir='.', **kwargs):
        Extension.__init__(self, name, sources=[], **kwargs)
        self.cmake_lists_dir = os.path.abspath(cmake_lists_dir)


class cmake_build_ext(build_ext):
    def build_extensions(self):
        # Ensure that CMake is present and working
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError('Cannot find CMake executable')

        for ext in self.extensions:
            ext_fullpath = self.get_ext_fullpath(ext.name)
            extdir = os.path.abspath(os.path.dirname(ext_fullpath))
            # filename = os.path.basename(ext_fullpath)
            # cfg = 'Debug' if options['--debug'] == 'ON' else 'Release'
            cfg = 'Release'
            py_executable = sys.executable

            cmake_args = [
                f'-DCMAKE_BUILD_TYPE={cfg.upper()}',
                f'-DCMAKE_INSTALL_PREFIX={extdir}',
                f'-DPython_ROOT_DIR={os.path.dirname(os.path.dirname(os.path.dirname(os.__file__)))}',
                f'-DPYTHON_EXECUTABLE={py_executable}',
                # Ask CMake to place the resulting library in the directory
                # containing the extension
                # '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir),  # strange
                # Other intermediate static libraries are placed in a
                # temporary build directory instead
                # '-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), self.build_temp),  # strange
                # Hint CMake to use the same Python executable that
                # is launching the build, prevents possible mismatching if
                # multiple versions of Python are installed
                # '-DPYTHON_EXECUTABLE={}'.format(sys.executable),
                # Add other project-specific CMake arguments if needed
                # ...
            ]

            is_pypy = platform.python_implementation() == 'PyPy'
            if is_pypy:
                cmake_args += [
                    '-DPYPY_VERSION=1'
                    '-DPython3_FIND_IMPLEMENTATIONS=PyPy'
                ]

            # We can handle some platform-specific settings at our discretion
            if platform.system() == 'Windows':
                plat = ('x64' if platform.architecture()[0] == '64bit' else 'Win32')
                # cmake_args += [
                # These options are likely to be needed under Windows
                # '-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE',
                # '-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir),
                # ]
                # Assuming that Visual Studio and MinGW are supported compilers
                if self.compiler.compiler_type == 'msvc':
                    cmake_args += [
                        f'-DCMAKE_GENERATOR_PLATFORM={plat}',
                    ]
                else:
                    cmake_args += [
                        '-G', 'MinGW Makefiles',
                    ]
            # cmake_args += cmake_cmd_args

            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)

            # Config
            subprocess.check_call(['cmake', ext.cmake_lists_dir] + cmake_args,
                                  cwd=self.build_temp)

            # Build
            subprocess.check_call(['cmake', '--build', '.', '--config', cfg, '--target', 'install'],
                                  cwd=self.build_temp)

            # rename
            if platform.system() != 'Windows':
                os.rename(os.path.join(extdir, 'cjson.so'), ext_fullpath)
            else:
                os.rename(os.path.join(extdir, 'bin/cjson.dll'), ext_fullpath)


setup(
    name='cjson',
    use_scm_version={
        "local_scheme": lambda x: "",
        "write_to": "src/version.h",
        "write_to_template": version_template,
    },
    ext_modules=[
        CMakeExtension(name='cjson')
    ],
    cmdclass=dict(build_ext=cmake_build_ext)
)
