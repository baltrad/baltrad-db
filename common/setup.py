#!/usr/bin/env python
from distribute_setup import use_setuptools
use_setuptools()
import setuptools

setuptools.setup(name="baltrad.bdbcommon",
    version="0.1-dev",
    namespace_packages=["baltrad"],
    setup_requires=['nose>=1.0'],
    packages=setuptools.find_packages(
        "src",
        exclude=["*.tests.*", "*.tests"],
    ),
    package_dir={
        "": "src"
    },
    install_requires=[
#        "hlhdf",
    ],
    test_suite="nose.collector",
    tests_require=[
        "mock >= 0.7",
    ],
)
