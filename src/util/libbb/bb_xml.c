/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_xml.c,v 1.1 2007-09-04 23:37:20 deweerdt Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2007 Frederik Deweerdt

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : BlackBoard

-----------------------------------------------------------------------

Purpose   : XML dumper for the blackboard

-----------------------------------------------------------------------
 */

#include <bb_core.h>
#include <bb_alias.h>
#include <bb_tools.h>
#include <libxml/xmlreader.h>

static int bb_xml_process_node(xmlTextReaderPtr reader, bbtools_request_t * req)
{
	const xmlChar *name;

	name = xmlTextReaderConstName(reader);
	if (name == NULL) {
		puts("aze1");
		return -1;
	}

	if (!strcmp((char *) name, "bb")) {
		int n_data;
		int max_data_desc_size;
		char *n_data_str;
		char *max_data_desc_size_str;
		n_data_str = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "n_data");
		n_data = atoi(n_data_str);
		free(n_data_str);
		max_data_desc_size_str = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "max_data_desc_size");
		max_data_desc_size = atoi(max_data_desc_size_str);
		free(max_data_desc_size_str);

		printf("n_data %d, max_data_desc_size %d\n", n_data, max_data_desc_size);
		if (bb_attach(&req->theBB, req->bbname) != BB_OK) {
			if (bb_create(&req->theBB, req->bbname, n_data, max_data_desc_size) != BB_OK) {
				return -1;
			}

		}
		return 0;
	} else if (!strcmp((char *) name, "variable")) {
		int alias_target, type, dimension, type_size, data_offset;
		char *alias_target_str, *type_str, *dimension_str, *type_size_str, *data_offset_str;
		char *value, *name;
		S_BB_DATADESC_T dd;
		void *v;
		int i = 0;
		char *str_to_free, *cur_val;
		char *saveptr;

		alias_target_str = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "alias_target");
		alias_target = atoi(alias_target_str);
		type_str = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "type");
		type = atoi(type_str);
		dimension_str = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "dimension");
		dimension = atoi(dimension_str);
		type_size_str = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "type_size");
		type_size = atoi(type_size_str);
		data_offset_str = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "data_offset");
		data_offset = atoi(data_offset_str);
		name = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "name");
		value = (char *) xmlTextReaderGetAttribute(reader, BAD_CAST "value");

		cur_val = strdup(value);
		str_to_free = cur_val;
		bb_set_varname(&dd, name);
		dd.type = type;
		dd.dimension = dimension;
		dd.type_size = type_size;
		dd.data_offset = data_offset;
		dd.alias_target = alias_target;

		if (alias_target != -1) {
			/* this is an alias */
			bb_alias_publish(req->theBB, &dd, &bb_data_desc(req->theBB)[alias_target]);
		} else {
			/*
			   Then, consider the real variables. Tought part,
			   as we need to restore the original values
			 */
			v = bb_publish(req->theBB, &dd);
			while (i++ < dimension) {
				char *val;
				int is_hex = !strncasecmp(value, "0x", 2);
				val = strtok_r(cur_val, ",", &saveptr);
				cur_val = NULL;
				switch (type) {
					case E_BB_CHAR:
					case E_BB_UCHAR:
						memset(v, 0, dimension);
						strcpy((char *) v, value);
						i = dimension;
						break;
					case E_BB_USER:
						{
							int n;
							for (n = 0; n < type_size; n++) {
								*(uint8_t *) v = strtol(val, NULL, is_hex ? 16 : 10);
								v = ((int8_t *) v) + 1;
								val = strtok_r(cur_val, ",", &saveptr);
								cur_val = NULL;
								if (val && !strncmp("0x", val, 2)) {
									val += 2;
									is_hex = 1;
								}
							}
						}
						continue;
					default:
						bb_value_direct_write(v, dd, val, is_hex);
						v = ((int8_t *)v)+type_size;
					}
				}
				free(str_to_free);
			}
		free(name);
		free(value);
		free(alias_target_str);
		free(type_str);
		free(dimension_str);
		free(type_size_str);
		free(data_offset_str);
	} else {
		return 0;
	}

	return 0;
}

int bb_xml_load_file(bbtools_request_t * req)
{
	xmlTextReaderPtr reader;
	int ret;
	char *filename = req->argv[1];

	reader = xmlNewTextReaderFilename(filename);
	if (reader != NULL) {
		ret = xmlTextReaderRead(reader);
		while (ret == 1) {
			if (bb_xml_process_node(reader, req) < 0) {
				puts("failed");
				return -1;
			}
			ret = xmlTextReaderRead(reader);
		}
		xmlFreeTextReader(reader);
		if (ret != 0) {
			printf("%s : failed to parse\n", filename);
		}
	} else {
		printf("Unable to open %s\n", filename);
	}
	return 0;
}
