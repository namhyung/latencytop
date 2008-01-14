/*
 * Copyright 2007, Intel Corporation
 *
 * This file is part of LatencyTOP
 *
 * This program file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file named COPYING; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * Authors:
 * 	Arjan van de Ven <arjan@linux.intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <glib.h>

#include "latencytop.h"


/* translate kernel output to human readable */
struct translate_line {
	char original[1024];
	char display[1024];	
};


GList *translations;

char *translate(char *line)
{
	char buffer[4096], *c, *c2;
	GList *item;
	struct translate_line *trans;

	memset(buffer, 0, 4096);	
	strcpy(buffer, line);
	c = strchr(buffer, '(');
	if (c)
		c = strchr(c, '+');
	if (c)
		*c=0;
		
	c2 = buffer;
	while (c2[0] == ' ') c2++;
	item = g_list_first(translations);
	while (item) {
		trans = item->data;
		item = g_list_next(item);
		if (strcmp(trans->original, c2)==0)
			return trans->display;
	}
	
	c = strstr(line, "()");
	if (c) { *c = ' '; *(c+1)= ' '; }
	
	return line;
}

void init_translations(char *filename)
{
	char *c1, *c2;
	FILE *file;
	
	file = fopen(filename, "r");
	if (!file)
		return;
	while (!feof(file)) {
		struct translate_line *trans;
		char line[4096];
		memset(line, 0, 4096);
		fgets(line, 4095, file);
		if (line[0]=='#')
			continue;
		c1 = strchr(line,'|');
		if (!c1)
			continue;
		*c1=0;
		c1++;
		while (*c1==' ' || *c1=='\t') c1++;
		c2=strchr(c1, '\n');
		if (c2) *c2=0;
		trans = malloc(sizeof(struct translate_line));
		strcpy(trans->original, line);
		strcpy(trans->display, c1);
		
		translations = g_list_append(translations, trans);
	}
}
