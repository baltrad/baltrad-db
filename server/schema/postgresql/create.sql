CREATE TABLE bdb_sources (
    id SERIAL NOT NULL PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE bdb_files (
    id SERIAL NOT NULL PRIMARY KEY,
    uuid TEXT NOT NULL UNIQUE,
    hash TEXT NOT NULL,
    stored_date DATE NOT NULL,
    stored_time TIME WITHOUT TIME ZONE NOT NULL,
    what_object TEXT NOT NULL,
    what_date DATE NOT NULL,
    what_time TIME WITHOUT TIME ZONE NOT NULL, 
    source_id INTEGER NOT NULL REFERENCES bdb_sources(id),
    size INTEGER NOT NULL,
    lo_id INTEGER,
    UNIQUE (hash, source_id)
);
CREATE INDEX bdb_files_stored_at_key ON bdb_files(stored_at);
CREATE INDEX bdb_files_what_object_key ON bdb_files(what_object);
CREATE INDEX bdb_files_what_date_key ON bdb_files(what_date);
CREATE INDEX bdb_files_what_time_key ON bdb_files(what_time);
CREATE INDEX bdb_files_combined_datetime_key ON bdb_files((what_date + what_time));
CREATE RULE remove_lo AS ON DELETE TO bdb_files
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
