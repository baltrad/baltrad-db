from baltrad.bdbclient import rest
import os, errno, subprocess, time, string
from baltradutils import jprops

PIDFILE="./build/bdb.pid"
LOGFILE="./build/bdb.log"
 
def execute_command(cmd):
    """executes the specified command
    :param: a string defining the command to be run
    :raise: :class:`Exception` if an error occurs
    """    
    code = subprocess.call(cmd, shell=True)
    if code != 0:
        raise Exception("Could not run command %s"%cmd)
  
def does_client_and_server_exist():
    """tests if the binaries baltrad-bdb-server and baltrad-bdb-client are on the path
    :return: True if both client and server exists, otherwise False 
    """    
    code = subprocess.call("which baltrad-bdb-server", shell=True)
    if code != 0:
        return False
    code = subprocess.call("which baltrad-bdb-client", shell=True)
    if code != 0:
        return False
    return True
 
def is_server_running(uri):
    """tests if the server is running by trying to send a list_sources command
    :param uri: the uri to the server 
    :return: True if server is running otherwise False 
    """    
    code = subprocess.call("baltrad-bdb-client list_sources --url=%s >> /dev/null 2>&1"%uri, shell=True)
    if code != 0:
        return False
    return True
 
def get_server_uri():
    """returns the server uri as defined in the BDB_PYCLIENT_ITEST_PROPERTYFILE
    :return: the server uri defined by the property baltrad.bdb.server.uri
    """    
    config = os.environ.get("BDB_PYCLIENT_ITEST_PROPERTYFILE", "")
     
    properties = None
     
    with open(config, 'rb') as fp:
        properties = jprops.load_properties(fp)
         
    return properties['baltrad.bdb.server.uri']
 
def pid_exists(pid):
    """tests if the pid is a running process
    :param pid: the pid of the process to check for 
    :return: True if the process is running otherwise False 
    """    
    if pid <= 0: # We really don't want to even try pid == 0
        return False
    try:
        os.kill(pid, 0)
    except OSError as e:
        return e.errno == errno.EPERM
    else:
        return True
 
def stop_server(uri, removepid=False):
    """stops the server running with the specified uri
    :param uri: the uri to the server 
    """
    if is_server_running(uri):
        if os.path.exists(PIDFILE):
            try:
                pid = int(open(PIDFILE).read().strip())
                if pid_exists(pid):
                    os.kill(pid, 15) # TERM
            except:
                pass
 
    if removepid and os.path.exists(PIDFILE):
        os.unlink(PIDFILE)
         
 
def setup():
    """executed by the nose tests before running the test suite
    """

    if os.path.exists(LOGFILE):
        os.unlink(LOGFILE)
     
    if not does_client_and_server_exist():
        raise Exception("Couldn't find client and server binaries")
 
    config = os.environ.get("BDB_PYCLIENT_ITEST_PROPERTYFILE", "")
     
    uri = get_server_uri()

    stop_server(uri, True)
     
    execute_command("baltrad-bdb-drop --conf=%s"%config)
    execute_command("baltrad-bdb-create --conf=%s"%config)
    execute_command("baltrad-bdb-server --conf=%s --pidfile=%s --logfile=%s"%(config, PIDFILE, LOGFILE))
     
    for i in range(3):
        if is_server_running(uri):
            break
        time.sleep(1)
 
    execute_command("baltrad-bdb-client import_sources --url=%s ./etc/itest_sources.xml"%uri)
 
def teardown():
    """executed by the nose tests after the test suite has been executed
    """    
    stop_server(get_server_uri())
     
def get_database():
    auth = rest.NoAuth()
    uri = get_server_uri()
    #if opts.keyczar_key:
    #    auth = rest.KeyczarAuth(opts.keyczar_key, opts.keyczar_name)
    #else:
    #    auth = rest.NoAuth()
    #
    return rest.RestfulDatabase(uri, auth)
