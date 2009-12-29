# -*- coding: UTF-8 -*-
import sys
from sqlalchemy import (MetaData, Table, Column, ForeignKey,
                        ForeignKeyConstraint, PrimaryKeyConstraint,
                        UniqueConstraint, create_engine)

from sqlalchemy.types import (Text, Integer, Float, Date, Time,
                              Boolean)

from sqlalchemy.databases.postgres import PGBigInteger


meta = MetaData()

sources = Table("sources", meta,
    Column("id", Integer, primary_key=True),
    Column("country_code", Integer, unique=True),
    Column("wmo_code", Integer, unique=True),
    Column("radar_site", Text, unique=True),
    Column("originating_centre", Integer, unique=True),
    Column("node_id", Text, nullable=False, unique=True),
    Column("place", Text, unique=True))

files = Table("files", meta,
    Column("id", Integer, primary_key=True),
    Column("path", Text, unique=True, nullable=False),
    Column("object", Text, nullable=False),
    Column("n_date", Date, nullable=False),
    Column("n_time", Time, nullable=False),
    Column("source_id", Integer, ForeignKey(sources.c.id),
           nullable=False))

data_objects = Table("data_objects", meta,
    Column("id", Integer, primary_key=True),
    Column("parent_id", Integer, ForeignKey("data_objects.id")),
    Column("name", Text, nullable=False),
    Column("product", Text),
    Column("startdate", Date),
    Column("starttime", Time),
    Column("enddate", Date),
    Column("endtime", Time),
    Column("file_id", Integer, nullable=False),
    ForeignKeyConstraint(["file_id"], [files.c.id],
                         ondelete="CASCADE"))

attribute_groups = Table("attribute_groups", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Integer, nullable=False))

attributes = Table("attributes", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Text, nullable=False, unique=True),
    Column("converter", Text, nullable=False),
    Column("storage_table", Text, nullable=False),
    Column("storage_column", Text, nullable=False))

attribute_values_int = Table("attribute_values_int", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("data_object_id", Integer, nullable=False),
    Column("value", PGBigInteger, nullable=False),
    ForeignKeyConstraint(["data_object_id"], [data_objects.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "data_object_id"))

attribute_values_str = Table("attribute_values_str", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("data_object_id", Integer, nullable=False),
    Column("value", Text, nullable=False),
    ForeignKeyConstraint(["data_object_id"], [data_objects.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "data_object_id"))

attribute_values_real = Table("attribute_values_real", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("data_object_id", Integer, nullable=False),
    Column("value", Float, nullable=False),
    ForeignKeyConstraint(["data_object_id"], [data_objects.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "data_object_id"))

attribute_values_bool = Table("attribute_values_bool", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("data_object_id", Integer, nullable=False),
    Column("value", Boolean, nullable=False),
    ForeignKeyConstraint(["data_object_id"], [data_objects.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "data_object_id"))

attribute_values_date = Table("attribute_values_date", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("data_object_id", Integer, nullable=False),
    Column("value", Date, nullable=False),
    ForeignKeyConstraint(["data_object_id"], [data_objects.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "data_object_id"))

attribute_values_time = Table("attribute_values_time", meta,
    Column("attribute_id", Integer, ForeignKey(attributes.c.id),
           nullable=False),
    Column("data_object_id", Integer, nullable=False),
    Column("value", Time, nullable=False),
    ForeignKeyConstraint(["data_object_id"], [data_objects.c.id],
                         ondelete="CASCADE"),
    PrimaryKeyConstraint("attribute_id", "data_object_id"))

default_storage = {
    "string": ("attribute_values_str", "value"),
    "int": ("attribute_values_int", "value"),
    "real": ("attribute_values_real", "value"),
    "sequence": ("attribute_values_str", "value"),
    "bool": ("attribute_values_bool", "value"),
    "date": ("attribute_values_date", "value"),
    "time": ("attribute_values_time", "value")
}

special_storage = {
    "object": ("files", "object"),
    "date": ("files", "n_date"),
    "time": ("files", "n_time"),
    "product": ("data_objects", "product"),
    "startdate": ("data_objects", "startdate"),
    "starttime": ("data_objects", "starttime"),
    "enddate": ("data_objects", "enddate"),
    "endtime": ("data_objects", "endtime"),
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
    ("path",    "string",   "files",   "path"),
    ("src_WMO", "int",      "sources", "wmo_code"),
    ("src_RAD", "string",   "sources", "radar_site"),
    ("src_ORG", "int",      "sources", "originating_centre"),
    ("src_CTY", "int",      "sources", "country_code"),
    ("src_node", "string",  "sources", "node_id")
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
    ("pl", "PL42",         12514, "plram", u"Ramza"),
    ("pl", "PL43",         12544, "plpas", u"Pastewnik"),
    ("pl", "PL44",         12579, "plrze", u"Rzeszow"),
    ("pl", "PL45",         12331, "plpoz", u"Poznan"),
    ("pl", "PL46",         12220, "plswi", u"Swidwin"),
    ("pl", "PL47",         12151, "plgda", u"Gdansk"),
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
        table, column = special_storage.get(name, default_storage[type])
        engine.execute(q,
                       name="/".join((group, name)) if group else name,
                       converter=type,
                       storage_table=table,
                       storage_column=column)
    
    for (name, type, table, column) in virtual_attributes:
        engine.execute(q,
                       name=name,
                       converter=type,
                       storage_table=table,
                       storage_column=column)

def populate_sources_table(engine):
    q = sources.insert()

    for cc, country in countries.iteritems():
        engine.execute(q,
                       country_code=country,
                       originating_centre=org_centres[cc],
                       node_id=cc)
    
    for (cc, site, wmo_code, node_id, place) in radars:
        engine.execute(q,
                       wmo_code=(wmo_code if wmo_code != 0  else None),
                       radar_site=site,
                       node_id=node_id,
                       place=place)
    
if __name__ == "__main__":
    engine = create_engine(sys.argv[1])
    meta.drop_all(bind=engine)
    meta.create_all(bind=engine)
    populate_attributes_table(engine)
    populate_sources_table(engine)
