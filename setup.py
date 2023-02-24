#!/usr/bin/env python3

from setuptools import setup, Extension
from pathlib import Path
import re


root_dir = Path(__file__).parent

version = dict()
with open(str(root_dir/"version.txt")) as version_fd:
    for line in version_fd:
        level, no = re.fullmatch(r'^\s*VERSION_([A-Z]+)\s+([0-9]+)\s*$', line).groups()
        version[level.lower()] = int(no)


setup(name="pysdcxx",
    version=f"{version['major']}.{version['minor']}.{version['patch']}",
    description="Sørensen–Dice coefficient on string bigram multi-sets (in C++)",
    author="Václav Krpec",
    author_email="vencik@razdva.cz",
    url="https://github.com/vencik/libsdcxx",
    license="BSD-3-Clause license",
    long_description=(root_dir/"README.md").read_text(),
    long_description_content_type="text/markdown",
    license_files=["LICENSE"],
    package_dir={"": "src"},
    packages=["pysdcxx"],
    ext_modules=[Extension(
        "libpysdcxx",
        sources=[
            "src/libpysdcxx/bigrams.cxx",
            "src/libpysdcxx/bigram_multiset.cxx",
            "src/libpysdcxx/unordered_bigram_multiset.cxx",
            "src/libpysdcxx/sequence_matcher.cxx",
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
