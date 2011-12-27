#!/usr/bin/env python
from distribute_setup import use_setuptools
use_setuptools()
import setuptools

setuptools.setup(name="baltrad.bdbserver",
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
        "baltrad.bdbcommon",
        "jprops >= 0.1",
        "SQLAlchemy >= 0.7",
        "werkzeug >= 0.8",
        "python-daemon >= 1.5",
        "psycopg2",
    ],
    entry_points = {
        "console_scripts" : [
            "baltrad-bdb-create = baltrad.bdbserver.main:run_create",
            "baltrad-bdb-drop = baltrad.bdbserver.main:run_drop",
            "baltrad-bdb-server = baltrad.bdbserver.main:run_server",
        ]
    },
    test_suite="nose.collector",
    tests_require=[
        "mock >= 0.7",
    ],
)
