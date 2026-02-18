"""
Copyright (C) 2014- Swedish Meteorological and Hydrological Institute (SMHI)

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy import sql

revision = '008'
down_revision = '007'

def upgrade():
    # Add the parent column
    op.add_column('bdb_sources', sa.Column('parent', sa.Text(), nullable=True))
    
    # Populate parent information
    connection = op.get_bind()
    _update_parent_information(connection)


def downgrade():
    op.drop_column('bdb_sources', 'parent')


def _update_parent_information(conn):
    # Define the sources table for updates
    meta = sa.MetaData()
    sources = sa.Table('bdb_sources', meta,
        sa.Column('id', sa.Integer, primary_key=True),
        sa.Column('name', sa.Text, nullable=False),
        sa.Column('parent', sa.Text),
    )
    
    # Mapping of parent (country/org) to list of source node names
    nod_group_mapping = [
        ("at", ["atrau", "atfel", "atpat", "atzir", "atval"]),
        ("by", ["bymin"]),
        ("be", ["bezav", "bewid", "bejab"]),
        ("je", ["ukjer"]),
        ("hr", ["hrbil", "hrosi"]),
        ("cz", ["czska", "czbrd"]),
        ("dk", ["dkste", "dkrom", "dksin", "dkbor", "dkvir", "dkaal", "dkaar", "dkhvi", "dkvej", "dkvix", "dkode", "dkege", "dkhor"]),
        ("ee", ["eehar", "eesur"]),
        ("fi", ["fivan", "fiika", "fianj", "fikuo", "fikor", "fiuta", "filuo", "fivim", "fikes", "fipet"]),
        ("fr", ["frabb", "frbor", "frbou", "frgre", "frcae", "frnan", "frnim", "frtou", "frtra", "frtro", "frlep", "frtre", "frbol", "frpla", "fropo", "frniz", "frcol", "frale",
                "frmcl", "frave", "frche", "frbla", "frmom", "frmtc"]),
        ("de", ["deemd", "deham", "deboo", "deros", "dehan", "dehnr", "debln", "depro", "deess", "defld", "deumd", "deoft", "deneu", "dedrs", "denhb", "detur", "deeis", "deisn",
                "deflg", "demuc", "desna", "demem"]),
        ("gr", ["grimi", "grand", "grlar", "grthe", "geaig", "grakt", "grxri", "grast"]),
        ("hu", ["hubud", "hunap", "hupog"]),
        ("is", ["iskef", "istgb"]),
        ("ie", ["iesha", "iedub"]),
        ("it", ["itbrc", "itset", "itist", "itmac", "itfog", "itspc", "itgat", "itpis", "itras", "itteo", "itcos", "itgra", "itmid", "itbri", "itmil", "itfiu", "itdec", "itzou",
                "itcro", "itser", "itmon", "itpet"]),
        ("lt", ["ltlau", "ltvln"]),
        ("lv", ["lvrix"]),
        ("nl", ["nldbl", "nldhl"]),
        ("no", ["nober", "noosl", "nohgb", "norsa", "norst", "nobml", "noand", "nohas", "nosta", "nohur"]),
        ("pl", ["plleg", "plram", "plpas", "plrze", "plpoz", "plswi", "plgda", "plbrz"]),
        ("pt", ["ptlea", "ptcal"]),
        ("ro", ["robob", "robuc", "rocra", "robar", "romed", "roora", "rotim"]),
        ("rs", ["rsfru", "rsbaj", "rssam", "rsval", "rsbuk", "rspet", "rscrn", "rsuzi", "rsbes", "rssje", "rskru", "rsnis", "rskuk", "rsjas"]),
        ("ru", ["ruudx", "ruudb", "ruudi", "ruudk", "ruudp", "ruudl", "ruuwj", "ruavn", "ruudv"]),
        ("sk", ["skjav", "skkoj"]),
        ("si", ["silis"]),
        ("es", ["eslid", "esmad", "esval", "esmur", "esbar", "escor", "eszar", "essan", "essse", "esbad", "esmal", "essev", "esalm", "espma", "eslpa"]),
        ("se", ["sekir", "selul", "seosu", "seovi", "sehud", "selek", "searl", "sease", "sevil", "sevar", "seang", "sekkr"]),
        ("ch", ["chalb", "chdol", "chlem"]),
        ("uk", ["ukcle", "ukham", "ukche", "ukcas", "ukpre", "uking", "ukcyg", "ukdud", "uklew", "ukcob", "ukhhd", "ukmun", "ukthu", "ukdea", "ukhmy"]),
        ("eh", ["ehkap"])
    ]
    
    # Update parent for each group
    for parent, nodenames in nod_group_mapping:
        stmt = sql.update(sources).where(sources.c.name.in_(nodenames)).values(parent=parent)
        conn.execute(stmt)
