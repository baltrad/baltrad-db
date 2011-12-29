# Copyright 2010-2011 Estonian Meteorological and Hydrological Institute
# 
# This file is part of baltrad-db.
# 
# baltrad-db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# baltrad-db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

import contextlib
import getopt
import os
import shutil
import sys

from baltrad.bdbcommon.oh5 import Source

from .rest import RestfulDatabase

def import_file(database, args):
    source = args[0]
    
    with open(source, "r") as data:
        entry = database.store(data)
    
    print "stored with UUID", entry.uuid

def show_file_metadata(database, args):
    target_uuid = args[0]

    entry = database.get_file_entry(target_uuid)

    if entry:
        for node in entry.metadata:
            print node.get_path()
    else:
        print "file not found"

def export_file(database, args):
    target_uuid = args[0]
    outfile = args[1]
    
    content = database.get_file_content(target_uuid)

    if content:
        with contextlib.closing(content):
            with open(outfile, "w") as outf:
                shutil.copyfileobj(content, outf)
    else:
        print "file not found"

def remove_file(database, args):
    target_uuid = args[0]

    if database.remove_file_entry(target_uuid):
        print target_uuid, "deleted"
    else:
        print "file not found"

def list_sources(database, args):
    for source in database.get_sources():
        print source.name + "\t" + source.to_string()

def import_sources(database, args):
    rave_sources_path = os.path.abspath(args.pop(0))

    print "Reading sources from", rave_sources_path

    with open(rave_sources_path) as f:
        sources = Source.from_rave_xml(f.read())
    
    db_sources = dict([(src.name, src) for src in database.get_sources()])

    for source in sources:
        db_source = db_sources.pop(source.name, None)
        if not db_source:
            database.add_source(source)
#            print "ADD:\t", source.name, type(source.to_string())
        elif db_source != source:
            print "UPDATE:\t", source.name, db_source.to_string(), "->", source.to_string()
        else:
            print "OK:\t", source.name, source.to_string()
    
    for db_source in db_sources:
        print "REMOVE:\t", db_source

COMMAND_HANDLERS = {
    "import": import_file,
    "export": export_file,
    "metadata": show_file_metadata,
    "remove": remove_file,
    "list_sources": list_sources,
    "import_sources": import_sources,
}

USAGE = """
%(prog)s --url=SERVER_ADDRESS COMMAND [args]

  Available commands:
    import - import a file to the database
    export - export a file from the database
    metadata - print file metadata
    remove - remove a file from the database
    list_sources - print sources defined in the database
    import_sources - update source definitions from Rave config
"""

def print_usage():
    print USAGE % {"prog": os.path.basename(sys.argv[0])}

def run():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "", ["url=", "help"])
    except getopt.GetoptError, err:
        print >> sys.stderr, str(err)
        raise SystemExit

    server_url = "http://localhost:8080/"
    for opt, value in opts:
        if opt == "--url":
            server_url = value
        elif opt == "--help":
            print_usage()
            raise SystemExit
        else:
            raise SystemExit("uhandled option: " + opt)

    try:
        command = args.pop(0)
    except IndexError:
        raise SystemExit("you must specify a command for the tool")
    
    try:
        handler = COMMAND_HANDLERS[command]
    except KeyError:
        raise SystemExit("Unkown command: %s" % command)
    
    return handler(RestfulDatabase(server_url), args)
