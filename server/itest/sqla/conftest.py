import os
import pytest
from baltrad.bdbserver.sqla import storage, backend

@pytest.fixture(scope="session")
def bdb_backend():
    url = os.environ.get("BDB_TEST_DB", "")
    _backend = None
    
    try:
        _backend = backend.SqlAlchemyBackend(
            url,
            storage=storage.DatabaseStorage()
        )
    except Exception:
        pass

    if _backend:
        _backend.drop()
        _backend.create()
    
    yield _backend

    if _backend:
        _backend.drop()
