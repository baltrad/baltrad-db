#!/usr/bin/env python

import setuptools

REQUIRED_PACKAGES=[]

# If we can't get hold of pyhl we install h5py instead
try:
    import _pyhl
except ModuleNotFoundError:
    REQUIRED_PACKAGES.append("h5py >= 3.6, < 4.0")
    
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
    install_requires=REQUIRED_PACKAGES,
    test_suite="nose.collector",
    tests_require=[
        "mock >= 0.7",
    ],
)
