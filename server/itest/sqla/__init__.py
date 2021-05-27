import os
from baltrad.bdbserver.sqla import storage, backend

_backend = None

def setup_package():
    global _backend
    url = os.environ.get("BDB_TEST_DB", "")
    try:
        _backend = backend.SqlAlchemyBackend(
            url,
            storage=storage.DatabaseStorage()
        )
    except:
        pass

    if _backend:
        _backend.drop()
        _backend.create()
def teardown_package():
    global _backend
    if _backend:
        _backend.drop()

def get_backend():
    return _backend
