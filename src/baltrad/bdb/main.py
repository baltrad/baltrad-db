import configparser
import getopt
import os
import sys

from .web.app import Application, serve
from .sqla.backend import SqlAlchemyBackend

DEFAULT_CONF = os.path.join(
    os.path.dirname(__file__),
    "defaults.ini"
)

def parse_config(conffiles):
    parser = configparser.ConfigParser()
    for conffile in conffiles:
        try:
            fp = open(conffile, "r")
            parser.read_file(fp)
        except IOError:
            print >> sys.stderr, "failed to read configuration from ", conffile
            continue
    return parser

def run():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "", ["conf="])
    except getopt.GetoptError, err:
        print >> sys.stderr, str(err)
        raise SystemExit

    conffiles = [DEFAULT_CONF]

    for opt, value in opts:
        if opt == "--conf":
            conffiles.append(os.path.abspath(value))
        else:
            assert False, "uhandled option: " + opt
    
    config = parse_config(conffiles)
    backend = SqlAlchemyBackend.create_from_config(config)
    app = Application(backend)
    serve(app, config)
