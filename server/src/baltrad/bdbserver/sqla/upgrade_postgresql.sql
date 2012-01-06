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


CREATE OR REPLACE FUNCTION bdbupgrade_split_stored_at() RETURNS VOID AS $$
BEGIN
  PERFORM true FROM information_schema.columns
    WHERE table_name = 'bdb_files' AND column_name = 'stored_at';
  IF FOUND THEN
    RAISE NOTICE 'splitting stored_at to stored_date and stored_time';
    ALTER TABLE bdb_files ADD COLUMN stored_date DATE DEFAULT NULL;
    ALTER TABLE bdb_files ADD COLUMN stored_time TIME DEFAULT NULL;
    UPDATE bdb_files
      SET stored_date = stored_at::date, stored_time = stored_at::time;
    ALTER TABLE bdb_files DROP COLUMN stored_at;
    ALTER TABLE bdb_files ALTER COLUMN stored_date DROP DEFAULT;
    ALTER TABLE bdb_files ALTER COLUMN stored_date DROP NOT NULL;
    ALTER TABLE bdb_files ALTER COLUMN stored_time DROP DEFAULT;
    ALTER TABLE bdb_files ALTER COLUMN stored_time DROP NOT NULL;
    CREATE INDEX bdb_files_stored_datetime_key ON bdb_files((stored_date + stored_time));
  END IF;
END;
$$ LANGUAGE plpgsql;

SELECT bdbupgrade_add_size_to_bdb_file_content();
SELECT bdbupgrade_del_bdb_nodes_indexes();
SELECT bdbupgrade_add_bdb_nodes_indexes();
SELECT bdbupgrade_add_bdb_files_indexes();
SELECT bdbupgrade_merge_file_content_to_files();
SELECT bdbupgrade_split_stored_at();

DROP FUNCTION bdbupgrade_add_size_to_bdb_file_content();
DROP FUNCTION bdbupgrade_del_bdb_nodes_indexes();
DROP FUNCTION bdbupgrade_add_bdb_nodes_indexes();
DROP FUNCTION bdbupgrade_add_bdb_files_indexes();
DROP FUNCTION bdbupgrade_merge_file_content_to_files();
DROP FUNCTION bdbupgrade_split_stored_at();
DROP FUNCTION restart_seq_with_max(TEXT, TEXT);
DROP FUNCTION make_plpgsql();
