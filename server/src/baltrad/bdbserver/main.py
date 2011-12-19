import getopt
import logging
import os
import sys

from .config import Properties
from .web.app import Application, serve
from . import backend

def run():
    logging.basicConfig()
    logging.getLogger("werkzeug").setLevel(logging.INFO)

    try:
        opts, args = getopt.getopt(sys.argv[1:], "", ["conf="])
    except getopt.GetoptError, err:
        print >> sys.stderr, str(err)
        raise SystemExit
    
    conffile = None
    for opt, value in opts:
        if opt == "--conf":
            conffile = os.path.abspath(value)
        else:
            assert False, "uhandled option: " + opt

    if not conffile:
        raise SystemExit("configuration file not specified")

    try:
        config = Properties.load(conffile)
    except IOError:
        raise SystemExit("failed to read configuration from " + conffile)
    
    config = config.filter("baltrad.bdb.server.")

    be = backend.create_from_config(config)
    if not be.is_operational():
        raise SystemExit("backend is not operational")
    app = Application(be)
    serve(app, config)
