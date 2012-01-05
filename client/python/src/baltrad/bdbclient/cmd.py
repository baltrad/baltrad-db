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
import logging
import os
import shutil
import sys

from abc import abstractmethod, ABCMeta

import pkg_resources

from baltrad.bdbcommon.oh5 import Attribute, Source

logger = logging.getLogger("baltrad.bdbclient.cmd")

class ExecutionError(RuntimeError):
    pass

class Command(object):
    """command-line client command interface
    """
    __metaclass__ = ABCMeta

    @abstractmethod
    def update_optionparser(self, parser):
        raise NotImplementedError()
    
    @abstractmethod
    def execute(self, database, opts, args):
        """execute this command

        :param database: a database instance to operate on
        :param opts: options passed from command line
        :param args: arguments passed from command line
        """
        raise NotImplementedError()

    def __call__(self, database, opts, args):
        return self.execute(database, opts, args)

    @classmethod
    def get_implementation_by_name(cls, name):
        """get an implementing class by name

        the implementing class is looked up from 'baltrad.bdbclient.commands'
        entry point. 

        :raise: :class:`LookupError` if not found
        """
        try:
            return pkg_resources.load_entry_point(
                "baltrad.bdbclient",
                "baltrad.bdbclient.commands",
                name
            )
        except ImportError:
            raise LookupError(name)

class ImportFile(Command):
    def update_optionparser(self, parser):
        parser.set_usage(parser.get_usage().strip() + " FILE [, FILE]")

    def execute(self, database, opts, args):
        for path in args: 
            with open(path, "r") as data:
                entry = database.store(data)
            print path, "stored with UUID", entry.uuid

class ExportFile(Command):
    def update_optionparser(self, parser):
        parser.set_usage(parser.get_usage().strip() + " UUID OUTFILE")

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
    def update_optionparser(self, parser):
        parser.set_usage(parser.get_usage().strip() + " UUID [, UUID]")

    def execute(self, database, opts, args):
        for uuid in args:
            if database.remove_file_entry(uuid):
                print uuid, "deleted"
            else:
                print uuid, "not found"
        
class PrintMetadata(Command):
    def update_optionparser(self, parser):
        parser.set_usage(parser.get_usage().strip() + " UUID")

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
    def update_optionparser(self, parser):
        pass

    def execute(self, database, opts, args):
        sources = sorted(
            database.get_sources(),
            cmp=lambda x, y: cmp(x.name, y.name)
        )

        for source in sources:
            print source.name + "\t" + source.to_string()

class ImportSources(Command):
    def update_optionparser(self, parser):
        parser.set_usage(parser.get_usage().strip() + " [OPTIONS] FILE")
        parser.add_option("--ignore", dest="ignore",
            action="append", default=[],
            help="ignore a source definition",
        )
        parser.add_option("--dry-run", dest="dry_run",
            action="store_true",
            help="show what would be done, don't actually modify the source",
        )
        parser.add_option("--no-updates", dest="update",
            default=True,
            action="store_false",
            help="don't update changed sources",
        )
        parser.add_option("--no-removes", dest="remove",
            default=True,
            action="store_false",
            help="don't remove sources",
        )

    def execute(self, database, opts, args):
        try:
            rave_sources_path = os.path.abspath(args.pop(0))
        except IndexError:
            raise ExecutionError("missing input file")
        
        logging.basicConfig(format="%(message)s")
        if opts.dry_run:
            logger.setLevel(logging.INFO)
        if opts.verbose:
            logger.setLevel(logging.DEBUG)

        logger.debug("Reading sources from %s" % rave_sources_path)
        with open(rave_sources_path) as f:
            sources = Source.from_rave_xml(f.read())
        
        db_sources = dict([(src.name, src) for src in database.get_sources()])

        for source in sources:
            if source.name in opts.ignore:
                logger.debug("ignoring %s" % source.name)
                continue

            db_source = db_sources.pop(source.name, None)
            if not db_source:
                if opts.dry_run or opts.verbose:
                    logger.info("adding %s as %s", source.name, source)
                else:
                    database.add_source(source)
            elif db_source != source:
                if opts.dry_run or opts.verbose:
                    logger.info("changing %s from %s to %s", source.name, db_source, source)
                else:
                    database.update_source(db_source.name, source)
            else:
                logger.debug("%s is up-to-date", source.name)
        
        if opts.remove:
            self.remove(database, db_sources.values())
    
    def remove(self, database, sources):
        for source in sources:
            logger.debug("removing %s", source.name)
            database.remove_source(source.name)
