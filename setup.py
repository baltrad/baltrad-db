#!/usr/bin/env python
from distribute_setup import use_setuptools
use_setuptools()
import setuptools

setuptools.setup(name="baltrad.bdb",
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
        "configparser >= 3.2",
#        "hlhdf",
        "SQLAlchemy >= 0.7",
        "werkzeug >= 0.8",
    ],
    entry_points = {
        "console_scripts" : [
            "baltrad-db-server = baltrad.bdb.main:run",
        ]
    },
    test_suite="nose.collector",
    tests_require=[
        "mock >= 0.7",
    ],
)
