
CREATE TABLE bdb_attribute_groups (
	id SERIAL NOT NULL, 
	name INTEGER NOT NULL, 
	PRIMARY KEY (id)
)

;

CREATE TABLE bdb_attributes (
	id SERIAL NOT NULL, 
	name TEXT NOT NULL, 
	converter TEXT NOT NULL, 
	storage_table TEXT NOT NULL, 
	storage_column TEXT NOT NULL, 
	ignore_in_hash BOOLEAN NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (name)
)

;

CREATE TABLE bdb_sources (
	id SERIAL NOT NULL, 
	name TEXT NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (name)
)

;

CREATE TABLE bdb_files (
	id SERIAL NOT NULL, 
	hash_type TEXT NOT NULL, 
	unique_id TEXT NOT NULL, 
	object TEXT NOT NULL, 
	n_date DATE NOT NULL, 
	n_time TIME WITHOUT TIME ZONE NOT NULL, 
	source_id INTEGER NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (unique_id, source_id), 
	 FOREIGN KEY(source_id) REFERENCES bdb_sources (id)
)

;

CREATE TABLE bdb_file_content (
	file_id INTEGER NOT NULL, 
	lo_id INTEGER, 
	PRIMARY KEY (file_id), 
	 FOREIGN KEY(file_id) REFERENCES bdb_files (id) ON DELETE CASCADE
)

;
CREATE RULE remove_lo AS ON DELETE TO bdb_file_content DO SELECT lo_unlink(OLD.lo_id);

CREATE TABLE bdb_groups (
	id SERIAL NOT NULL, 
	parent_id INTEGER, 
	name TEXT NOT NULL, 
	file_id INTEGER NOT NULL, 
	PRIMARY KEY (id), 
	 FOREIGN KEY(parent_id) REFERENCES bdb_groups (id), 
	 FOREIGN KEY(file_id) REFERENCES bdb_files (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_int (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value BIGINT NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_real (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value DOUBLE PRECISION NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id)
)

;

CREATE TABLE bdb_attribute_values_date (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value DATE NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_bool (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value BOOLEAN NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_invalid_attributes (
	name TEXT NOT NULL, 
	group_id INTEGER NOT NULL, 
	PRIMARY KEY (name, group_id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_str (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value TEXT NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_time (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value TIME WITHOUT TIME ZONE NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_source_kvs (
	source_id INTEGER, 
	key VARCHAR NOT NULL, 
	value VARCHAR NOT NULL, 
	PRIMARY KEY (source_id, key), 
	 FOREIGN KEY(source_id) REFERENCES bdb_sources (id)
)

;
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (1, 'Conventions', 'string', 'bdb_attribute_values_str', 'value', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (2, 'what/object', 'string', 'bdb_files', 'object', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (3, 'what/version', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (4, 'what/date', 'date', 'bdb_files', 'n_date', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (5, 'what/time', 'time', 'bdb_files', 'n_time', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (6, 'what/source', 'string', 'bdb_attribute_values_str', 'value', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (7, 'where/lon', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (8, 'where/lat', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (9, 'where/height', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (10, 'where/elangle', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (11, 'where/nbins', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (12, 'where/rstart', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (13, 'where/rscale', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (14, 'where/nrays', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (15, 'where/a1gate', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (16, 'where/startaz', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (17, 'where/stopaz', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (18, 'where/projdef', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (19, 'where/xsize', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (20, 'where/ysize', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (21, 'where/xscale', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (22, 'where/yscale', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (23, 'where/LL_lon', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (24, 'where/LL_lat', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (25, 'where/UL_lon', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (26, 'where/UL_lat', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (27, 'where/UR_lon', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (28, 'where/UR_lat', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (29, 'where/LR_lon', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (30, 'where/LR_lat', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (31, 'where/minheight', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (32, 'where/maxheight', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (33, 'where/az_angle', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (34, 'where/angles', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (35, 'where/range', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (36, 'where/start_lon', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (37, 'where/start_lat', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (38, 'where/stop_lon', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (39, 'where/stop_lat', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (40, 'where/levels', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (41, 'where/interval', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (42, 'how/task', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (43, 'how/startepochs', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (44, 'how/endepochs', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (45, 'how/system', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (46, 'how/software', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (47, 'how/sw_version', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (48, 'how/zr_a', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (49, 'how/zr_b', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (50, 'how/kr_a', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (51, 'how/kr_b', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (52, 'how/simulated', 'bool', 'bdb_attribute_values_bool', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (53, 'how/beamwidth', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (54, 'how/wavelength', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (55, 'how/rpm', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (56, 'how/pulsewidth', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (57, 'how/lowprf', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (58, 'how/highprf', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (59, 'how/azmethod', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (60, 'how/binmethod', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (61, 'how/azangles', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (62, 'how/elangles', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (63, 'how/aztimes', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (64, 'how/angles', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (65, 'how/arotation', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (66, 'how/camethod', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (67, 'how/nodes', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (68, 'how/ACCnum', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (69, 'how/minrange', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (70, 'how/maxrange', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (71, 'how/NI', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (72, 'how/dealiased', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (73, 'how/pointaccEL', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (74, 'how/pointaccAZ', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (75, 'how/malfunc', 'bool', 'bdb_attribute_values_bool', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (76, 'how/radar_msg', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (77, 'how/radhoriz', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (78, 'how/MDS', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (79, 'how/OUR', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (80, 'how/Dclutter', 'sequence', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (81, 'how/comment', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (82, 'how/SQI', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (83, 'how/CSR', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (84, 'how/LOG', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (85, 'how/VPRCorr', 'bool', 'bdb_attribute_values_bool', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (86, 'how/freeze', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (87, 'how/min', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (88, 'how/max', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (89, 'how/step', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (90, 'how/levels', 'int', 'bdb_attribute_values_int', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (91, 'how/peakpwr', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (92, 'how/avgpwr', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (93, 'how/dynrange', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (94, 'how/RAC', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (95, 'how/BBC', 'bool', 'bdb_attribute_values_bool', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (96, 'how/PAC', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (97, 'how/S2N', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (98, 'how/polarization', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (99, 'what/product', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (100, 'what/quantity', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (101, 'what/startdate', 'date', 'bdb_attribute_values_date', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (102, 'what/starttime', 'time', 'bdb_attribute_values_time', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (103, 'what/enddate', 'date', 'bdb_attribute_values_date', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (104, 'what/endtime', 'time', 'bdb_attribute_values_time', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (105, 'what/gain', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (106, 'what/offset', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (107, 'what/nodata', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (108, 'what/undetect', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (109, 'CLASS', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (110, 'IMAGE_VERSION', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (111, 'file:id', 'int', 'bdb_files', 'id', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (112, 'what/source:name', 'string', 'bdb_sources', 'name', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (113, 'what/source:node', 'string', 'bdb_sources', 'name', True);
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
