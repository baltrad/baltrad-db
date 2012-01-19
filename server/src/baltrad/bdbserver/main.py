from copy import copy
import logging
import optparse
import os
import sys

import daemon
from daemon.pidfile import TimeoutPIDLockFile
import lockfile

from baltrad.bdbserver import backend, config
from baltrad.bdbserver.web import app

logger = logging.getLogger("baltrad.bdbserver")

def excepthook(*exc_info):
    logger.error("unhandled exception", exc_info=exc_info)
    sys.exit(1)

def check_path(option, opt, value):
    return os.path.abspath(value)

class Option(optparse.Option):
    TYPES = optparse.Option.TYPES + ("path",)
    TYPE_CHECKER = copy(optparse.Option.TYPE_CHECKER)
    TYPE_CHECKER["path"] = check_path

def create_optparser():
    optparser = optparse.OptionParser(option_class=Option)
    optparser.set_usage(
        "%s [--conf=CONFFILE] [ARGS]" % (
            os.path.basename(sys.argv[0])
        )
    )
    optparser.add_option(
        "--conf", type="path", dest="conffile",
        help="configuration file",
    )
    return optparser

def create_backend(config):
    type_ = config.get("baltrad.bdb.server.backend.type")
    try:
        backend_cls = backend.Backend.get_implementation(type_)
    except LookupError:
        raise LookupError(
            "unknown baltrad.bdb.server.backend.type: %s" % type_
        )
    return backend_cls.create_from_config(config)

def read_config(conffile):
    if not conffile:
        raise SystemExit("configuration file not specified")
    try:
        return config.Properties.load(conffile)
    except IOError:
        raise SystemExit("failed to read configuration from " + conffile)
   
def run_create():
    optparser = create_optparser()

    opts, args = optparser.parse_args()

    config = read_config(opts.conffile)

    backend = create_backend(config)
    backend.create()

def run_drop():
    optparser = create_optparser()

    opts, args = optparser.parse_args()

    conf = read_config(opts.conffile)

    backend = create_backend(conf)
    backend.drop()

def run_upgrade():
    optparser = create_optparser()

    opts, args = optparser.parse_args()

    conf = read_config(opts.conffile)

    backend = create_backend(conf)
    backend.upgrade()

def configure_logging(opts):
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)

    if opts.foreground:
        logger.addHandler(logging.StreamHandler(sys.stdout))
    if opts.logfile:
        logger.addHandler(logging.FileHandler(opts.logfile))
    
def run_server():
    sys.excepthook = excepthook
    optparser = create_optparser()
    optparser.add_option(
        "--foreground", action="store_true",
        default=False,
        help="don't detach the process"
    )
    optparser.add_option(
        "--logfile", type="path", action="store",
        help="location of the log file",
    )
    optparser.add_option(
        "--pidfile", type="path", action="store",
        default="/var/run/baltrad-bdb-server.pid",
        help="location of the pid file"
    )

    opts, args = optparser.parse_args()

    conf = read_config(opts.conffile)

    pidfile=TimeoutPIDLockFile(opts.pidfile, acquire_timeout=0)

    daemon_ctx = daemon.DaemonContext(
        working_directory="/",
        chroot_directory=None,
        stdout=sys.stdout if opts.foreground else None,
        stderr=sys.stderr if opts.foreground else None,
        detach_process=not opts.foreground,
        pidfile=pidfile
    )
    
    server_type = conf["baltrad.bdb.server.type"]
    if server_type != "werkzeug":
        raise SystemExit("invalid server type in config %s" % server_type)
    server_uri = conf["baltrad.bdb.server.uri"]
    
    # try locking the pidfile to report possible errors to the user
    try:
        with pidfile:
            pass
    except lockfile.AlreadyLocked:
        raise SystemExit("pidfile already locked: %s" % opts.pidfile)
    except lockfile.LockFailed:
        raise SystemExit("failed to lock pidfile: %s" % opts.pidfile)

    with daemon_ctx:
        configure_logging(opts)
        application = app.from_conf(conf)
    app.serve(server_uri, application)

