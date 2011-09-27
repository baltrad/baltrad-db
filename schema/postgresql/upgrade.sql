CREATE OR REPLACE FUNCTION make_plpgsql()
RETURNS VOID
  LANGUAGE SQL
AS $$
  CREATE LANGUAGE plpgsql;
$$;

SELECT
  CASE
    WHEN EXISTS (
      SELECT 1 from pg_catalog.pg_language where lanname='plpgsql'
    ) THEN 
      NULL
    ELSE make_plpgsql()
  END;

CREATE OR REPLACE FUNCTION restart_seq_with_max(table_name TEXT, column_name TEXT)
 RETURNS BIGINT AS $$
DECLARE
 maxval BIGINT;
BEGIN
 EXECUTE 'SELECT COALESCE(MAX(' || column_name || '), 0) + 1 FROM '
         || table_name INTO maxval;
 EXECUTE 'ALTER SEQUENCE '
         || table_name || '_' || column_name || '_seq'
         || ' RESTART WITH '
         || maxval;
 RETURN maxval;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION bdbupgrade_add_size_to_bdb_file_content() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM information_schema.tables WHERE table_name = 'bdb_file_content';
  IF NOT FOUND THEN
    RETURN; /* bdb_file_content has been removed */
  END IF;
  PERFORM true FROM information_schema.columns
    WHERE table_name = 'bdb_file_content' AND column_name = 'size';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding column "size" to bdb_files';
    ALTER TABLE bdb_file_content ADD COLUMN size INTEGER DEFAULT 0;
    ALTER TABLE bdb_file_content ALTER COLUMN size DROP DEFAULT;
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_del_bdb_nodes_indexes() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM pg_class WHERE relname = 'bdb_nodes_name_key';
  IF FOUND THEN
    RAISE NOTICE 'removing index bdb_nodes_name_key';
    DROP INDEX bdb_nodes_name_key;
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_add_bdb_nodes_indexes() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM pg_class WHERE relname = 'bdb_nodes_parent_id_name_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_nodes_parent_id_name_key';
    CREATE UNIQUE INDEX bdb_nodes_parent_id_name_key ON bdb_nodes(parent_id, name);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_nodes_id_name_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_nodes_id_name_key';
    CREATE INDEX bdb_nodes_id_name_key ON bdb_nodes(id, name);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_nodes_file_id_name_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_nodes_file_id_name_key';
    CREATE INDEX bdb_nodes_file_id_name_key ON bdb_nodes(file_id, name);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_add_bdb_files_indexes() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_stored_at_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_files_stored_at_key';
    CREATE INDEX bdb_files_stored_at_key ON bdb_files(stored_at);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_what_object_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_files_what_object_key';
    CREATE INDEX bdb_files_what_object_key ON bdb_files(what_object);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_what_date_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_files_what_date_key';
    CREATE INDEX bdb_files_what_date_key ON bdb_files(what_date);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_what_time_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_files_what_time_key';
    CREATE INDEX bdb_files_what_time_key ON bdb_files(what_time);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_combined_datetime_key';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding index bdb_files_combined_datetime_key';
    CREATE INDEX bdb_files_combined_datetime_key ON bdb_files((what_date + what_time));
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_add_source_dkvir() RETURNS VOID AS $$
DECLARE
  src_id integer;
BEGIN
  SELECT id INTO src_id FROM bdb_sources WHERE name = 'dkvir';
  IF NOT FOUND THEN
    RAISE NOTICE 'adding source dkvir';
    INSERT INTO bdb_sources(name) VALUES ('dkvir') RETURNING id INTO src_id;
    INSERT INTO bdb_source_kvs(source_id, key, value) VALUES (src_id, 'PLC', 'Virring');
    INSERT INTO bdb_source_kvs(source_id, key, value) VALUES (src_id, 'WMO', '06103');
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_add_sources_nod_key() RETURNS VOID AS $$
DECLARE
  source RECORD;
BEGIN
  FOR source IN SELECT * FROM bdb_sources WHERE id < 54 LOOP
    PERFORM true FROM bdb_source_kvs WHERE key = 'NOD' AND source_id = source.id;
    IF NOT FOUND THEN
      RAISE NOTICE 'adding NOD=% to bdb_source_kvs', source.name;
      INSERT INTO bdb_source_kvs(source_id, key, value) VALUES (source.id, 'NOD', source.name);
    END IF;
  END LOOP;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION bdbupgrade_rename_source_eetal_to_eehar() RETURNS VOID AS $$
BEGIN
  UPDATE bdb_sources
    SET name = 'eehar'
    WHERE id = 13 AND name = 'eetal';
  UPDATE bdb_source_kvs
    SET value = 'eehar'
    WHERE source_id = 13 AND key = 'NOD' AND value = 'eetal';
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_update_danish_source_wmo_numbers() RETURNS VOID AS $$
BEGIN
  UPDATE bdb_source_kvs
    SET value = '06173'
    WHERE source_id = 10 AND key = 'WMO' AND value = '06180';
  UPDATE bdb_source_kvs
    SET value = '06096'
    WHERE source_id = 11 AND key = 'WMO' AND value = '0';
  UPDATE bdb_source_kvs
    SET value = '06034'
    WHERE source_id = 12 AND key = 'WMO' AND value = '0';
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_merge_file_content_to_files() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM information_schema.tables WHERE table_name = 'bdb_file_content';
  IF FOUND THEN
    RAISE NOTICE 'merging bdb_file_content to bdb_files (this may take a while)';
    ALTER TABLE bdb_files ADD COLUMN size INTEGER DEFAULT 0;
    ALTER TABLE bdb_files ADD COLUMN lo_id INTEGER DEFAULT NULL;
    UPDATE bdb_files
      SET size = c.size, lo_id = c.lo_id
      FROM bdb_file_content AS c
      WHERE id = c.file_id;
    ALTER TABLE bdb_files ALTER COLUMN size DROP DEFAULT;
    ALTER TABLE bdb_files ALTER COLUMN size SET NOT NULL;
    CREATE RULE remove_lo AS ON DELETE TO bdb_files
        DO SELECT lo_unlink(OLD.lo_id);
    DROP TABLE bdb_file_content;
  END IF;
END;
$$ LANGUAGE plpgsql;

SELECT bdbupgrade_add_size_to_bdb_file_content();
SELECT bdbupgrade_del_bdb_nodes_indexes();
SELECT bdbupgrade_add_bdb_nodes_indexes();
SELECT bdbupgrade_add_bdb_files_indexes();
SELECT bdbupgrade_merge_file_content_to_files();
SELECT restart_seq_with_max('bdb_sources', 'id');
SELECT bdbupgrade_add_source_dkvir();
SELECT bdbupgrade_add_sources_nod_key();
SELECT bdbupgrade_rename_source_eetal_to_eehar();
SELECT bdbupgrade_update_danish_source_wmo_numbers();

DROP FUNCTION bdbupgrade_add_size_to_bdb_file_content();
DROP FUNCTION bdbupgrade_del_bdb_nodes_indexes();
DROP FUNCTION bdbupgrade_add_bdb_nodes_indexes();
DROP FUNCTION bdbupgrade_add_bdb_files_indexes();
DROP FUNCTION bdbupgrade_add_source_dkvir();
DROP FUNCTION bdbupgrade_add_sources_nod_key();
DROP FUNCTION bdbupgrade_rename_source_eetal_to_eehar();
DROP FUNCTION bdbupgrade_update_danish_source_wmo_numbers();
DROP FUNCTION bdbupgrade_merge_file_content_to_files();
DROP FUNCTION restart_seq_with_max(TEXT, TEXT);
DROP FUNCTION make_plpgsql();
