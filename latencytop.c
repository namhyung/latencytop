/*
 * Copyright 2008, Intel Corporation
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
#include <sys/types.h>
#include <dirent.h>

#include <glib.h>

#include "latencytop.h"

GList *lines;

GList *procs;
GList *allprocs;
int total_time = 0;
int total_count = 0;

unsigned int pid_with_max = 0;
unsigned int pidmax = 0;
int firsttime = 1;

void parse_global_list(void)
{
	FILE *file;
	char ln[4096];
	file = fopen("/proc/latency_stats","r+");
	if (!file)
		return;
	/* wipe first line */
	fgets(ln, 4096, file);
	total_time = 0;
	total_count = 0;
	while (!feof(file)) {
		struct latency_line *line;
		char *c, *c2;
		memset(ln, 0, 4096);
		fgets(&ln[0], 4095, file);
		if (strlen(ln)<2)
			break;
		line = malloc(sizeof(struct latency_line));
		memset(line, 0, sizeof(struct latency_line));
		line->count = strtoull(ln, &c, 10);
		line->time = strtoull(c, &c, 10);
		line->max = strtoull(c, &c, 10);
		total_time += line->time;
		total_count += line->count;
		c2 = strchr(c, '\n');
		if (c2) *c2=0;
		while (c[0]==' ') c++;
		strcpy(line->reason, c);

		lines = g_list_append(lines, line);
	}
	/* reset for next time */
	fprintf(file, "erase\n");
	fclose(file);
}

gint comparef(gconstpointer A, gconstpointer B)
{
	struct latency_line *a, *b;
	a = (struct latency_line *)A; 
	b = (struct latency_line *)B;
	if (a->max >  b->max)
		return -1;
	if (a->max < b->max)
		return 1;
	if (a->time >  b->time)
		return -1;
	if (a->time < b->time)
		return 1;
	return -1;
}

void sort_list(void)
{
	lines = g_list_sort(lines, comparef);
}


void delete_list(void)
{
	GList *entry, *entry2,*next;
	struct latency_line *line;
	struct process *proc;
	while (lines) {
		entry = g_list_first(lines);
		line = entry->data;
		free(line);
		lines = g_list_delete_link(lines, entry);
	}
	entry = g_list_first(allprocs);
	while (entry) {
		next = g_list_next(entry);
		proc = entry->data;
		while (proc->latencies) {
			entry2 = g_list_first(proc->latencies);
			line = entry2->data;
			free(line);
			proc->latencies = g_list_delete_link(proc->latencies, entry2);
		}
		proc->max = 0;
		if (!proc->exists) {
			free(proc);
			allprocs = g_list_delete_link(allprocs, entry);
		}
		entry = next;
	}
	g_list_free(procs);
	procs = NULL;
}

void prune_unused_procs(void)
{
	GList *entry, *entry2;
	struct latency_line *line;
	struct process *proc;

	entry = g_list_first(procs);
	while (entry) {
		proc = entry->data;
		entry2 = g_list_next(entry);
		if (!proc->used) {
			while (proc->latencies) {
				entry2 = g_list_first(proc->latencies);
				line = entry2->data;
				free(line);
				proc->latencies = g_list_delete_link(proc->latencies, entry2);
			}
			free(proc);
			procs = g_list_delete_link(procs, entry);
		}
		entry = entry2;
	}
}



void parse_process(struct process *process)
{
	DIR *dir;
	struct dirent *dirent;
	char filename[PATH_MAX];

	sprintf(filename, "/proc/%i/task/", process->pid);


	dir = opendir(filename);
	if (!dir)
		return;
	while ((dirent = readdir(dir))) {
		FILE *file;
		char line[4096];
		int pid;
		if (dirent->d_name[0]=='.')
			continue;
		pid = strtoull(dirent->d_name, NULL, 10);
		if (pid<=0) /* not a process */
			continue;


		sprintf(filename, "/proc/%i/task/%i/latency", process->pid, pid);
		file = fopen(filename, "r+");
		if (!file)
			continue;
		/* wipe first line*/
		fgets(line, 4096, file);
		while (!feof(file)) {
			struct latency_line *ln;
			char *c, *c2;
			memset(line, 0, 4096);
			fgets(&line[0], 4095, file);
			if (strlen(line)<2)
				break;
			ln = malloc(sizeof(struct latency_line));
			memset(ln, 0, sizeof(struct latency_line));
			ln->count = strtoull(line, &c, 10);
			ln->time = strtoull(c, &c, 10);
			ln->max = strtoull(c, &c, 10);
			c2 = strchr(c, '\n');
			if (c2) *c2=0;
			strcpy(ln->reason, c);

			if (ln->max > process->max)
				process->max = ln->max;
			process->latencies = g_list_append(process->latencies, ln);
			process->used = 1;
		}
		/* reset for next time */
		fprintf(file, "erase\n");
		fclose(file);
	}
	/* 100 usec minimum */
	if (process->maxdelay > 0.1 && !firsttime) {
		struct latency_line *ln;
			
		ln = malloc(sizeof(struct latency_line));
		memset(ln, 0, sizeof(struct latency_line));
		ln->count = 1;

		ln->time = process->maxdelay * 1000;    
		ln->max = ln->time;
		strcpy(ln->reason, "Scheduler: waiting for cpu");
		if (ln->max > process->max)
			process->max = ln->max;
		process->latencies = g_list_append(process->latencies, ln);
		process->used = 1;
	}
	closedir(dir);
}

struct process* find_create_process(unsigned int pid)
{
	GList *entry;
	struct process *proc;

	entry = g_list_first(allprocs);
	while (entry) {
		proc = entry->data;
		if (proc->pid == pid) {
			return proc;
		}
		entry = g_list_next(entry);
	}
	proc = malloc(sizeof(struct process));
	memset(proc, 0, sizeof(struct process));
	proc->pid = pid;
	allprocs = g_list_append(allprocs, proc);

	return proc;
}

void parse_processes(void)
{
	DIR *dir;
	struct dirent *dirent;
	char filename[PATH_MAX];
	struct process *process;

	pidmax = 0;

	dir = opendir("/proc");
	if (!dir)
		return;
	while ((dirent = readdir(dir))) {
		FILE *file;
		char line[1024];
		int pid;
		if (dirent->d_name[0]=='.')
			continue;
		pid = strtoull(dirent->d_name, NULL, 10);
		if (pid<=0) /* not a process */
			continue;

		process = find_create_process(pid);
		process->exists = 1;

		sprintf(filename, "/proc/%i/status", pid);
		file = fopen(filename, "r");
		if (file) {
			char *q;
			fgets(line, 1023, file);
			strncpy(&process->name[0], &line[6], 64);
			q = strchr(process->name, '\n');
			if (q) *q=0;
			fclose(file);
		}

		sprintf(filename, "/proc/%i/sched", pid);
		file = fopen(filename, "r+");
		if (file) {
			char *q;
			double d;
			memset(line, 0, 1024);
			while (!feof(file)) {
				fgets(line, 1023, file);
				q = strchr(line, ':');
				if (strstr(line, "se.wait_max") && q) {
					sscanf(q+1,"%lf", &d);
					process->maxdelay = d;
				}
			}
			fprintf(file,"erase");
			fclose(file);
		}

		sprintf(filename, "/proc/%i/statm", pid);
		file = fopen(filename, "r");
		if (file) {
			fgets(line, 1023, file);
			if (strcmp(line, "0 0 0 0 0 0 0\n")==0)
				process->kernelthread = 1;

			fclose(file);
		}

		parse_process(process);

		if (process->latencies) {
			process->latencies = g_list_sort(process->latencies, comparef);
			if (!process->kernelthread && pidmax < process->max) {	
				pidmax = process->max;
				pid_with_max = process->pid;
			}
			procs = g_list_append(procs, process);
		};
	}
	closedir(dir);
}


void dump_global_to_console(void)
{
	GList *item;
	struct latency_line *line;
	int i = 0;

	item = g_list_first(lines);
	while (item) {
		line = item->data;
		item = g_list_next(item);
		printf( "[max %4.1fmsec] %40s - %5.2f msec (%3.1f%%)\n",
			line->max * 0.001,
			line->reason,
			(line->time * 0.001 +0.0001) / line->count,
			line->time * 100.0 /  total_time );
		i++;
	}

}


int main(int argc, char **argv)
{
	init_translations("latencytop.trans");
	if (argc>1 && strcmp(argv[1],"-d")==0) {
		parse_global_list();
		sort_list();
		dump_global_to_console();
		return EXIT_SUCCESS;
	}
	initialize_curses();
	while (1 + argc || argv==NULL) {
		parse_processes();
		prune_unused_procs();
		parse_global_list();
		sort_list();
		if (!total_time)
			total_time = 1;
		update_display(30);
		delete_list();
		firsttime = 0;
	}
	return EXIT_SUCCESS;
}
