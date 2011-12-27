import getopt
import logging
import os
import sys

import daemon
from daemon.pidfile import TimeoutPIDLockFile

from .config import Properties
from .web.app import Application, serve
from . import backend

def run():
    logging.basicConfig()
    logging.getLogger("werkzeug").setLevel(logging.INFO)

    try:
        opts, args = getopt.getopt(sys.argv[1:], "", ["conf=", "pidfile=", "foreground"])
    except getopt.GetoptError, err:
        print >> sys.stderr, str(err)
        raise SystemExit
    
    conffile = None
    foreground = False
    pidfile = "/var/run/baltrad-bdb-server.pid"

    for opt, value in opts:
        if opt == "--conf":
            conffile = os.path.abspath(value)
        elif opt == "--foreground":
            foreground = True
        elif opt == "--pidfile":
            pidfile = os.path.abspath(value)
        else:
            assert False, "uhandled option: " + opt

    if not conffile:
        raise SystemExit("configuration file not specified")

    try:
        config = Properties.load(conffile)
    except IOError:
        raise SystemExit("failed to read configuration from " + conffile)
    
    config = config.filter("baltrad.bdb.server.")

    daemon_ctx = daemon.DaemonContext(
        working_directory="/",
        chroot_directory=None,
        stdout=sys.stdout,
        stderr=sys.stderr,
        detach_process=not foreground,
        pidfile=TimeoutPIDLockFile(pidfile, acquire_timeout=0),
    )


    with daemon_ctx:
        be = backend.create_from_config(config)
        if not be.is_operational():
            raise SystemExit("backend is not operational")
        app = Application(be)
        serve(app, config)
