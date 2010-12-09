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

SELECT bdbupgrade_add_size_to_bdb_file_content();

DROP FUNCTION bdbupgrade_add_size_to_bdb_file_content();
DROP FUNCTION make_plpgsql();