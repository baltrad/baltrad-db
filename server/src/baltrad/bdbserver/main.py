import logging
import logging.handlers
import os
import sys

import daemon
from daemon.pidfile import TimeoutPIDLockFile
import lockfile

from baltrad.bdbcommon import optparse
from baltrad.bdbserver import backend, config
from baltrad.bdbserver.web import app


if sys.version_info < (3,):
  import urlparse
else:
  import urllib.parse as urlparse


logger = logging.getLogger("baltrad.bdbserver")

SYSLOG_ADDRESS = "/dev/log"
SYSLOG_FACILITY = "local3"

def excepthook(*exc_info):
    logger.error("unhandled exception", exc_info=exc_info)
    sys.exit(1)

def create_optparser():
    optparser = optparse.create_parser()
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
        backend_cls = backend.Backend.get_impl(type_)
    except LookupError:
        raise LookupError(
            "unknown baltrad.bdb.server.backend.type: %s" % type_
        )
    return backend_cls.from_conf(config)

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

def run_migrate_db():
    optparser = create_optparser()
    optparser.add_option(
        "--from-storage", type="string", dest="from_storage", default="db", help="the from storage type (default db)",
    )
    optparser.add_option(
        "--to-storage", type="string", dest="to_storage", default="fs", help="the storage type (default fs)",
    )
    
    opts, args = optparser.parse_args()
    
    conf = read_config(opts.conffile)
    
    if conf.get("baltrad.bdb.server.backend.type") != "sqla":
        raise Exception("current backend.type in configuration not set to sqla")
    if opts.from_storage == opts.to_storage:
        raise Exception("Cannot use same from and to storage type (%s)"%opts.from_storage)
    
    b = create_backend(conf)

    b.change_storage(conf, opts.from_storage, opts.to_storage)

def get_logging_level(conf):
    v = conf.get("baltrad.bdb.server.log.level", "INFO")
    if v == "DEBUG":
        return logging.DEBUG
    elif v == "INFO":
        return logging.INFO
    elif v == "WARN":
        return logging.WARN
    elif v == "WARNING":
        return logging.WARNING
    elif v == "ERROR":
        return logging.ERROR
    else:
        return logging.INFO

def configure_logging(opts, logtype, logid, level=logging.INFO):
    logger = logging.getLogger()
    logger.setLevel(level)

    default_formatter = logging.Formatter('%(asctime)-15s %(levelname)-8s %(message)s')
    if opts.foreground or logtype == 'stdout':
        handler = logging.StreamHandler(sys.stdout)
        add_loghandler(logger, handler, default_formatter)
    if opts.logfile:
        handler = logging.handlers.WatchedFileHandler(opts.logfile)
        add_loghandler(logger, handler, default_formatter)
    if logtype == "syslog":
        handler = logging.handlers.SysLogHandler(SYSLOG_ADDRESS, facility=SYSLOG_FACILITY)
        formatter = logging.Formatter('python[' + logid + ']: %(name)s: %(message)s')
        add_loghandler(logger, handler, formatter)

    
def add_loghandler(logger, handler, formatter=None):   
    handler.setFormatter(formatter)
    logger.addHandler(handler)

## Checks if the process with provided pid is running
# by checking the /proc directory.
# @param pid - the pid to check for
# @return True if a process with provided pid is running, otherwise False
def isprocessrunning(pid):
    return os.path.exists("/proc/%d"%pid)

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
    conf = read_config(opts.conffile)

    pidfile=TimeoutPIDLockFile(opts.pidfile, acquire_timeout=1.0)

    daemon_ctx = daemon.DaemonContext(
        working_directory="/",
        chroot_directory=None,
        stdout=sys.stdout if opts.foreground else None,
        stderr=sys.stderr if opts.foreground else None,
        detach_process=not opts.foreground,
        prevent_core=False,
        pidfile=pidfile
    )
    
    server_type = conf["baltrad.bdb.server.type"]
    if server_type not in ["cherrypy", "werkzeug"]:
        raise SystemExit("invalid server type in config %s" % server_type)
    
    server_uri = conf["baltrad.bdb.server.uri"]
    
    # try locking the pidfile to report possible errors to the user
    tryagain = False
    try:
        with pidfile:
            pass
    except lockfile.AlreadyLocked:
        tryagain = True
    except lockfile.LockFailed:
        tryagain = True
    except lockfile.LockTimeout:
        tryagain = True

    if tryagain:
        pid = lockfile.pidlockfile.read_pid_from_pidfile(opts.pidfile)
        if pid != None and not isprocessrunning(pid):
            try:
                message = "pidfile exists but it seems like process is not running, probably due to an uncontrolled shutdown. Resetting.\n"
                sys.stderr.write(message)
                os.remove(opts.pidfile)
            except:
                pass
    
        try:
            with pidfile:
                pass
        except lockfile.AlreadyLocked:
            raise SystemExit("pidfile already locked: %s" % opts.pidfile)
        except lockfile.LockFailed:
            raise SystemExit("failed to lock pidfile: %s" % opts.pidfile)
        except lockfile.LockTimeout:
            raise SystemExit("lock timeout on pidfile: %s" % opts.pidfile)
            
    with daemon_ctx:
        logtype = conf.get("baltrad.bdb.server.log.type", "logfile")
        logid = conf.get("baltrad.bdb.server.log.id", "baltrad.bdb.server")
        configure_logging(opts, logtype, logid, get_logging_level(conf))
        sys.excepthook = excepthook
        application = app.from_conf(conf)
        if server_type == "werkzeug":
            app.serve(server_uri, application)
        elif server_type == "cherrypy":
            try:
                from cheroot.wsgi import WSGIServer
            except:
                from cherrypy.wsgiserver import CherryPyWSGIServer as WSGIServer
            parsedurl = urlparse.urlsplit(server_uri)
            cherryconf = conf.filter("baltrad.bdb.server.cherrypy.")
            nthreads = cherryconf.get_int("threads", 10)
            nbacklog = cherryconf.get_int("backlog", 5)
            ntimeout = cherryconf.get_int("timeout", 10)            
            server = WSGIServer((parsedurl.hostname, parsedurl.port), application,
                numthreads=nthreads, request_queue_size=nbacklog, timeout=ntimeout)
            try:
                server.start()
            finally:
                server.stop()

