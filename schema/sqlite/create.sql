
CREATE TABLE attribute_groups (
	id INTEGER NOT NULL, 
	name INTEGER NOT NULL, 
	PRIMARY KEY (id)
)

;

CREATE TABLE attributes (
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

CREATE TABLE sources (
	id INTEGER NOT NULL, 
	node_id TEXT NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (node_id)
)

;

CREATE TABLE files (
	id INTEGER NOT NULL, 
	unique_id TEXT NOT NULL, 
	path TEXT NOT NULL, 
	proposed_filename TEXT NOT NULL, 
	filename_version INTEGER NOT NULL, 
	object TEXT NOT NULL, 
	n_date DATE NOT NULL, 
	n_time TIME NOT NULL, 
	source_id INTEGER NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (path), 
	 UNIQUE (unique_id), 
	 FOREIGN KEY(source_id) REFERENCES sources (id), 
	 UNIQUE (proposed_filename, filename_version)
)

;

CREATE TABLE data_objects (
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
	 FOREIGN KEY(parent_id) REFERENCES data_objects (id), 
	 FOREIGN KEY(file_id) REFERENCES files (id) ON DELETE CASCADE
)

;

CREATE TABLE attribute_values_str (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value TEXT NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id)
)

;

CREATE TABLE attribute_values_int (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value INTEGER NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE
)

;

CREATE TABLE attribute_values_bool (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value BOOLEAN NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id)
)

;

CREATE TABLE attribute_values_time (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value TIME NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE
)

;

CREATE TABLE attribute_values_date (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value DATE NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id)
)

;

CREATE TABLE attribute_values_real (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value FLOAT NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE, 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id)
)

;

CREATE TABLE source_centres (
	id INTEGER NOT NULL, 
	originating_centre INTEGER NOT NULL, 
	country_code INTEGER NOT NULL, 
	wmo_cccc VARCHAR(4) NOT NULL, 
	PRIMARY KEY (id), 
	 FOREIGN KEY(id) REFERENCES sources (id), 
	 UNIQUE (originating_centre), 
	 UNIQUE (country_code), 
	 UNIQUE (wmo_cccc)
)

;

CREATE TABLE source_radars (
	id INTEGER NOT NULL, 
	centre_id INTEGER NOT NULL, 
	radar_site TEXT, 
	wmo_code INTEGER, 
	place TEXT, 
	PRIMARY KEY (id), 
	 UNIQUE (place), 
	 FOREIGN KEY(id) REFERENCES sources (id), 
	 UNIQUE (wmo_code), 
	 UNIQUE (radar_site), 
	 FOREIGN KEY(centre_id) REFERENCES source_centres (id)
)

;
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (1, 'Conventions', 'string', 'attribute_values_str', 'value', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (2, 'what/object', 'string', 'files', 'object', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (3, 'what/version', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (4, 'what/date', 'date', 'files', 'n_date', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (5, 'what/time', 'time', 'files', 'n_time', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (6, 'what/source', 'string', 'attribute_values_str', 'value', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (7, 'where/lon', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (8, 'where/lat', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (9, 'where/height', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (10, 'where/elangle', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (11, 'where/nbins', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (12, 'where/rstart', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (13, 'where/rscale', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (14, 'where/nrays', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (15, 'where/a1gate', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (16, 'where/startaz', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (17, 'where/stopaz', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (18, 'where/projdef', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (19, 'where/xsize', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (20, 'where/ysize', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (21, 'where/xscale', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (22, 'where/yscale', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (23, 'where/LL_lon', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (24, 'where/LL_lat', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (25, 'where/UL_lon', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (26, 'where/UL_lat', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (27, 'where/UR_lon', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (28, 'where/UR_lat', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (29, 'where/LR_lon', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (30, 'where/LR_lat', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (31, 'where/minheight', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (32, 'where/maxheight', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (33, 'where/az_angle', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (34, 'where/angles', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (35, 'where/range', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (36, 'where/start_lon', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (37, 'where/start_lat', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (38, 'where/stop_lon', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (39, 'where/stop_lat', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (40, 'where/levels', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (41, 'where/interval', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (42, 'how/task', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (43, 'how/startepochs', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (44, 'how/endepochs', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (45, 'how/system', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (46, 'how/software', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (47, 'how/sw_version', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (48, 'how/zr_a', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (49, 'how/zr_b', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (50, 'how/kr_a', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (51, 'how/kr_b', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (52, 'how/simulated', 'bool', 'attribute_values_bool', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (53, 'how/beamwidth', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (54, 'how/wavelength', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (55, 'how/rpm', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (56, 'how/pulsewidth', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (57, 'how/lowprf', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (58, 'how/highprf', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (59, 'how/azmethod', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (60, 'how/binmethod', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (61, 'how/azangles', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (62, 'how/elangles', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (63, 'how/aztimes', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (64, 'how/angles', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (65, 'how/arotation', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (66, 'how/camethod', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (67, 'how/nodes', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (68, 'how/ACCnum', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (69, 'how/minrange', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (70, 'how/maxrange', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (71, 'how/NI', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (72, 'how/dealiased', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (73, 'how/pointaccEL', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (74, 'how/pointaccAZ', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (75, 'how/malfunc', 'bool', 'attribute_values_bool', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (76, 'how/radar_msg', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (77, 'how/radhoriz', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (78, 'how/MDS', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (79, 'how/OUR', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (80, 'how/Dclutter', 'sequence', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (81, 'how/comment', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (82, 'how/SQI', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (83, 'how/CSR', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (84, 'how/LOG', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (85, 'how/VPRCorr', 'bool', 'attribute_values_bool', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (86, 'how/freeze', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (87, 'how/min', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (88, 'how/max', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (89, 'how/step', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (90, 'how/levels', 'int', 'attribute_values_int', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (91, 'how/peakpwr', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (92, 'how/avgpwr', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (93, 'how/dynrange', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (94, 'how/RAC', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (95, 'how/BBC', 'bool', 'attribute_values_bool', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (96, 'how/PAC', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (97, 'how/S2N', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (98, 'how/polarization', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (99, 'what/product', 'string', 'data_objects', 'product', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (100, 'what/quantity', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (101, 'what/startdate', 'date', 'data_objects', 'startdate', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (102, 'what/starttime', 'time', 'data_objects', 'starttime', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (103, 'what/enddate', 'date', 'data_objects', 'enddate', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (104, 'what/endtime', 'time', 'data_objects', 'endtime', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (105, 'what/gain', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (106, 'what/offset', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (107, 'what/nodata', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (108, 'what/undetect', 'real', 'attribute_values_real', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (109, 'CLASS', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (110, 'IMAGE_VERSION', 'string', 'attribute_values_str', 'value', False);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (111, 'path', 'string', 'files', 'path', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (112, 'file_id', 'int', 'files', 'id', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (113, 'src_WMO', 'int', 'source_radars', 'wmo_code', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (114, 'src_RAD', 'string', 'source_radars', 'radar_site', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (115, 'src_ORG', 'int', 'source_centres', 'originating_centre', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (116, 'src_CTY', 'int', 'source_centres', 'country_code', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (117, 'src_PLC', 'string', 'source_radars', 'place', True);
INSERT INTO attributes (id, name, converter, storage_table, storage_column, ignore_in_hash) VALUES (118, 'src_node', 'string', 'sources', 'node_id', True);
INSERT INTO sources (id, node_id) VALUES (1, 'nl');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (1, 99, 632, 'EHDB');
INSERT INTO sources (id, node_id) VALUES (2, 'dk');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (2, 94, 611, 'EKMI');
INSERT INTO sources (id, node_id) VALUES (3, 'no');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (3, 88, 633, 'ENMI');
INSERT INTO sources (id, node_id) VALUES (4, 'ee');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (4, 231, 612, 'EEMH');
INSERT INTO sources (id, node_id) VALUES (5, 'lv');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (5, 236, 625, 'EVRR');
INSERT INTO sources (id, node_id) VALUES (6, 'se');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (6, 82, 643, 'ESWI');
INSERT INTO sources (id, node_id) VALUES (7, 'fi');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (7, 86, 613, 'EFKL');
INSERT INTO sources (id, node_id) VALUES (8, 'pl');
INSERT INTO source_centres (id, originating_centre, country_code, wmo_cccc) VALUES (8, 220, 634, 'SOWR');
INSERT INTO sources (id, node_id) VALUES (9, 'dkbor');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (9, 2, 'DN44', NULL, 'Bornholm');
INSERT INTO sources (id, node_id) VALUES (10, 'dkste');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (10, 2, 'DN41', 6180, 'Stevns');
INSERT INTO sources (id, node_id) VALUES (11, 'dkrom');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (11, 2, 'DN42', NULL, 'Rømø');
INSERT INTO sources (id, node_id) VALUES (12, 'dksin');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (12, 2, 'DN43', NULL, 'Sindal');
INSERT INTO sources (id, node_id) VALUES (13, 'eetal');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (13, 4, 'EE-HARKU', 26038, 'Harku');
INSERT INTO sources (id, node_id) VALUES (14, 'eesur');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (14, 4, 'EE-SYRGAVERE', 26232, 'Sürgavere');
INSERT INTO sources (id, node_id) VALUES (15, 'fivan');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (15, 7, 'FI42', 2975, 'Vantaa');
INSERT INTO sources (id, node_id) VALUES (16, 'fiika');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (16, 7, 'FI43', 2941, 'Ikaalinen');
INSERT INTO sources (id, node_id) VALUES (17, 'fianj');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (17, 7, 'FI44', 2954, 'Anjalankoski');
INSERT INTO sources (id, node_id) VALUES (18, 'fikuo');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (18, 7, 'FI45', 2918, 'Kuopio');
INSERT INTO sources (id, node_id) VALUES (19, 'fikor');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (19, 7, 'FI46', 2933, 'Korpo');
INSERT INTO sources (id, node_id) VALUES (20, 'fiuta');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (20, 7, 'FI47', 2870, 'Utajärvi');
INSERT INTO sources (id, node_id) VALUES (21, 'filuo');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (21, 7, 'FI48', 2840, 'Luosto');
INSERT INTO sources (id, node_id) VALUES (22, 'fivim');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (22, 7, 'FI49', 2925, 'Vimpeli');
INSERT INTO sources (id, node_id) VALUES (23, 'lvrix');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (23, 5, 'LVaa', 26422, 'Riga');
INSERT INTO sources (id, node_id) VALUES (24, 'nldbl');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (24, 1, 'NL50', 6260, 'De Bilt');
INSERT INTO sources (id, node_id) VALUES (25, 'nldhl');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (25, 1, 'NL51', 6234, 'Den Helder');
INSERT INTO sources (id, node_id) VALUES (26, 'norst');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (26, 3, 'NOaa', 1104, 'Røst');
INSERT INTO sources (id, node_id) VALUES (27, 'nobom');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (27, 3, 'NOab', 1405, 'Bømlo');
INSERT INTO sources (id, node_id) VALUES (28, 'nosol');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (28, 3, 'NO41', 1499, 'Oslo');
INSERT INTO sources (id, node_id) VALUES (29, 'nohgb');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (29, 3, 'NO42', 1438, 'Hægebostad');
INSERT INTO sources (id, node_id) VALUES (30, 'norsa');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (30, 3, 'NO43', 1247, 'Rissa');
INSERT INTO sources (id, node_id) VALUES (31, 'noand');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (31, 3, 'NOac', 1018, 'Andøya');
INSERT INTO sources (id, node_id) VALUES (32, 'nohas');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (32, 3, 'NOad', 1042, 'Hasvik');
INSERT INTO sources (id, node_id) VALUES (33, 'plleg');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (33, 8, 'PL41', 12374, 'Legionowo');
INSERT INTO sources (id, node_id) VALUES (34, 'plram');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (34, 8, 'PL42', 12514, 'Ramża');
INSERT INTO sources (id, node_id) VALUES (35, 'plpas');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (35, 8, 'PL43', 12544, 'Pastewnik');
INSERT INTO sources (id, node_id) VALUES (36, 'plrze');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (36, 8, 'PL44', 12579, 'Rzeszów');
INSERT INTO sources (id, node_id) VALUES (37, 'plpoz');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (37, 8, 'PL45', 12331, 'Poznań');
INSERT INTO sources (id, node_id) VALUES (38, 'plswi');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (38, 8, 'PL46', 12220, 'Świdwin');
INSERT INTO sources (id, node_id) VALUES (39, 'plgda');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (39, 8, 'PL47', 12151, 'Gdańsk');
INSERT INTO sources (id, node_id) VALUES (40, 'plbrz');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (40, 8, 'PL48', 12568, 'Brzuchania');
INSERT INTO sources (id, node_id) VALUES (41, 'sekir');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (41, 6, 'SE40', 2032, 'Kiruna');
INSERT INTO sources (id, node_id) VALUES (42, 'selul');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (42, 6, 'SE41', 2092, 'Luleå');
INSERT INTO sources (id, node_id) VALUES (43, 'seosu');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (43, 6, 'SE42', 2200, 'Östersund');
INSERT INTO sources (id, node_id) VALUES (44, 'seovi');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (44, 6, 'SE43', 2262, 'Örnsköldsvik');
INSERT INTO sources (id, node_id) VALUES (45, 'sehud');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (45, 6, 'SE44', 2334, 'Hudiksvall');
INSERT INTO sources (id, node_id) VALUES (46, 'selek');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (46, 6, 'SE45', 2430, 'Leksand');
INSERT INTO sources (id, node_id) VALUES (47, 'searl');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (47, 6, 'SE46', 2451, 'Arlanda');
INSERT INTO sources (id, node_id) VALUES (48, 'sease');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (48, 6, 'SE47', 2588, 'Ase');
INSERT INTO sources (id, node_id) VALUES (49, 'sevil');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (49, 6, 'SE48', 2570, 'Vilebo');
INSERT INTO sources (id, node_id) VALUES (50, 'sevar');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (50, 6, 'SE49', 2600, 'Vara');
INSERT INTO sources (id, node_id) VALUES (51, 'seang');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (51, 6, 'SE50', 2606, 'Ängelholm');
INSERT INTO sources (id, node_id) VALUES (52, 'sekkr');
INSERT INTO source_radars (id, centre_id, radar_site, wmo_code, place) VALUES (52, 6, 'SE51', 2666, 'Karlskrona');
