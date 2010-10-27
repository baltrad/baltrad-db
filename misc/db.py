# -*- coding: UTF-8 -*-

# Copyright 2010 Estonian Meteorological and Hydrological Institute
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

import codecs
import itertools
import os
import sys
import string
import StringIO as stringio

from sqlalchemy import (MetaData, Table, Column, ForeignKey,
                        ForeignKeyConstraint, PrimaryKeyConstraint,
                        UniqueConstraint, create_engine, DDL)

from sqlalchemy.types import (Text, Integer, Float, Date, Time,
                              Boolean, String, TypeEngine, TypeDecorator)

from sqlalchemy.databases.postgres import PGBigInteger, PGDoublePrecision

from sqlalchemy.sql import expression as sqlexpr

class Int64(TypeDecorator):
    impl = TypeEngine

    def load_dialect_impl(self, dialect):
        if dialect.name == "postgres":
            return PGBigInteger()
        else:
            return dialect.type_descriptor(Integer)

class Double(TypeDecorator):
    impl = TypeEngine

    def load_dialect_impl(self, dialect):
        if dialect.name == "postgres":
            return PGDoublePrecision()
        else:
            return dialect.type_descriptor(Float)

meta = MetaData()

sources = Table("bdb_sources", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Text, nullable=False, unique=True),
)

source_kvs = Table("bdb_source_kvs", meta,
    Column("source_id", Integer, ForeignKey(sources.c.id)),
    Column("key", String, nullable=False),
    Column("value", String, nullable=False),
    PrimaryKeyConstraint("source_id", "key"),
)

files = Table("bdb_files", meta,
    Column("id", Integer, primary_key=True),
    Column("hash", Text, nullable=False),
    Column("object", Text, nullable=False),
    Column("n_date", Date, nullable=False),
    Column("n_time", Time, nullable=False),
    Column("source_id", Integer, ForeignKey(sources.c.id),
           nullable=False),
    UniqueConstraint("hash", "source_id"),
)

file_content = Table("bdb_file_content", meta,
    Column("file_id", Integer, ForeignKey(files.c.id, ondelete="CASCADE"),
           primary_key=True),
    Column("lo_id", Integer))
DDL("CREATE RULE remove_lo AS ON DELETE TO %(table)s "
        "DO SELECT lo_unlink(OLD.lo_id)",
    on="postgres").execute_at("after-create", file_content)

nodes = Table("bdb_nodes", meta,
    Column("id", Integer, primary_key=True),
    Column("parent_id", Integer, ForeignKey("bdb_nodes.id")),
    Column("name", Text, nullable=False),
    Column("type", Integer), # 1 - group, 2 - attribute
    Column("file_id", Integer, nullable=False),
    ForeignKeyConstraint(["file_id"], [files.c.id],
                         ondelete="CASCADE")
)

attributes = Table("bdb_attributes", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Text, nullable=False, unique=True),
    Column("converter", Text, nullable=False),
    Column("storage_table", Text, nullable=False),
    Column("storage_column", Text, nullable=False),
    Column("ignore_in_hash", Boolean, nullable=False, default=False)
)

attribute_values = Table("bdb_attribute_values", meta,
    Column("node_id", Integer, nullable=False),
    Column("value_int", Int64),
    Column("value_str", Text),
    Column("value_real", Double),
    Column("value_bool", Boolean),
    Column("value_date", Date),
    Column("value_time", Time),
    ForeignKeyConstraint(["node_id"], [nodes.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("node_id"),
)

default_storage = {
    "string":   attribute_values.c.value_str,
    "int":      attribute_values.c.value_int,
    "real":     attribute_values.c.value_real,
    "sequence": attribute_values.c.value_str,
    "bool":     attribute_values.c.value_bool,
    "date":     attribute_values.c.value_date,
    "time":     attribute_values.c.value_time,
}

special_storage = {
    "object":    files.c.object,
    "date":      files.c.n_date,
    "time":      files.c.n_time,
}

# name, group, type
attribute_entries = [
    ("Conventions", None, "string"),

    ##
    # Table 1
    # Mandatory top-level what header Attributes for all weather radar files
    ##
    ("object",      "what", "string"),
    ("version",     "what", "string"),
    ("date",        "what", "date"),
    ("time",        "what", "time"),
    ("source",      "what", "string"),
    
    ##
    # Table 4
    # where Attributes for polar data objects
    #
    ("lon",         "where", "real"),
    ("lat",         "where", "real"),
    ("height",      "where", "real"),
    # Dataset specific
    ("elangle",     "where", "real"),
    ("nbins",       "where", "int"),
    ("rstart",      "where", "real"),
    ("rscale",      "where", "real"),
    ("nrays",       "where", "int"),
    ("a1gate",      "where", "int"),
    # Sector specific
    ("startaz",     "where", "real"),
    ("stopaz",      "where", "real"),

    ##
    # Table 5
    # where Attributes for geographical image data Groups
    ##
    ("projdef",     "where", "string"),
    ("xsize",       "where", "int"),
    ("ysize",       "where", "int"),
    ("xscale",      "where", "real"),
    ("yscale",      "where", "real"),
    ("LL_lon",      "where", "real"),
    ("LL_lat",      "where", "real"),
    ("UL_lon",      "where", "real"),
    ("UL_lat",      "where", "real"),
    ("UR_lon",      "where", "real"),
    ("UR_lat",      "where", "real"),
    ("LR_lon",      "where", "real"),
    ("LR_lat",      "where", "real"),

    ##
    # Table 6
    # where for cross-section data Group
    ##
    # Common Attributes
    #("xsize",       "where", "int"), # dup
    #("ysize",       "where", "int"), # dup
    #("xscale",      "where", "real"), # dup
    #("yscale",      "where", "real"), # dup
    ("minheight",   "where", "real"),
    ("maxheight",   "where", "real"),
    # RHI specific
    #("lon",         "where", "real"); # dup
    #("lat",         "where", "real"); # dup
    ("az_angle",    "where", "real"),
    ("angles",      "where", "sequence"),
    ("range",       "where", "real"),
    # Cross section and side panel specific
    ("start_lon",   "where", "real"),
    ("start_lat",   "where", "real"),
    ("stop_lon",    "where", "real"),
    ("stop_lat",    "where", "real"),
    
    ##
    # Table 7
    # where for vertical profiles
    ##
    #("lon",         "where", "real"); // dup
    #("lat",         "where", "real"); // dup
    #("height",      "where", "real"); // dup
    ("levels",      "where", "int"),
    ("interval",    "where", "real"),
    #("minheight",   "where", "real"); // dup
    #("maxheight",   "where", "real"); // dup

    ##
    # Table 8
    # how attributes for all objects
    ##
    # General
    ("task",        "how", "string"),
    ("startepochs", "how", "int"),
    ("endepochs",   "how", "int"),
    ("system",      "how", "string"),
    ("software",    "how", "string"),
    ("sw_version",  "how", "string"),
    ("zr_a",        "how", "real"),
    ("zr_b",        "how", "real"),
    ("kr_a",        "how", "real"),
    ("kr_b",        "how", "real"),
    ("simulated",   "how", "bool"),
    # Data from individual radars
    ("beamwidth",   "how", "real"),
    ("wavelength",  "how", "real"),
    ("rpm",         "how", "real"),
    ("pulsewidth",  "how", "real"),
    ("lowprf",      "how", "int"),
    ("highprf",     "how", "int"),
    # Polar data
    ("azmethod",    "how", "string"),
    ("binmethod",   "how", "string"),
    ("azangles",    "how", "sequence"),
    ("elangles",    "how", "sequence"),
    ("aztimes",     "how", "sequence"),
    # Cartesian images including composites
    ("angles",      "how", "sequence"),
    ("arotation",   "how", "sequence"),
    ("camethod",    "how", "string"),
    ("nodes",       "how", "sequence"),
    ("ACCnum",      "how", "int"),
    # Vertical profile specific
    ("minrange",    "how", "real"),
    ("maxrange",    "how", "real"),
    ("NI",          "how", "real"),
    ("dealiased",   "how", "int"),
    # Quality
    ("pointaccEL",  "how", "real"),
    ("pointaccAZ",  "how", "real"),
    ("malfunc",     "how", "bool"),
    ("radar_msg",   "how", "string"),
    ("radhoriz",    "how", "real"),
    ("MDS",         "how", "real"),
    ("OUR",         "how", "real"),
    ("Dclutter",    "how", "sequence"),
    ("comment",     "how", "string"),
    ("SQI",         "how", "real"),
    ("CSR",         "how", "real"),
    ("LOG",         "how", "real"),
    ("VPRCorr",     "how", "bool"),
    ("freeze",      "how", "real"),
    ("min",         "how", "real"),
    ("max",         "how", "real"),
    ("step",        "how", "real"),
    ("levels",      "how", "int"),
    ("peakpwr",     "how", "real"),
    ("avgpwr",      "how", "real"),
    ("dynrange",    "how", "real"),
    ("RAC",         "how", "real"),
    ("BBC",         "how", "bool"),
    ("PAC",         "how", "real"),
    ("S2N",         "how", "real"),
    ("polarization", "how", "string"),

    ##
    # Table 13
    # what Group for Dataset Objects
    ##
    ("product",     "what", "string"),
    # XXX: wtf, this has varying type (double or sequence)
    # prodpar   Tab. 15
    ("quantity",    "what", "string"),
    ("startdate",   "what", "date"),
    ("starttime",   "what", "time"),
    ("enddate",     "what", "date"),
    ("endtime",     "what", "time"),
    ("gain",        "what", "real"),
    ("offset",      "what", "real"),
    ("nodata",      "what", "real"),
    ("undetect",    "what", "real"),

    ##
    # Table 17
    # Eight-bit Image attributes.
    ##
    ("CLASS",         None, "string"),
    ("IMAGE_VERSION", None, "string"),
]

virtual_attributes = [
    ("file:id",          "int",      files.c.id),
    ("what/source:name", "string",   sources.c.name),
    ("what/source:node", "string",   sources.c.name),
]

ignored_in_hash = [
    "what/source",
    "Conventions",
]

class _Org(object):
    def __init__(self, name, org, cty):
        self.name = name
        self.kv = {
            "ORG": org,
            "CTY": cty,
        }

orgs = [
    _Org("dk", 94,  611),
    _Org("ee", 231, 612),
    _Org("fi", 86,  613),
    _Org("lv", 236, 625),
    _Org("nl", 99,  632),
    _Org("no", 88,  633),
    _Org("pl", 220, 634),
    _Org("se", 82,  643),
]

class _Radar(object):
    def __init__(self, name, rad, wmo, plc):
        self.name = name
        self.kv = {
            "RAD": rad,
            "WMO": wmo,
            "PLC": plc,
        }

# node, CID, WMO code, node
radars = [
    _Radar("dkbor", "DN44",             "0", u"Bornholm"),
    _Radar("dkste", "DN41",         "06180", u"Stevns"),
    _Radar("dkrom", "DN42",             "0", u"Rømø"),
    _Radar("dksin", "DN43",             "0", u"Sindal"),
    _Radar("eetal", "EE-HARKU",     "26038", u"Harku"),
    _Radar("eesur", "EE-SYRGAVERE", "26232", u"Sürgavere"),
    _Radar("fivan", "FI42",         "02975", u"Vantaa"),
    _Radar("fiika", "FI43",         "02941", u"Ikaalinen"),
    _Radar("fianj", "FI44",         "02954", u"Anjalankoski"),
    _Radar("fikuo", "FI45",         "02918", u"Kuopio"),
    _Radar("fikor", "FI46",         "02933", u"Korpo"),
    _Radar("fiuta", "FI47",         "02870", u"Utajärvi"),
    _Radar("filuo", "FI48",         "02840", u"Luosto"),
    _Radar("fivim", "FI49",         "02925", u"Vimpeli"),
    _Radar("lvrix", "LVaa",         "26422", u"Riga"),
    _Radar("nldbl", "NL50",         "06260", u"De Bilt"),
    _Radar("nldhl", "NL51",         "06234", u"Den Helder"),
    _Radar("norst", "NOaa",         "01104", u"Røst"),
    _Radar("nobom", "NOab",         "01405", u"Bømlo"),
    _Radar("nosol", "NO41",         "01499", u"Oslo"),
    _Radar("nohgb", "NO42",         "01438", u"Hægebostad"),
    _Radar("norsa", "NO43",         "01247", u"Rissa"),
    _Radar("noand", "NOac",         "01018", u"Andøya"),
    _Radar("nohas", "NOad",         "01042", u"Hasvik"),
    _Radar("plleg", "PL41",         "12374", u"Legionowo"),
    _Radar("plram", "PL42",         "12514", u"Ramża"),
    _Radar("plpas", "PL43",         "12544", u"Pastewnik"),
    _Radar("plrze", "PL44",         "12579", u"Rzeszów"),
    _Radar("plpoz", "PL45",         "12331", u"Poznań"),
    _Radar("plswi", "PL46",         "12220", u"Świdwin"),
    _Radar("plgda", "PL47",         "12151", u"Gdańsk"),
    _Radar("plbrz", "PL48",         "12568", u"Brzuchania"),
    _Radar("sekir", "SE40",         "02032", u"Kiruna"),
    _Radar("selul", "SE41",         "02092", u"Luleå"),
    _Radar("seosu", "SE42",         "02200", u"Östersund"),
    _Radar("seovi", "SE43",         "02262", u"Örnsköldsvik"),
    _Radar("sehud", "SE44",         "02334", u"Hudiksvall"),
    _Radar("selek", "SE45",         "02430", u"Leksand"),
    _Radar("searl", "SE46",         "02451", u"Arlanda"),
    _Radar("sease", "SE47",         "02588", u"Ase"),
    _Radar("sevil", "SE48",         "02570", u"Vilebo"),
    _Radar("sevar", "SE49",         "02600", u"Vara"),
    _Radar("seang", "SE50",         "02606", u"Ängelholm"),
    _Radar("sekkr", "SE51",         "02666", u"Karlskrona"),
]

def populate_attributes_table(engine):
    q = attributes.insert()

    for (name, group, type) in attribute_entries:
        column = special_storage.get(name, default_storage[type])
        full_name = "/".join((group, name)) if group else name
        engine.execute(q,
                       name=full_name,
                       converter=type,
                       storage_table=column.table.name,
                       storage_column=column.name,
                       ignore_in_hash=full_name in ignored_in_hash)
    
    for (name, type, column) in virtual_attributes:
        engine.execute(q,
                       name=name,
                       converter=type,
                       storage_table=column.table.name,
                       storage_column=column.name,
                       ignore_in_hash=True)

def populate_sources_table(engine):
    idgen = (i for i in itertools.count(1))

    for src in orgs + radars:
        id = idgen.next()
        engine.execute(sources.insert(), id=id, name=src.name)
        kvs = [{"source_id": id, "key": k, "value": v} for k, v in src.kv.iteritems()]
        for kv in kvs:
            engine.execute(source_kvs.insert(), **kv)
    
class Template(string.Template):
    delimiter = ":"

class StringIOExecutor(object):
    def __init__(self):
        self._ids = {}
        self.content = stringio.StringIO()

    def __call__(self, object, *args, **kw):
        if isinstance(object, sqlexpr.Insert):
            if "id" in object.table.columns and "id" not in kw:
                kw["id"] = self.generate_id(object.table)
            nullable = [col for col in object.table.columns if col.nullable]
            for col in nullable:
                if col.key not in kw:
                    kw[col.key] = None
        if isinstance(object, basestring) and object.startswith("\nDROP TABLE"):
            object = object.replace("DROP TABLE", "DROP TABLE IF EXISTS")
        kw = self.escape(kw)
        print >> self.content, Template(unicode(object)).substitute(kw) + ";"
    
    def escape(self, params):
        escaped = {}
        for param, value in params.iteritems():
            if isinstance(value, basestring):
                value = u"'%s'" % value
            elif value is None:
                value = u"NULL"
            escaped[param] = value
        return escaped
    
    def generate_id(self, table):
        if table not in self._ids:
            gen = (i for i in itertools.count(1))
            self._ids[table] = gen
        return self._ids[table].next()
    
    def write_content(self, path):
        path = os.path.abspath(path)
        dir, file = os.path.split(path)
        if not os.path.exists(dir):
            os.makedirs(dir)
        f = codecs.open(path, "w", encoding="utf-8")
        f.write(self.content.getvalue())
        f.close()
        self.content = stringio.StringIO()

def main():
    if len(sys.argv) < 3:
        print >> sys.stderr, "usage %s <dialect> <outdir>" % sys.argv[0]
        sys.exit(1)
    dialect_name = sys.argv[1]
    outdir = os.path.abspath(sys.argv[2])
    
    executor = StringIOExecutor()
    engine = create_engine("%s://" % dialect_name,
                           strategy="mock", executor=executor)
    meta.create_all(bind=engine)
    populate_attributes_table(engine)
    populate_sources_table(engine)
    executor.write_content(os.path.join(outdir, dialect_name, "create.sql"))
    meta.drop_all(bind=engine)
    executor.write_content(os.path.join(outdir, dialect_name, "drop.sql"))
    
if __name__ == "__main__":
    main()
