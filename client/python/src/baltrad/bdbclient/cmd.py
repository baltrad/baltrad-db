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
import datetime
import logging
import os
import shutil
import itertools

from abc import abstractmethod, ABCMeta

import pkg_resources

from baltrad.bdbcommon import expr, oh5
from baltrad.bdbclient import db

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
                if isinstance(node, oh5.Attribute):
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
        if opts.dry_run and not opts.verbose:
            logger.setLevel(logging.INFO)

        logger.debug("Reading sources from %s" % rave_sources_path)
        with open(rave_sources_path) as f:
            sources = oh5.Source.from_rave_xml(f.read())
        
        db_sources = dict([(src.name, src) for src in database.get_sources()])

        for source in sources:
            if source.name in opts.ignore:
                logger.debug("ignoring %s" % source.name)
                continue

            db_source = db_sources.pop(source.name, None)
            if not db_source:
                logger.info("adding %s as %s", source.name, source)
                if not opts.dry_run:
                    database.add_source(source)
            elif db_source != source:
                logger.info("changing %s from %s to %s", source.name, db_source, source)
                if not opts.dry_run:
                    database.update_source(db_source.name, source)
            else:
                logger.debug("%s is up-to-date", source.name)
        
        if opts.remove:
            self.remove(database, db_sources.values(), opts.dry_run)
    
    def remove(self, database, sources, dry_run):
        for source in sources:
            logger.debug("removing %s", source.name)
            if not dry_run:
                database.remove_source(source.name)

class Dump(Command):
    def update_optionparser(self, parser):
        parser.set_usage(parser.get_usage().strip() + " [OPTIONS] DUMP_DIR")
        parser.add_option("--layers", dest="layers",
            action="store", type="int", default=3,
            help="number of directory layers in the dump",
        )

    def execute(self, database, opts, args):
        if len(args) > 1:
            raise ExecutionError("too many arguments")
        elif len(args) == 0:
            raise ExecutionError("missing dump target directory")

        dump_dir = os.path.abspath(args[0])

        if not opts.verbose:
            logger.setLevel(logging.INFO)

        result = database.execute_file_query(db.FileQuery())
        logger.info("dumping %s files to %s in %s layers",
            result.size(), dump_dir, opts.layers
        )
        while result.next():
            uuid = result.get_uuid()
            target  = self.get_target_path(dump_dir, opts.layers, uuid)
            target_dir = os.path.dirname(target)
            if os.path.exists(target):
                logger.debug("skipping %s", uuid)
                continue
            if not os.path.exists(target_dir):
                os.makedirs(target_dir)
            logger.debug("dumping %s to %s", uuid, target)
            shutil.copyfileobj(database.get_file_content(uuid), open(target, "w"))
    
    def get_target_path(self, dump_dir, layer_count, uuid):
        uuid_str = str(uuid)
        elements = [dump_dir]
        for i in range(0, layer_count):
            elements.append(uuid_str[i])
        elements.append(uuid_str)

        return os.path.join(*elements)

class DataAvailabilityStatistics(Command):
    nominal_datetime_attr = expr.add(
        expr.attribute("what/date", "date"),
        expr.attribute("what/time", "time")
    )
    stored_datetime_attr = expr.add(
        expr.attribute("_bdb/stored_date", "date"),
        expr.attribute("_bdb/stored_time", "time"),
    )
    source_attr = expr.attribute("_bdb/source_name", "string")
    object_attr = expr.attribute("what/object", "string")
    uuid_attr = expr.attribute("_bdb/uuid", "string")


    def update_optionparser(self, parser):
        parser.set_usage(parser.get_usage().strip() + " [OPTIONS]")
        parser.add_option("--from", dest="from_date",
            action="store", type="iso8601_datetime",
            help="lowest nominal date to select from (inclusive)"
        )
        parser.add_option("--to", dest="to_date",
            action="store", type="iso8601_datetime",
            help="highest nominal date to select to (exclusive)"
        )
        parser.add_option("--source", dest="sources",
            action="store", type="list", default=[], metavar="LIST",
            help="comma separated list of filters on BDB source name"
        )
        parser.add_option("--object", dest="objects",
            action="store", type="list", default=[], metavar="LIST",
            help="comma separated list of filters on /what/object"
        )
        parser.add_option("--frequency", dest="frequency",
            action="store", type="int", metavar="FREQ",
            help="the frequency the data is expected to arrive (in seconds)"
        )
        parser.add_option("--eta", dest="etas",
            action="store", type="list", default=[], metavar="LIST",
            help="add availability checkpoint (seconds from nominal time)"
        )
        parser.add_option("--source-attr", dest="source_attr",
            action="store",
            help="attribute to use for source lookups"
        )

    def execute(self, database, opts, args):
        if not opts.from_date:
            raise ExecutionError("--from must be specified")
        if not opts.to_date:
            raise ExecutionError("--to must be specified")
        if not opts.etas:
            raise ExecutionError("no --eta specified")
        if not opts.frequency:
            raise ExecutionError("--frequency must be specified")
        if not opts.sources:
            raise ExecutionError("no --source specified")
        if not opts.objects:
            raise ExecutionError("no --object specified")
        if opts.source_attr:
            self.source_attr = expr.attribute(opts.source_attr, "string")
        
        opts.etas = [int(eta) for eta in opts.etas]
        opts.sources.sort()
        opts.objects.sort()
        opts.etas.sort()

        file_counts = self.get_file_counts(database, opts)
        self.print_results(opts, file_counts)

    def get_file_counts(self, database, opts):
        result = {}
        for eta in opts.etas:
            eta_file_counts = self.query_for_eta(database, opts, eta)
            for source, obj in itertools.product(opts.sources, opts.objects):
                src_file_counts = result.setdefault((source, obj), [])
                src_file_counts.append(eta_file_counts.get((source, obj), 0))
        return result
        
    def print_results(self, opts, file_counts):
        expected_count = self.get_expected_file_count(opts)

        max_source_len = max([len(src) for src in opts.sources])
        max_source_len = max(max_source_len, 6)
        max_object_len = max([len(obj) for obj in opts.objects])
        max_object_len = max(max_object_len, 6)
        
        head_tmpl = "%%-%ds %%-%ds" % (max_source_len, max_object_len)
        print head_tmpl % ("source", "object"),
        for eta in opts.etas:
            print "%6d" % eta,
        print ""

        for source, obj in itertools.product(opts.sources, opts.objects):
            print head_tmpl % (source, obj),
            for count in file_counts[(source, obj)]:
                print "%5d%%" % (count / float(expected_count) * 100),
            print ""

    def create_base_query(self, opts):
        qry = db.AttributeQuery()
        qry.append_filter(expr.between(
            self.nominal_datetime_attr,
            expr.literal(opts.from_date),
            expr.literal(opts.to_date)
        ))
        if opts.sources:
            qry.append_filter(expr.in_(
                self.source_attr,
                opts.sources
            ))
        if opts.objects:
            qry.append_filter(expr.in_(
                self.object_attr,
                opts.objects
            ))
        qry.fetch("source", self.source_attr)
        qry.fetch("object", self.object_attr)
        qry.fetch("filecount", expr.count(self.uuid_attr))
        qry.group = [self.source_attr, self.object_attr]
        return qry
        
    def filter_query_for_eta(self, qry, eta):
        qry.append_filter(expr.le(
            expr.sub(self.stored_datetime_attr, self.nominal_datetime_attr),
            expr.literal(datetime.timedelta(seconds=eta))
        ))
        return qry

    def query_for_eta(self, database, opts, eta):
        qry = self.create_base_query(opts)
        qry = self.filter_query_for_eta(qry, eta)
        qresult = database.execute_attribute_query(qry)
        result = {}
        while qresult.next():
            key = (qresult.get("source"), qresult.get("object"))
            result[key] = qresult.get("filecount")
        return result

    def get_expected_file_count(self, opts):
        delta = opts.to_date - opts.from_date
        delta_seconds = delta.days * 86400 + delta.seconds
        return delta_seconds / opts.frequency
