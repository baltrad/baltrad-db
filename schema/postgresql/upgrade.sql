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


CREATE OR REPLACE FUNCTION bdbupgrade_add_size_to_bdb_file_content() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM information_schema.columns
    WHERE table_name = 'bdb_file_content' AND column_name = 'size';
  IF NOT FOUND THEN
    ALTER TABLE bdb_file_content ADD COLUMN size INTEGER DEFAULT 0;
    ALTER TABLE bdb_file_content ALTER COLUMN size DROP DEFAULT;
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_add_bdb_nodes_name_index() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM pg_class WHERE relname = 'bdb_nodes_name_key';
  IF NOT FOUND THEN
    CREATE INDEX bdb_nodes_name_key ON bdb_nodes(name);
  END IF;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION bdbupgrade_add_bdb_files_attribute_indexes() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_stored_at_key';
  IF NOT FOUND THEN
    CREATE INDEX bdb_files_stored_at_key ON bdb_files(stored_at);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_what_object_key';
  IF NOT FOUND THEN
    CREATE INDEX bdb_files_what_object_key ON bdb_files(what_object);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_what_date_key';
  IF NOT FOUND THEN
    CREATE INDEX bdb_files_what_date_key ON bdb_files(what_date);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_what_time_key';
  IF NOT FOUND THEN
    CREATE INDEX bdb_files_what_time_key ON bdb_files(what_time);
  END IF;
  PERFORM true FROM pg_class WHERE relname = 'bdb_files_combined_datetime_key';
  IF NOT FOUND THEN
    CREATE INDEX bdb_files_combined_datetime_key ON bdb_files((what_date + what_time));
  END IF;
END;
$$ LANGUAGE plpgsql;

SELECT bdbupgrade_add_size_to_bdb_file_content();
SELECT bdbupgrade_add_bdb_nodes_name_index();
SELECT bdbupgrade_add_bdb_files_attribute_indexes();

DROP FUNCTION bdbupgrade_add_size_to_bdb_file_content();
DROP FUNCTION bdbupgrade_add_bdb_nodes_name_index();
DROP FUNCTION bdbupgrade_add_bdb_files_attribute_indexes();
DROP FUNCTION make_plpgsql();
