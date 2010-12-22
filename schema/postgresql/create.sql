CREATE TABLE bdb_sources (
    id SERIAL NOT NULL PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE bdb_files (
    id SERIAL NOT NULL PRIMARY KEY,
    uuid TEXT NOT NULL UNIQUE,
    hash TEXT NOT NULL,
    stored_at TIMESTAMP WITHOUT TIME ZONE NOT NULL,
    what_object TEXT NOT NULL,
    what_date DATE NOT NULL,
    what_time TIME WITHOUT TIME ZONE NOT NULL, 
    source_id INTEGER NOT NULL REFERENCES bdb_sources(id),
    UNIQUE (hash, source_id)
);
CREATE INDEX bdb_files_stored_at_key ON bdb_files(stored_at);
CREATE INDEX bdb_files_what_object_key ON bdb_files(what_object);
CREATE INDEX bdb_files_what_date_key ON bdb_files(what_date);
CREATE INDEX bdb_files_what_time_key ON bdb_files(what_time);
CREATE INDEX bdb_files_combined_datetime_key ON bdb_files((what_date + what_time));

CREATE TABLE bdb_file_content (
    file_id INTEGER NOT NULL PRIMARY KEY
        REFERENCES bdb_files(id) ON DELETE CASCADE,
    size INTEGER NOT NULL,
    lo_id INTEGER
);

CREATE RULE remove_lo AS ON DELETE TO bdb_file_content
    DO SELECT lo_unlink(OLD.lo_id);

CREATE TABLE bdb_nodes (
    id SERIAL NOT NULL PRIMARY KEY, 
    file_id INTEGER NOT NULL REFERENCES bdb_files(id) ON DELETE CASCADE,
    parent_id INTEGER REFERENCES bdb_nodes(id), 
    name TEXT NOT NULL,
    type INTEGER /* 1: group, 2: attribute, 3: dataset */
);
CREATE INDEX bdb_nodes_id_name_key ON bdb_nodes(id, name);
CREATE INDEX bdb_nodes_file_id_name_key ON bdb_nodes(file_id, name);
CREATE UNIQUE INDEX bdb_nodes_parent_id_name_key ON bdb_nodes(parent_id, name);

CREATE TABLE bdb_attribute_values (
    node_id INTEGER NOT NULL PRIMARY KEY
        REFERENCES bdb_nodes(id) ON DELETE CASCADE,
    value_int BIGINT,
    value_str TEXT,
    value_double DOUBLE PRECISION,
    value_bool BOOLEAN,
    value_date DATE,
    value_time TIME WITHOUT TIME ZONE
);

CREATE TABLE bdb_source_kvs (
    source_id INTEGER NOT NULL REFERENCES bdb_sources(id),
    key VARCHAR NOT NULL,
    value VARCHAR NOT NULL,
    PRIMARY KEY (source_id, key)
);

INSERT INTO bdb_sources (id, name) VALUES (1, 'dk');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (1, 'ORG', 94);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (1, 'CTY', 611);
INSERT INTO bdb_sources (id, name) VALUES (2, 'ee');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (2, 'ORG', 231);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (2, 'CTY', 612);
INSERT INTO bdb_sources (id, name) VALUES (3, 'fi');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (3, 'ORG', 86);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (3, 'CTY', 613);
INSERT INTO bdb_sources (id, name) VALUES (4, 'lv');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (4, 'ORG', 236);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (4, 'CTY', 625);
INSERT INTO bdb_sources (id, name) VALUES (5, 'nl');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (5, 'ORG', 99);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (5, 'CTY', 632);
INSERT INTO bdb_sources (id, name) VALUES (6, 'no');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (6, 'ORG', 88);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (6, 'CTY', 633);
INSERT INTO bdb_sources (id, name) VALUES (7, 'pl');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (7, 'ORG', 220);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (7, 'CTY', 634);
INSERT INTO bdb_sources (id, name) VALUES (8, 'se');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (8, 'ORG', 82);
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (8, 'CTY', 643);
INSERT INTO bdb_sources (id, name) VALUES (9, 'dkbor');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (9, 'PLC', 'Bornholm');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (9, 'RAD', 'DN44');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (9, 'WMO', '0');
INSERT INTO bdb_sources (id, name) VALUES (10, 'dkste');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (10, 'PLC', 'Stevns');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (10, 'RAD', 'DN41');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (10, 'WMO', '06180');
INSERT INTO bdb_sources (id, name) VALUES (11, 'dkrom');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (11, 'PLC', 'Rømø');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (11, 'RAD', 'DN42');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (11, 'WMO', '0');
INSERT INTO bdb_sources (id, name) VALUES (12, 'dksin');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (12, 'PLC', 'Sindal');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (12, 'RAD', 'DN43');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (12, 'WMO', '0');
INSERT INTO bdb_sources (id, name) VALUES (13, 'eetal');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (13, 'PLC', 'Harku');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (13, 'RAD', 'EE-HARKU');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (13, 'WMO', '26038');
INSERT INTO bdb_sources (id, name) VALUES (14, 'eesur');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (14, 'PLC', 'Sürgavere');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (14, 'RAD', 'EE-SYRGAVERE');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (14, 'WMO', '26232');
INSERT INTO bdb_sources (id, name) VALUES (15, 'fivan');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (15, 'PLC', 'Vantaa');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (15, 'RAD', 'FI42');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (15, 'WMO', '02975');
INSERT INTO bdb_sources (id, name) VALUES (16, 'fiika');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (16, 'PLC', 'Ikaalinen');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (16, 'RAD', 'FI43');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (16, 'WMO', '02941');
INSERT INTO bdb_sources (id, name) VALUES (17, 'fianj');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (17, 'PLC', 'Anjalankoski');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (17, 'RAD', 'FI44');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (17, 'WMO', '02954');
INSERT INTO bdb_sources (id, name) VALUES (18, 'fikuo');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (18, 'PLC', 'Kuopio');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (18, 'RAD', 'FI45');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (18, 'WMO', '02918');
INSERT INTO bdb_sources (id, name) VALUES (19, 'fikor');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (19, 'PLC', 'Korpo');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (19, 'RAD', 'FI46');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (19, 'WMO', '02933');
INSERT INTO bdb_sources (id, name) VALUES (20, 'fiuta');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (20, 'PLC', 'Utajärvi');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (20, 'RAD', 'FI47');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (20, 'WMO', '02870');
INSERT INTO bdb_sources (id, name) VALUES (21, 'filuo');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (21, 'PLC', 'Luosto');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (21, 'RAD', 'FI48');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (21, 'WMO', '02840');
INSERT INTO bdb_sources (id, name) VALUES (22, 'fivim');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (22, 'PLC', 'Vimpeli');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (22, 'RAD', 'FI49');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (22, 'WMO', '02925');
INSERT INTO bdb_sources (id, name) VALUES (23, 'lvrix');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (23, 'PLC', 'Riga');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (23, 'RAD', 'LVaa');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (23, 'WMO', '26422');
INSERT INTO bdb_sources (id, name) VALUES (24, 'nldbl');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (24, 'PLC', 'De Bilt');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (24, 'RAD', 'NL50');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (24, 'WMO', '06260');
INSERT INTO bdb_sources (id, name) VALUES (25, 'nldhl');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (25, 'PLC', 'Den Helder');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (25, 'RAD', 'NL51');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (25, 'WMO', '06234');
INSERT INTO bdb_sources (id, name) VALUES (26, 'norst');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (26, 'PLC', 'Røst');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (26, 'RAD', 'NOaa');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (26, 'WMO', '01104');
INSERT INTO bdb_sources (id, name) VALUES (27, 'nobom');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (27, 'PLC', 'Bømlo');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (27, 'RAD', 'NOab');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (27, 'WMO', '01405');
INSERT INTO bdb_sources (id, name) VALUES (28, 'nosol');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (28, 'PLC', 'Oslo');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (28, 'RAD', 'NO41');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (28, 'WMO', '01499');
INSERT INTO bdb_sources (id, name) VALUES (29, 'nohgb');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (29, 'PLC', 'Hægebostad');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (29, 'RAD', 'NO42');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (29, 'WMO', '01438');
INSERT INTO bdb_sources (id, name) VALUES (30, 'norsa');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (30, 'PLC', 'Rissa');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (30, 'RAD', 'NO43');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (30, 'WMO', '01247');
INSERT INTO bdb_sources (id, name) VALUES (31, 'noand');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (31, 'PLC', 'Andøya');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (31, 'RAD', 'NOac');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (31, 'WMO', '01018');
INSERT INTO bdb_sources (id, name) VALUES (32, 'nohas');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (32, 'PLC', 'Hasvik');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (32, 'RAD', 'NOad');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (32, 'WMO', '01042');
INSERT INTO bdb_sources (id, name) VALUES (33, 'plleg');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (33, 'PLC', 'Legionowo');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (33, 'RAD', 'PL41');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (33, 'WMO', '12374');
INSERT INTO bdb_sources (id, name) VALUES (34, 'plram');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (34, 'PLC', 'Ramża');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (34, 'RAD', 'PL42');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (34, 'WMO', '12514');
INSERT INTO bdb_sources (id, name) VALUES (35, 'plpas');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (35, 'PLC', 'Pastewnik');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (35, 'RAD', 'PL43');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (35, 'WMO', '12544');
INSERT INTO bdb_sources (id, name) VALUES (36, 'plrze');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (36, 'PLC', 'Rzeszów');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (36, 'RAD', 'PL44');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (36, 'WMO', '12579');
INSERT INTO bdb_sources (id, name) VALUES (37, 'plpoz');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (37, 'PLC', 'Poznań');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (37, 'RAD', 'PL45');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (37, 'WMO', '12331');
INSERT INTO bdb_sources (id, name) VALUES (38, 'plswi');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (38, 'PLC', 'Świdwin');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (38, 'RAD', 'PL46');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (38, 'WMO', '12220');
INSERT INTO bdb_sources (id, name) VALUES (39, 'plgda');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (39, 'PLC', 'Gdańsk');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (39, 'RAD', 'PL47');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (39, 'WMO', '12151');
INSERT INTO bdb_sources (id, name) VALUES (40, 'plbrz');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (40, 'PLC', 'Brzuchania');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (40, 'RAD', 'PL48');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (40, 'WMO', '12568');
INSERT INTO bdb_sources (id, name) VALUES (41, 'sekir');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (41, 'PLC', 'Kiruna');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (41, 'RAD', 'SE40');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (41, 'WMO', '02032');
INSERT INTO bdb_sources (id, name) VALUES (42, 'selul');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (42, 'PLC', 'Luleå');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (42, 'RAD', 'SE41');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (42, 'WMO', '02092');
INSERT INTO bdb_sources (id, name) VALUES (43, 'seosu');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (43, 'PLC', 'Östersund');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (43, 'RAD', 'SE42');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (43, 'WMO', '02200');
INSERT INTO bdb_sources (id, name) VALUES (44, 'seovi');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (44, 'PLC', 'Örnsköldsvik');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (44, 'RAD', 'SE43');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (44, 'WMO', '02262');
INSERT INTO bdb_sources (id, name) VALUES (45, 'sehud');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (45, 'PLC', 'Hudiksvall');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (45, 'RAD', 'SE44');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (45, 'WMO', '02334');
INSERT INTO bdb_sources (id, name) VALUES (46, 'selek');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (46, 'PLC', 'Leksand');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (46, 'RAD', 'SE45');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (46, 'WMO', '02430');
INSERT INTO bdb_sources (id, name) VALUES (47, 'searl');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (47, 'PLC', 'Arlanda');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (47, 'RAD', 'SE46');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (47, 'WMO', '02451');
INSERT INTO bdb_sources (id, name) VALUES (48, 'sease');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (48, 'PLC', 'Ase');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (48, 'RAD', 'SE47');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (48, 'WMO', '02588');
INSERT INTO bdb_sources (id, name) VALUES (49, 'sevil');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (49, 'PLC', 'Vilebo');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (49, 'RAD', 'SE48');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (49, 'WMO', '02570');
INSERT INTO bdb_sources (id, name) VALUES (50, 'sevar');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (50, 'PLC', 'Vara');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (50, 'RAD', 'SE49');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (50, 'WMO', '02600');
INSERT INTO bdb_sources (id, name) VALUES (51, 'seang');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (51, 'PLC', 'Ängelholm');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (51, 'RAD', 'SE50');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (51, 'WMO', '02606');
INSERT INTO bdb_sources (id, name) VALUES (52, 'sekkr');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (52, 'PLC', 'Karlskrona');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (52, 'RAD', 'SE51');
INSERT INTO bdb_source_kvs (source_id, key, value) VALUES (52, 'WMO', '02666');
ALTER SEQUENCE bdb_sources_id_seq RESTART WITH 53;
