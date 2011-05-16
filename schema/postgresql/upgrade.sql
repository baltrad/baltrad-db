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

SELECT bdbupgrade_add_size_to_bdb_file_content();
SELECT bdbupgrade_del_bdb_nodes_indexes();
SELECT bdbupgrade_add_bdb_nodes_indexes();
SELECT bdbupgrade_add_bdb_files_indexes();
SELECT restart_seq_with_max('bdb_sources', 'id');
SELECT bdbupgrade_add_source_dkvir();

DROP FUNCTION bdbupgrade_add_size_to_bdb_file_content();
DROP FUNCTION bdbupgrade_del_bdb_nodes_indexes();
DROP FUNCTION bdbupgrade_add_bdb_nodes_indexes();
DROP FUNCTION bdbupgrade_add_bdb_files_indexes();
DROP FUNCTION bdbupgrade_add_source_dkvir();
DROP FUNCTION restart_seq_with_max(TEXT, TEXT);
DROP FUNCTION make_plpgsql();
