import base64
import getopt
import httplib
import json
import pprint
import sys

def create_http_connection(server_url):
    return httplib.HTTPConnection(server_url)

def http_request(server_url, method, path, data="", headers={}):
    conn = create_http_connection(server_url)
    conn.request(method, path, data, headers)
    return conn.getresponse()

def import_file(server_url, args):
    source = args[0]
    
    data = json.dumps({
        "data": base64.encodestring(open(source, "r").read())
    })
    response = http_request(
        server_url, "POST", "/file/", data,
        headers={
            "Content-Type": "application/json",
        }
    )
    
    if response.status == httplib.CREATED:
        print "stored at", response.getheader("Location")
    else:
        print "failed to store", source, response.status

def show_file_metadata(server_url, args):
    target_uuid = args[0]

    response = http_request(
        server_url, "GET", "/file/" + target_uuid + "/metadata"
    )

    if response.status == httplib.OK:
        data = json.loads(response.read())
        pprint.pprint(data["metadata"])
    elif response.status == httplib.NOT_FOUND:
        print "file not found"
    else:
        print "error", response.status

def export_file(server_url, args):
    target_uuid = args[0]
    outfile = args[1]

    response = http_request(
        server_url, "GET", "/file/" + target_uuid
    )

    if response.status == httplib.OK:
        data = json.loads(response.read())["data"]
        outf = open(outfile, "w")
        outf.write(base64.standard_b64decode(data))
    elif response.status == httplib.NOT_FOUND:
        print "file not found"
    else:
        print "error", response.status
        

def remove_file(server_url, args):
    target_uuid = args[0]

    response = http_request(
        server_url, "DELETE", "/file/" + target_uuid
    )

    if response.status == httplib.OK:
        print target_uuid, "deleted"
    elif response.status == httplib.NOT_FOUND:
        print "file not found"
    else:
        print "couldn't delete", target_uuid, response.status

def list_sources(server_url, args):
    response = http_request(
        server_url, "GET", "/source/"
    )

    if response.status == httplib.OK:
        data = json.loads(response.read())
        pprint.pprint(data["sources"])
    else:
        print "error", response.status

def run():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "", ["url="])
    except getopt.GetoptError, err:
        print >> sys.stderr, str(err)
        raise SystemExit

    server_url = "localhost:8080"
    for opt, value in opts:
        if opt == "--url":
            server_url = value
        else:
            assert False, "uhandled option: " + opt

    command = args.pop(0)
    if command == "import":
        import_file(server_url, args)
    elif command == "export":
        export_file(server_url, args)
    elif command == "metadata":
        show_file_metadata(server_url, args)
    elif command == "remove":
        remove_file(server_url, args)
    elif command == "sources":
        list_sources(server_url, args)
