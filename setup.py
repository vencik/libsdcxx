#!/usr/bin/env python3

from distutils.core import setup, Extension
from os.path import dirname, realpath
import re


version = dict()
with open(f"{dirname(realpath(__file__))}/version.txt") as version_fd:
    for line in version_fd:
        level, no = re.fullmatch(r'^\s*VERSION_([A-Z]+)\s+([0-9]+)\s*$', line).groups()
        version[level.lower()] = int(no)


setup(name="pysdc",
    version=f"{version['major']}.{version['minor']}.{version['patch']}",
    description="Sørensen–Dice coefficient on string bigram multi-sets",
    author="Václav Krpec",
    author_email="vencik@razdva.cz",
    url="https://github.com/vencik/libsdcxx",
    license="BSD-3-Clause license",
    license_files=["LICENSE"],
    package_dir={"": "src"},
    packages=["pysdc"],
    ext_modules=[Extension(
        "libpysdc",
        sources=[
            "src/libpysdc/bigrams.cxx",
            "src/libpysdc/bigram_multiset.cxx",
            "src/libpysdc/unordered_bigram_multiset.cxx",
            "src/libpysdc/sequence_matcher.cxx",
        ],
        extra_compile_args=["-Isrc", "-std=c++17"],
    )],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Topic :: Text Processing",
        "License :: OSI Approved :: BSD License",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS :: MacOS X",
        "Programming Language :: Python :: 3",
    ],
)
