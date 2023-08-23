#!/usr/bin/env python
import setuptools

import sys

REQUIRED_PACKAGES= [
    "baltrad.bdbcommon",
    "jprops >= 2.0.2",
    "progressbar >= 2.0",
    "psycopg2",
    "pyasn1",
    "pycrypto >= 2.4",
    "python-daemon >= 1.6",
    "python-keyczar >= 0.7b",
    "SQLAlchemy >= 1.0.13",
    "sqlalchemy-migrate >= 0.10.0",
    "werkzeug >= 0.14"
]
if sys.version_info > (3,):
    REQUIRED_PACKAGES= [
        "baltrad.bdbcommon",
        "SMHI-python3-radar_pyutils",
        "progressbar2 >= 2.4",
        "psycopg2",
        "pyasn1",
#        "pycrypto >= 2.4",
        "python-daemon >= 1.6",
        "baltrad-crypto >= 0.71rc0",
        "sqlalchemy >= 1.0.13",
#       "sqlalchemy-migrate >= 0.10.0",
        "werkzeug >= 0.14"
]


setuptools.setup(name="baltrad.bdbserver",
    version="0.1-dev",
    namespace_packages=["baltrad"],
    setup_requires=['nose>=1.0'],
    packages=setuptools.find_packages(
        "src",
    ),
    package_dir={
        "": "src"
    },
    package_data={
        "": ["*.sql", "*.cfg"]
    },
    install_requires=REQUIRED_PACKAGES,
    entry_points = {
        "baltrad.bdbserver.backends": [
            "sqla = baltrad.bdbserver.sqla.backend:SqlAlchemyBackend",
        ],
        "baltrad.bdbserver.web.auth": [
            "noauth = baltrad.bdbserver.web.auth:NoAuth",
            "keyczar = baltrad.bdbserver.web.auth:KeyczarAuth",
        ],
        "baltrad.bdbserver.sqla.storage": [
            "db = baltrad.bdbserver.sqla.storage:DatabaseStorage",
            "fs = baltrad.bdbserver.sqla.storage:FileSystemStorage",
        ],
        "console_scripts" : [
            "baltrad-bdb-create = baltrad.bdbserver.main:run_create",
            "baltrad-bdb-drop = baltrad.bdbserver.main:run_drop",
            "baltrad-bdb-server = baltrad.bdbserver.main:run_server",
            "baltrad-bdb-upgrade = baltrad.bdbserver.main:run_upgrade",
            "baltrad-bdb-migrate-db = baltrad.bdbserver.main:run_migrate_db",
        ]
    },
    test_suite="nose.collector",
    tests_require=[
        "mock >= 0.7",
    ],
)
