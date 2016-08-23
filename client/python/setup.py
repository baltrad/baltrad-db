#!/usr/bin/env python
from ez_setup import use_setuptools
use_setuptools(version="24.0.2")

import setuptools

setuptools.setup(name="baltrad.bdbclient",
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
        "pyasn1",
        "pycrypto >= 2.4",
        "python-keyczar >= 0.7b",
    ],
    entry_points = {
        "baltrad.bdbclient.commands": [
            "import = baltrad.bdbclient.cmd:ImportFile",
            "export = baltrad.bdbclient.cmd:ExportFile",
            "metadata = baltrad.bdbclient.cmd:PrintMetadata",
            "remove = baltrad.bdbclient.cmd:RemoveFile",
            "list_sources = baltrad.bdbclient.cmd:PrintSources",
            "import_sources = baltrad.bdbclient.cmd:ImportSources",
            "dump = baltrad.bdbclient.cmd:Dump",
            "da_stats = baltrad.bdbclient.cmd:DataAvailabilityStatistics",
        ],
        "console_scripts" : [
            "baltrad-bdb-client = baltrad.bdbclient.main:run"
        ]
    },
    test_suite="nose.collector",
    tests_require=[
        "mock >= 0.7",
        "jprops >= 0.1"
    ],
)
