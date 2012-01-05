import logging
import optparse
import os
import sys

from copy import copy

import daemon
from daemon.pidfile import TimeoutPIDLockFile

from .config import Properties
from .web.app import Application, serve
from .backend import Backend

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
        backend_cls = Backend.get_implementation_by_name(type_)
    except LookupError:
        raise LookupError(
            "unknown baltrad.bdb.server.backend.type: %s" % type_
        )
    return backend_cls.create_from_config(config)

def read_config(conffile):
    if not conffile:
        raise SystemExit("configuration file not specified")
    try:
        return Properties.load(conffile)
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

    config = read_config(opts.conffile)

    backend = create_backend(config)
    backend.drop()

def run_upgrade():
    optparser = create_optparser()

    opts, args = optparser.parse_args()

    config = read_config(opts.conffile)

    backend = create_backend(config)
    backend.upgrade()

def configure_logging(opts):
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)

    if opts.foreground:
        logger.addHandler(logging.StreamHandler(sys.stdout))
    if opts.logfile:
        logger.addHandler(logging.FileHandler(opts.logfile))
    
def run_server():
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

    config = read_config(opts.conffile)

    daemon_ctx = daemon.DaemonContext(
        working_directory="/",
        chroot_directory=None,
        stdout=sys.stdout if opts.foreground else None,
        stderr=sys.stderr if opts.foreground else None,
        detach_process=not opts.foreground,
        pidfile=TimeoutPIDLockFile(opts.pidfile, acquire_timeout=0),
    )

    with daemon_ctx:
        configure_logging(opts)
        backend = create_backend(config)
        if not backend.is_operational():
            raise SystemExit("backend is not operational")
        app = Application(backend)
        serve(app, config)
