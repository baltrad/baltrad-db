
CREATE TABLE bdb_attribute_groups (
	id INTEGER NOT NULL, 
	name INTEGER NOT NULL, 
	PRIMARY KEY (id)
)

;

CREATE TABLE bdb_attributes (
	id INTEGER NOT NULL, 
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
	id INTEGER NOT NULL, 
	node_id TEXT NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (node_id)
)

;

CREATE TABLE bdb_files (
	id INTEGER NOT NULL, 
	hash_type TEXT NOT NULL, 
	unique_id TEXT NOT NULL, 
	path TEXT NOT NULL, 
	proposed_filename TEXT NOT NULL, 
	filename_version INTEGER NOT NULL, 
	object TEXT NOT NULL, 
	n_date DATE NOT NULL, 
	n_time TIME NOT NULL, 
	source_id INTEGER NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (proposed_filename, filename_version), 
	 UNIQUE (unique_id), 
	 FOREIGN KEY(source_id) REFERENCES bdb_sources (id), 
	 UNIQUE (path)
)

;

CREATE TABLE bdb_groups (
	id INTEGER NOT NULL, 
	parent_id INTEGER, 
	name TEXT NOT NULL, 
	product TEXT, 
	startdate DATE, 
	starttime TIME, 
	enddate DATE, 
	endtime TIME, 
	file_id INTEGER NOT NULL, 
	PRIMARY KEY (id), 
	 FOREIGN KEY(parent_id) REFERENCES bdb_groups (id), 
	 FOREIGN KEY(file_id) REFERENCES bdb_files (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_time (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value TIME NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id)
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

CREATE TABLE bdb_invalid_attributes (
	name TEXT NOT NULL, 
	group_id INTEGER NOT NULL, 
	PRIMARY KEY (name, group_id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_int (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value INTEGER NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE
)

;

CREATE TABLE bdb_attribute_values_real (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value FLOAT NOT NULL, 
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
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id)
)

;

CREATE TABLE bdb_attribute_values_date (
	attribute_id INTEGER NOT NULL, 
	group_id INTEGER NOT NULL, 
	value DATE NOT NULL, 
	PRIMARY KEY (attribute_id, group_id), 
	 FOREIGN KEY(group_id) REFERENCES bdb_groups (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES bdb_attributes (id)
)

;

CREATE TABLE bdb_source_centres (
	id INTEGER NOT NULL, 
	originating_centre INTEGER NOT NULL, 
	country_code INTEGER NOT NULL, 
	wmo_cccc VARCHAR(4) NOT NULL, 
	PRIMARY KEY (id), 
	 FOREIGN KEY(id) REFERENCES bdb_sources (id), 
	 UNIQUE (wmo_cccc), 
	 UNIQUE (originating_centre), 
	 UNIQUE (country_code)
)

;

CREATE TABLE bdb_source_radars (
	id INTEGER NOT NULL, 
	centre_id INTEGER NOT NULL, 
	radar_site TEXT, 
	wmo_code INTEGER, 
	place TEXT, 
	PRIMARY KEY (id), 
	 FOREIGN KEY(centre_id) REFERENCES bdb_source_centres (id), 
	 FOREIGN KEY(id) REFERENCES bdb_sources (id), 
	 UNIQUE (radar_site), 
	 UNIQUE (wmo_code), 
	 UNIQUE (place)
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
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (99, 'what/product', 'string', 'bdb_groups', 'product', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (100, 'what/quantity', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (101, 'what/startdate', 'date', 'bdb_groups', 'startdate', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (102, 'what/starttime', 'time', 'bdb_groups', 'starttime', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (103, 'what/enddate', 'date', 'bdb_groups', 'enddate', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (104, 'what/endtime', 'time', 'bdb_groups', 'endtime', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (105, 'what/gain', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (106, 'what/offset', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (107, 'what/nodata', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (108, 'what/undetect', 'real', 'bdb_attribute_values_real', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (109, 'CLASS', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (110, 'IMAGE_VERSION', 'string', 'bdb_attribute_values_str', 'value', False);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (111, 'path', 'string', 'bdb_files', 'path', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (112, 'file_id', 'int', 'bdb_files', 'id', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (113, 'src_WMO', 'int', 'bdb_source_radars', 'wmo_code', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (114, 'src_RAD', 'string', 'bdb_source_radars', 'radar_site', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (115, 'src_ORG', 'int', 'bdb_source_centres', 'originating_centre', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (116, 'src_CTY', 'int', 'bdb_source_centres', 'country_code', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (117, 'src_PLC', 'string', 'bdb_source_radars', 'place', True);
INSERT INTO bdb_attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (118, 'src_node', 'string', 'bdb_sources', 'node_id', True);
INSERT INTO bdb_sources (id, node_id) VALUES (1, 'nl');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (1, 99, 632, 'EHDB');
INSERT INTO bdb_sources (id, node_id) VALUES (2, 'dk');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (2, 94, 611, 'EKMI');
INSERT INTO bdb_sources (id, node_id) VALUES (3, 'no');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (3, 88, 633, 'ENMI');
INSERT INTO bdb_sources (id, node_id) VALUES (4, 'ee');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (4, 231, 612, 'EEMH');
INSERT INTO bdb_sources (id, node_id) VALUES (5, 'lv');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (5, 236, 625, 'EVRR');
INSERT INTO bdb_sources (id, node_id) VALUES (6, 'se');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (6, 82, 643, 'ESWI');
INSERT INTO bdb_sources (id, node_id) VALUES (7, 'fi');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (7, 86, 613, 'EFKL');
INSERT INTO bdb_sources (id, node_id) VALUES (8, 'pl');
INSERT INTO bdb_source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (8, 220, 634, 'SOWR');
INSERT INTO bdb_sources (id, node_id) VALUES (9, 'dkbor');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (9, 2, 'DN44', NULL, 'Bornholm');
INSERT INTO bdb_sources (id, node_id) VALUES (10, 'dkste');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (10, 2, 'DN41', 6180, 'Stevns');
INSERT INTO bdb_sources (id, node_id) VALUES (11, 'dkrom');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (11, 2, 'DN42', NULL, 'Rømø');
INSERT INTO bdb_sources (id, node_id) VALUES (12, 'dksin');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (12, 2, 'DN43', NULL, 'Sindal');
INSERT INTO bdb_sources (id, node_id) VALUES (13, 'eetal');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (13, 4, 'EE-HARKU', 26038, 'Harku');
INSERT INTO bdb_sources (id, node_id) VALUES (14, 'eesur');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (14, 4, 'EE-SYRGAVERE', 26232, 'Sürgavere');
INSERT INTO bdb_sources (id, node_id) VALUES (15, 'fivan');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (15, 7, 'FI42', 2975, 'Vantaa');
INSERT INTO bdb_sources (id, node_id) VALUES (16, 'fiika');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (16, 7, 'FI43', 2941, 'Ikaalinen');
INSERT INTO bdb_sources (id, node_id) VALUES (17, 'fianj');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (17, 7, 'FI44', 2954, 'Anjalankoski');
INSERT INTO bdb_sources (id, node_id) VALUES (18, 'fikuo');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (18, 7, 'FI45', 2918, 'Kuopio');
INSERT INTO bdb_sources (id, node_id) VALUES (19, 'fikor');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (19, 7, 'FI46', 2933, 'Korpo');
INSERT INTO bdb_sources (id, node_id) VALUES (20, 'fiuta');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (20, 7, 'FI47', 2870, 'Utajärvi');
INSERT INTO bdb_sources (id, node_id) VALUES (21, 'filuo');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (21, 7, 'FI48', 2840, 'Luosto');
INSERT INTO bdb_sources (id, node_id) VALUES (22, 'fivim');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (22, 7, 'FI49', 2925, 'Vimpeli');
INSERT INTO bdb_sources (id, node_id) VALUES (23, 'lvrix');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (23, 5, 'LVaa', 26422, 'Riga');
INSERT INTO bdb_sources (id, node_id) VALUES (24, 'nldbl');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (24, 1, 'NL50', 6260, 'De Bilt');
INSERT INTO bdb_sources (id, node_id) VALUES (25, 'nldhl');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (25, 1, 'NL51', 6234, 'Den Helder');
INSERT INTO bdb_sources (id, node_id) VALUES (26, 'norst');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (26, 3, 'NOaa', 1104, 'Røst');
INSERT INTO bdb_sources (id, node_id) VALUES (27, 'nobom');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (27, 3, 'NOab', 1405, 'Bømlo');
INSERT INTO bdb_sources (id, node_id) VALUES (28, 'nosol');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (28, 3, 'NO41', 1499, 'Oslo');
INSERT INTO bdb_sources (id, node_id) VALUES (29, 'nohgb');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (29, 3, 'NO42', 1438, 'Hægebostad');
INSERT INTO bdb_sources (id, node_id) VALUES (30, 'norsa');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (30, 3, 'NO43', 1247, 'Rissa');
INSERT INTO bdb_sources (id, node_id) VALUES (31, 'noand');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (31, 3, 'NOac', 1018, 'Andøya');
INSERT INTO bdb_sources (id, node_id) VALUES (32, 'nohas');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (32, 3, 'NOad', 1042, 'Hasvik');
INSERT INTO bdb_sources (id, node_id) VALUES (33, 'plleg');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (33, 8, 'PL41', 12374, 'Legionowo');
INSERT INTO bdb_sources (id, node_id) VALUES (34, 'plram');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (34, 8, 'PL42', 12514, 'Ramża');
INSERT INTO bdb_sources (id, node_id) VALUES (35, 'plpas');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (35, 8, 'PL43', 12544, 'Pastewnik');
INSERT INTO bdb_sources (id, node_id) VALUES (36, 'plrze');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (36, 8, 'PL44', 12579, 'Rzeszów');
INSERT INTO bdb_sources (id, node_id) VALUES (37, 'plpoz');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (37, 8, 'PL45', 12331, 'Poznań');
INSERT INTO bdb_sources (id, node_id) VALUES (38, 'plswi');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (38, 8, 'PL46', 12220, 'Świdwin');
INSERT INTO bdb_sources (id, node_id) VALUES (39, 'plgda');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (39, 8, 'PL47', 12151, 'Gdańsk');
INSERT INTO bdb_sources (id, node_id) VALUES (40, 'plbrz');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (40, 8, 'PL48', 12568, 'Brzuchania');
INSERT INTO bdb_sources (id, node_id) VALUES (41, 'sekir');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (41, 6, 'SE40', 2032, 'Kiruna');
INSERT INTO bdb_sources (id, node_id) VALUES (42, 'selul');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (42, 6, 'SE41', 2092, 'Luleå');
INSERT INTO bdb_sources (id, node_id) VALUES (43, 'seosu');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (43, 6, 'SE42', 2200, 'Östersund');
INSERT INTO bdb_sources (id, node_id) VALUES (44, 'seovi');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (44, 6, 'SE43', 2262, 'Örnsköldsvik');
INSERT INTO bdb_sources (id, node_id) VALUES (45, 'sehud');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (45, 6, 'SE44', 2334, 'Hudiksvall');
INSERT INTO bdb_sources (id, node_id) VALUES (46, 'selek');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (46, 6, 'SE45', 2430, 'Leksand');
INSERT INTO bdb_sources (id, node_id) VALUES (47, 'searl');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (47, 6, 'SE46', 2451, 'Arlanda');
INSERT INTO bdb_sources (id, node_id) VALUES (48, 'sease');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (48, 6, 'SE47', 2588, 'Ase');
INSERT INTO bdb_sources (id, node_id) VALUES (49, 'sevil');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (49, 6, 'SE48', 2570, 'Vilebo');
INSERT INTO bdb_sources (id, node_id) VALUES (50, 'sevar');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (50, 6, 'SE49', 2600, 'Vara');
INSERT INTO bdb_sources (id, node_id) VALUES (51, 'seang');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (51, 6, 'SE50', 2606, 'Ängelholm');
INSERT INTO bdb_sources (id, node_id) VALUES (52, 'sekkr');
INSERT INTO bdb_source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (52, 6, 'SE51', 2666, 'Karlskrona');
