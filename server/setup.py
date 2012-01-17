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
    ),
    package_dir={
        "": "src"
    },
    package_data={
        "": ["*.sql"]
    },
    install_requires=[
        "baltrad.bdbcommon",
        "jprops >= 0.1",
        "pyasn1",
        "python-daemon >= 1.6",
        "python-keyczar",
        "psycopg2",
        "SQLAlchemy >= 0.7",
        "werkzeug >= 0.8",
    ],
    entry_points = {
        "baltrad.bdbserver.backends": [
            "sqlalchemy = baltrad.bdbserver.sqla.backend:SqlAlchemyBackend",
        ],
        "baltrad.bdbserver.web.auth": [
            "noauth = baltrad.bdbserver.web.auth:NoAuth",
            "keyczar = baltrad.bdbserver.web.auth:KeyczarAuth",
        ],
        "console_scripts" : [
            "baltrad-bdb-create = baltrad.bdbserver.main:run_create",
            "baltrad-bdb-drop = baltrad.bdbserver.main:run_drop",
            "baltrad-bdb-server = baltrad.bdbserver.main:run_server",
            "baltrad-bdb-upgrade = baltrad.bdbserver.main:run_upgrade",
        ]
    },
    test_suite="nose.collector",
    tests_require=[
        "mock >= 0.7",
    ],
)
