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

import optparse
import os
import sys

from .cmd import Command, ExecutionError
from .rest import RestfulDatabase

def extract_command(args):
    command = None
    result_args = []
    for arg in args:
        if not command and not arg.startswith("-"):
            command = arg
        else:
            result_args.append(arg)
    return command, result_args

def run():
    optparser = optparse.OptionParser()
    optparser.set_usage(
        "%s COMMAND [--url=SERVER_URL] [ARGS]" % (
            os.path.basename(sys.argv[0])
        )
    )
    optparser.add_option(
        "--url", dest="server_url",
        default="http://localhost:8080",
        help="BDB server URL",
    )
    optparser.add_option(
        "-v", "--verbose", dest="verbose",
        action="store_true",
        help="be verbose",
    )

    command_name, args = extract_command(sys.argv[1:])

    if not command_name:
        print >> sys.stderr, "missing command\n"
        optparser.print_usage()
        raise SystemExit(1)

    try:
        command = Command.get_implementation_by_name(command_name)()
    except LookupError:
        print >> sys.stderr, "'%s' is not a valid command." % command_name
        raise SystemExit(1)
    
    optparser.set_usage(
        "%s %s [--url=SERVER_URL]" % (
            os.path.basename(sys.argv[0]),
            command_name
        )
    )
    command.update_optionparser(optparser)

    opts, args = optparser.parse_args(args)

    database = RestfulDatabase(opts.server_url)
    
    try:
        return command.execute(database, opts, args)
    except ExecutionError, e:
        raise SystemExit(e)
