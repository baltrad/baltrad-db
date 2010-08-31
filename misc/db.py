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
                        UniqueConstraint, create_engine)

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
    Column("node_id", Text, nullable=False, unique=True),
)

source_centres = Table("bdb_source_centres", meta,
    Column("id", Integer, ForeignKey(sources.c.id),
           primary_key=True),
    Column("originating_centre", Integer, nullable=False, unique=True),
    Column("country_code", Integer, nullable=False, unique=True),
    Column("wmo_cccc", String(4), nullable=False, unique=True)
)

source_radars = Table("bdb_source_radars", meta,
    Column("id", Integer, ForeignKey(sources.c.id),
           primary_key=True),
    Column("centre_id", Integer, ForeignKey(source_centres.c.id),
           nullable=False),
    Column("radar_site", Text, unique=True),
    Column("wmo_code", Integer, unique=True),
    Column("place", Text, unique=True)
)

files = Table("bdb_files", meta,
    Column("id", Integer, primary_key=True),
    Column("hash_type", Text, nullable=False),
    Column("unique_id", Text, unique=True, nullable=False),
    Column("object", Text, nullable=False),
    Column("n_date", Date, nullable=False),
    Column("n_time", Time, nullable=False),
    Column("source_id", Integer, ForeignKey(sources.c.id),
           nullable=False),
)

groups = Table("bdb_groups", meta,
    Column("id", Integer, primary_key=True),
    Column("parent_id", Integer, ForeignKey("bdb_groups.id")),
    Column("name", Text, nullable=False),
    Column("product", Text),
    Column("startdate", Date),
    Column("starttime", Time),
    Column("enddate", Date),
    Column("endtime", Time),
    Column("file_id", Integer, nullable=False),
    ForeignKeyConstraint(["file_id"], [files.c.id],
                         ondelete="CASCADE")
)

attribute_groups = Table("bdb_attribute_groups", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Integer, nullable=False)
)

attributes = Table("bdb_attributes", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Text, nullable=False, unique=True),
    Column("converter", Text, nullable=False),
    Column("storage_table", Text, nullable=False),
    Column("storage_column", Text, nullable=False),
    Column("ignore_in_hash", Boolean, nullable=False, default=False)
)

invalid_attributes = Table("bdb_invalid_attributes", meta,
    Column("name", Text, nullable=False),
    Column("group_id", Integer, ForeignKey(groups.c.id, ondelete="CASCADE"),
           nullable=False),
    PrimaryKeyConstraint("name", "group_id"))

attribute_values_int = Table("bdb_attribute_values_int", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("group_id", Integer, nullable=False),
    Column("value", Int64, nullable=False),
    ForeignKeyConstraint(["group_id"], [groups.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "group_id")
)

attribute_values_str = Table("bdb_attribute_values_str", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("group_id", Integer, nullable=False),
    Column("value", Text, nullable=False),
    ForeignKeyConstraint(["group_id"], [groups.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "group_id")
)

attribute_values_real = Table("bdb_attribute_values_real", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("group_id", Integer, nullable=False),
    Column("value", Double, nullable=False),
    ForeignKeyConstraint(["group_id"], [groups.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "group_id")
)

attribute_values_bool = Table("bdb_attribute_values_bool", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("group_id", Integer, nullable=False),
    Column("value", Boolean, nullable=False),
    ForeignKeyConstraint(["group_id"], [groups.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "group_id")
)

attribute_values_date = Table("bdb_attribute_values_date", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("group_id", Integer, nullable=False),
    Column("value", Date, nullable=False),
    ForeignKeyConstraint(["group_id"], [groups.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "group_id")
)

attribute_values_time = Table("bdb_attribute_values_time", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("group_id", Integer, nullable=False),
    Column("value", Time, nullable=False),
    ForeignKeyConstraint(["group_id"], [groups.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "group_id")
)

default_storage = {
    "string":   attribute_values_str.c.value,
    "int":      attribute_values_int.c.value,
    "real":     attribute_values_real.c.value,
    "sequence": attribute_values_str.c.value,
    "bool":     attribute_values_bool.c.value,
    "date":     attribute_values_date.c.value,
    "time":     attribute_values_time.c.value,
}

special_storage = {
    "object":    files.c.object,
    "date":      files.c.n_date,
    "time":      files.c.n_time,
    "product":   groups.c.product,
    "startdate": groups.c.startdate,
    "starttime": groups.c.starttime,
    "enddate":   groups.c.enddate,
    "endtime":   groups.c.endtime,
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
    ("what/source:WMO",  "int",      source_radars.c.wmo_code),
    ("what/source:RAD",  "string",   source_radars.c.radar_site),
    ("what/source:ORG",  "int",      source_centres.c.originating_centre),
    ("what/source:CTY",  "int",      source_centres.c.country_code),
    ("what/source:PLC",  "string",   source_radars.c.place),
    ("what/source:node", "string",   sources.c.node_id),
]

ignored_in_hash = [
    "what/source",
    "Conventions",
]

countries = {
    "dk": 611,
    "ee": 612,
    "fi": 613,
    "lv": 625,
    "nl": 632,
    "no": 633,
    "pl": 634,
    "se": 643
}

# from WMO codes C-1
org_centres = {
    "dk": 94, # copenhagen
    "ee": 231, # estonia (NMC)
    "fi": 86, # helsinki
    "lv": 236, # latvia (NMC)
    "nl": 99, # de bilt
    "no": 88, # oslo
    "pl": 220, # warsaw
    "se": 82, # norrköping
}

wmo_ccccs = {
    "dk": "EKMI",
    "ee": "EEMH",
    "fi": "EFKL",
    "lv": "EVRR",
    "nl": "EHDB",
    "no": "ENMI",
    "pl": "SOWR",
    "se": "ESWI",
}

# country code, CID, WMO code, node, plc
radars = [
    ("dk", "DN44",             0, "dkbor", u"Bornholm"),
    ("dk", "DN41",          6180, "dkste", u"Stevns"),
    ("dk", "DN42",             0, "dkrom", u"Rømø"),
    ("dk", "DN43",             0, "dksin", u"Sindal"),
    ("ee", "EE-HARKU",     26038, "eetal", u"Harku"),
    ("ee", "EE-SYRGAVERE", 26232, "eesur", u"Sürgavere"),
    ("fi", "FI42",          2975, "fivan", u"Vantaa"),
    ("fi", "FI43",          2941, "fiika", u"Ikaalinen"),
    ("fi", "FI44",          2954, "fianj", u"Anjalankoski"),
    ("fi", "FI45",          2918, "fikuo", u"Kuopio"),
    ("fi", "FI46",          2933, "fikor", u"Korpo"),
    ("fi", "FI47",          2870, "fiuta", u"Utajärvi"),
    ("fi", "FI48",          2840, "filuo", u"Luosto"),
    ("fi", "FI49",          2925, "fivim", u"Vimpeli"),
    ("lv", "LVaa",         26422, "lvrix", u"Riga"),
    ("nl", "NL50",          6260, "nldbl", u"De Bilt"),
    ("nl", "NL51",          6234, "nldhl", u"Den Helder"),
    ("no", "NOaa",          1104, "norst", u"Røst"),
    ("no", "NOab",          1405, "nobom", u"Bømlo"),
    ("no", "NO41",          1499, "nosol", u"Oslo"),
    ("no", "NO42",          1438, "nohgb", u"Hægebostad"),
    ("no", "NO43",          1247, "norsa", u"Rissa"),
    ("no", "NOac",          1018, "noand", u"Andøya"),
    ("no", "NOad",          1042, "nohas", u"Hasvik"),
    ("pl", "PL41",         12374, "plleg", u"Legionowo"),
    ("pl", "PL42",         12514, "plram", u"Ramża"),
    ("pl", "PL43",         12544, "plpas", u"Pastewnik"),
    ("pl", "PL44",         12579, "plrze", u"Rzeszów"),
    ("pl", "PL45",         12331, "plpoz", u"Poznań"),
    ("pl", "PL46",         12220, "plswi", u"Świdwin"),
    ("pl", "PL47",         12151, "plgda", u"Gdańsk"),
    ("pl", "PL48",         12568, "plbrz", u"Brzuchania"),
    ("se", "SE40",          2032, "sekir", u"Kiruna"),
    ("se", "SE41",          2092, "selul", u"Luleå"),
    ("se", "SE42",          2200, "seosu", u"Östersund"),
    ("se", "SE43",          2262, "seovi", u"Örnsköldsvik"),
    ("se", "SE44",          2334, "sehud", u"Hudiksvall"),
    ("se", "SE45",          2430, "selek", u"Leksand"),
    ("se", "SE46",          2451, "searl", u"Arlanda"),
    ("se", "SE47",          2588, "sease", u"Ase"),
    ("se", "SE48",          2570, "sevil", u"Vilebo"),
    ("se", "SE49",          2600, "sevar", u"Vara"),
    ("se", "SE50",          2606, "seang", u"Ängelholm"),
    ("se", "SE51",          2666, "sekkr", u"Karlskrona"),
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
    centre_ids = {}

    for cc, country in countries.iteritems():
        centre_ids[cc] = id = idgen.next()
        engine.execute(sources.insert(), id=id, node_id=cc)
        engine.execute(source_centres.insert(),
                       id=id,
                       country_code=country,
                       wmo_cccc=wmo_ccccs[cc],
                       originating_centre=org_centres[cc])
    
    for (cc, site, wmo_code, node_id, place) in radars:
        id = idgen.next()
        engine.execute(sources.insert(), id=id, node_id=node_id)
        engine.execute(source_radars.insert(),
                       id=id,
                       centre_id=centre_ids[cc],
                       wmo_code=(wmo_code if wmo_code != 0  else None),
                       radar_site=site,
                       place=place)

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
