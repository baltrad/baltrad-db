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

import abc
import contextlib
import os
import shutil
import sys

from baltrad.bdbcommon.oh5 import Attribute, Source

class ExecutionError(RuntimeError):
    pass

class Command(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def add_options(self, parser):
        raise NotImplementedError()
    
    @abc.abstractmethod
    def execute(self, database, opts, args):
        raise NotImplementedError()
    
    def __call__(self, database, opts, args):
        return self.execute(database, opts, args)

class ImportFile(Command):
    def add_options(self, parser):
        parser.set_usage(
            "%s %s [OPTIONS] FILE [, FILE]" % (
                os.path.basename(sys.argv[0]),
                sys.argv[1]
            )
        )

    def execute(self, database, opts, args):
        for path in args: 
            with open(path, "r") as data:
                entry = database.store(data)
            print path, "stored with UUID", entry.uuid

class ExportFile(Command):
    def add_options(self, parser):
        parser.set_usage(
            "%s %s [OPTIONS] UUID OUTFILE" % (
                os.path.basename(sys.argv[0]),
                sys.argv[1]
            )
        )

    def execute(self, database, opts, args):
        try:
            uuid = args.pop(0)
        except IndexError:
            raise ExecutionError("missing UUID")
        
        try:
            outfile = args.pop(0)
        except IndexError:
            raise ExecutionError("missing OUTFILE")
    
        content = database.get_file_content(uuid)

        if content:
            with contextlib.closing(content):
                with open(outfile, "w") as outf:
                    shutil.copyfileobj(content, outf)
        else:
            print "file not found by UUID", uuid

class RemoveFile(Command):
    def add_options(self, parser):
        parser.set_usage(
            "%s %s [OPTIONS] UUID [, UUID]" % (
                os.path.basename(sys.argv[0]),
                sys.argv[1]
            )
        )
    
    def execute(self, database, opts, args):
        for uuid in args:
            if database.remove_file_entry(uuid):
                print uuid, "deleted"
            else:
                print uuid, "not found"
        
class PrintMetadata(Command):
    def add_options(self, parser):
        parser.set_usage(
            "%s %s [OPTIONS] UUID" % (
                os.path.basename(sys.argv[0]),
                sys.argv[1]
            )
        )
    
    def execute(self, database, opts, args):
        try:
            uuid = args.pop(0)
        except IndexError:
            raise ExecutionError("missing UUID")

        entry = database.get_file_entry(uuid)

        if entry:
            for node in entry.metadata.iternodes():
                if isinstance(node, Attribute):
                    print "%s=%s" % (node.path(), node.value)
        else:
            print "file not found"

class PrintSources(Command):
    def add_options(self, parser):
        parser.set_usage(
            "%s %s [OPTIONS]" % (
                os.path.basename(sys.argv[0]),
                sys.argv[1]
            )
        )
    

    def execute(self, database, opts, args):
        sources = sorted(
            database.get_sources(),
            cmp=lambda x, y: cmp(x.name, y.name)
        )

        for source in sources:
            print source.name + "\t" + source.to_string()

class ImportSources(Command):
    def add_options(self, parser):
        parser.set_usage(
            "%s %s [OPTIONS] FILE" % (
                os.path.basename(sys.argv[0]),
                sys.argv[1]
            )
        )

    def execute(self, database, opts, args):
        try:
            rave_sources_path = os.path.abspath(args.pop(0))
        except IndexError:
            raise ExecutionError("missing input file")

        print "Reading sources from", rave_sources_path

        with open(rave_sources_path) as f:
            sources = Source.from_rave_xml(f.read())
        
        db_sources = dict([(src.name, src) for src in database.get_sources()])

        for source in sources:
            db_source = db_sources.pop(source.name, None)
            if not db_source:
                database.add_source(source)
#                print "ADD:\t", source.name, type(source.to_string())
            elif db_source != source:
                print "UPDATE:\t", source.name, db_source.to_string(), "->", source.to_string()
            else:
                print "OK:\t", source.name, source.to_string()
        
        for db_source in db_sources:
            print "REMOVE:\t", db_source

