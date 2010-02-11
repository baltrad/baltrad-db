
CREATE TABLE attributes (
	id INTEGER NOT NULL, 
	name TEXT NOT NULL, 
	converter TEXT NOT NULL, 
	storage_table TEXT NOT NULL, 
	storage_column TEXT NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (name)
)

;

CREATE TABLE sources (
	id INTEGER NOT NULL, 
	country_code INTEGER, 
	wmo_code INTEGER, 
	radar_site TEXT, 
	originating_centre INTEGER, 
	node_id TEXT NOT NULL, 
	place TEXT, 
	PRIMARY KEY (id), 
	 UNIQUE (wmo_code), 
	 UNIQUE (country_code), 
	 UNIQUE (radar_site), 
	 UNIQUE (node_id), 
	 UNIQUE (place), 
	 UNIQUE (originating_centre)
)

;

CREATE TABLE files (
	id INTEGER NOT NULL, 
	unique_id TEXT NOT NULL, 
	path TEXT NOT NULL, 
	object TEXT NOT NULL, 
	n_date DATE NOT NULL, 
	n_time TIME NOT NULL, 
	source_id INTEGER NOT NULL, 
	PRIMARY KEY (id), 
	 UNIQUE (unique_id), 
	 UNIQUE (path), 
	 FOREIGN KEY(source_id) REFERENCES sources (id)
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
	 FOREIGN KEY(file_id) REFERENCES files (id) ON DELETE CASCADE, 
	 FOREIGN KEY(parent_id) REFERENCES data_objects (id)
)

;

CREATE TABLE attribute_values_str (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value TEXT NOT NULL, 
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
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE
)

;

CREATE TABLE attribute_values_bool (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value BOOLEAN NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE
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

CREATE TABLE attribute_values_real (
	attribute_id INTEGER NOT NULL, 
	data_object_id INTEGER NOT NULL, 
	value FLOAT NOT NULL, 
	PRIMARY KEY (attribute_id, data_object_id), 
	 FOREIGN KEY(attribute_id) REFERENCES attributes (id), 
	 FOREIGN KEY(data_object_id) REFERENCES data_objects (id) ON DELETE CASCADE
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

CREATE TABLE attribute_groups (
	id INTEGER NOT NULL, 
	name INTEGER NOT NULL, 
	PRIMARY KEY (id)
)

;
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (1, 'Conventions', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (2, 'what/object', 'string', 'files', 'object');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (3, 'what/version', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (4, 'what/date', 'date', 'files', 'n_date');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (5, 'what/time', 'time', 'files', 'n_time');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (6, 'what/source', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (7, 'where/lon', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (8, 'where/lat', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (9, 'where/height', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (10, 'where/elangle', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (11, 'where/nbins', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (12, 'where/rstart', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (13, 'where/rscale', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (14, 'where/nrays', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (15, 'where/a1gate', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (16, 'where/startaz', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (17, 'where/stopaz', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (18, 'where/projdef', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (19, 'where/xsize', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (20, 'where/ysize', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (21, 'where/xscale', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (22, 'where/yscale', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (23, 'where/LL_lon', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (24, 'where/LL_lat', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (25, 'where/UL_lon', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (26, 'where/UL_lat', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (27, 'where/UR_lon', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (28, 'where/UR_lat', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (29, 'where/LR_lon', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (30, 'where/LR_lat', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (31, 'where/minheight', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (32, 'where/maxheight', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (33, 'where/az_angle', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (34, 'where/angles', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (35, 'where/range', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (36, 'where/start_lon', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (37, 'where/start_lat', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (38, 'where/stop_lon', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (39, 'where/stop_lat', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (40, 'where/levels', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (41, 'where/interval', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (42, 'how/task', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (43, 'how/startepochs', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (44, 'how/endepochs', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (45, 'how/system', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (46, 'how/software', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (47, 'how/sw_version', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (48, 'how/zr_a', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (49, 'how/zr_b', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (50, 'how/kr_a', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (51, 'how/kr_b', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (52, 'how/simulated', 'bool', 'attribute_values_bool', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (53, 'how/beamwidth', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (54, 'how/wavelength', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (55, 'how/rpm', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (56, 'how/pulsewidth', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (57, 'how/lowprf', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (58, 'how/highprf', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (59, 'how/azmethod', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (60, 'how/binmethod', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (61, 'how/azangles', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (62, 'how/elangles', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (63, 'how/aztimes', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (64, 'how/angles', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (65, 'how/arotation', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (66, 'how/camethod', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (67, 'how/nodes', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (68, 'how/ACCnum', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (69, 'how/minrange', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (70, 'how/maxrange', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (71, 'how/NI', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (72, 'how/dealiased', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (73, 'how/pointaccEL', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (74, 'how/pointaccAZ', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (75, 'how/malfunc', 'bool', 'attribute_values_bool', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (76, 'how/radar_msg', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (77, 'how/radhoriz', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (78, 'how/MDS', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (79, 'how/OUR', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (80, 'how/Dclutter', 'sequence', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (81, 'how/comment', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (82, 'how/SQI', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (83, 'how/CSR', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (84, 'how/LOG', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (85, 'how/VPRCorr', 'bool', 'attribute_values_bool', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (86, 'how/freeze', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (87, 'how/min', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (88, 'how/max', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (89, 'how/step', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (90, 'how/levels', 'int', 'attribute_values_int', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (91, 'how/peakpwr', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (92, 'how/avgpwr', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (93, 'how/dynrange', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (94, 'how/RAC', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (95, 'how/BBC', 'bool', 'attribute_values_bool', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (96, 'how/PAC', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (97, 'how/S2N', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (98, 'how/polarization', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (99, 'what/product', 'string', 'data_objects', 'product');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (100, 'what/quantity', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (101, 'what/startdate', 'date', 'data_objects', 'startdate');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (102, 'what/starttime', 'time', 'data_objects', 'starttime');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (103, 'what/enddate', 'date', 'data_objects', 'enddate');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (104, 'what/endtime', 'time', 'data_objects', 'endtime');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (105, 'what/gain', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (106, 'what/offset', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (107, 'what/nodata', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (108, 'what/undetect', 'real', 'attribute_values_real', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (109, 'CLASS', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (110, 'IMAGE_VERSION', 'string', 'attribute_values_str', 'value');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (111, 'path', 'string', 'files', 'path');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (112, 'src_WMO', 'int', 'sources', 'wmo_code');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (113, 'src_RAD', 'string', 'sources', 'radar_site');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (114, 'src_ORG', 'int', 'sources', 'originating_centre');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (115, 'src_CTY', 'int', 'sources', 'country_code');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (116, 'src_PLC', 'string', 'sources', 'place');
INSERT INTO attributes (id, name, converter, storage_table, storage_column) VALUES (117, 'src_node', 'string', 'sources', 'node_id');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (1, 632, NULL, NULL, 99, 'nl', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (2, 611, NULL, NULL, 94, 'dk', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (3, 633, NULL, NULL, 88, 'no', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (4, 612, NULL, NULL, 231, 'ee', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (5, 625, NULL, NULL, 236, 'lv', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (6, 643, NULL, NULL, 82, 'se', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (7, 613, NULL, NULL, 86, 'fi', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (8, 634, NULL, NULL, 220, 'pl', NULL);
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (9, NULL, NULL, 'DN44', NULL, 'dkbor', 'Bornholm');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (10, NULL, 6180, 'DN41', NULL, 'dkste', 'Stevns');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (11, NULL, NULL, 'DN42', NULL, 'dkrom', 'Rømø');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (12, NULL, NULL, 'DN43', NULL, 'dksin', 'Sindal');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (13, NULL, 26038, 'EE-HARKU', NULL, 'eetal', 'Harku');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (14, NULL, 26232, 'EE-SYRGAVERE', NULL, 'eesur', 'Sürgavere');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (15, NULL, 2975, 'FI42', NULL, 'fivan', 'Vantaa');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (16, NULL, 2941, 'FI43', NULL, 'fiika', 'Ikaalinen');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (17, NULL, 2954, 'FI44', NULL, 'fianj', 'Anjalankoski');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (18, NULL, 2918, 'FI45', NULL, 'fikuo', 'Kuopio');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (19, NULL, 2933, 'FI46', NULL, 'fikor', 'Korpo');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (20, NULL, 2870, 'FI47', NULL, 'fiuta', 'Utajärvi');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (21, NULL, 2840, 'FI48', NULL, 'filuo', 'Luosto');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (22, NULL, 2925, 'FI49', NULL, 'fivim', 'Vimpeli');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (23, NULL, 26422, 'LVaa', NULL, 'lvrix', 'Riga');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (24, NULL, 6260, 'NL50', NULL, 'nldbl', 'De Bilt');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (25, NULL, 6234, 'NL51', NULL, 'nldhl', 'Den Helder');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (26, NULL, 1104, 'NOaa', NULL, 'norst', 'Røst');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (27, NULL, 1405, 'NOab', NULL, 'nobom', 'Bømlo');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (28, NULL, 1499, 'NO41', NULL, 'nosol', 'Oslo');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (29, NULL, 1438, 'NO42', NULL, 'nohgb', 'Hægebostad');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (30, NULL, 1247, 'NO43', NULL, 'norsa', 'Rissa');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (31, NULL, 1018, 'NOac', NULL, 'noand', 'Andøya');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (32, NULL, 1042, 'NOad', NULL, 'nohas', 'Hasvik');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (33, NULL, 12374, 'PL41', NULL, 'plleg', 'Legionowo');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (34, NULL, 12514, 'PL42', NULL, 'plram', 'Ramza');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (35, NULL, 12544, 'PL43', NULL, 'plpas', 'Pastewnik');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (36, NULL, 12579, 'PL44', NULL, 'plrze', 'Rzeszow');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (37, NULL, 12331, 'PL45', NULL, 'plpoz', 'Poznan');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (38, NULL, 12220, 'PL46', NULL, 'plswi', 'Swidwin');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (39, NULL, 12151, 'PL47', NULL, 'plgda', 'Gdansk');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (40, NULL, 12568, 'PL48', NULL, 'plbrz', 'Brzuchania');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (41, NULL, 2032, 'SE40', NULL, 'sekir', 'Kiruna');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (42, NULL, 2092, 'SE41', NULL, 'selul', 'Luleå');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (43, NULL, 2200, 'SE42', NULL, 'seosu', 'Östersund');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (44, NULL, 2262, 'SE43', NULL, 'seovi', 'Örnsköldsvik');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (45, NULL, 2334, 'SE44', NULL, 'sehud', 'Hudiksvall');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (46, NULL, 2430, 'SE45', NULL, 'selek', 'Leksand');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (47, NULL, 2451, 'SE46', NULL, 'searl', 'Arlanda');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (48, NULL, 2588, 'SE47', NULL, 'sease', 'Ase');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (49, NULL, 2570, 'SE48', NULL, 'sevil', 'Vilebo');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (50, NULL, 2600, 'SE49', NULL, 'sevar', 'Vara');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (51, NULL, 2606, 'SE50', NULL, 'seang', 'Ängelholm');
INSERT INTO sources (id, country_code, wmo_code, radar_site, originating_centre, node_id, place) VALUES (52, NULL, 2666, 'SE51', NULL, 'sekkr', 'Karlskrona');
