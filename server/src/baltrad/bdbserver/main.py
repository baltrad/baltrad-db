import getopt
import os
import sys

from .config import Properties
from .web.app import Application, serve
from . import backend

def run():
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
    
    try:
        config = Properties.load(conffile)
    except IOError:
        raise SystemExit("failed to read configuration from " + conffile)
    
    config = config.filter("baltrad.bdb.server.")

    be = backend.create_from_config(config)
    app = Application(be)
    serve(app, config)
