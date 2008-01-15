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
#include <sys/time.h>
#include <dirent.h>
#include <ncurses.h>
#include <time.h>
#include <wchar.h>
#include <ctype.h>

#include <glib.h>

#include "latencytop.h"

static WINDOW *title_bar_window;
static WINDOW *global_window;
static WINDOW *process_window;
static WINDOW *right_window;

static GList *cursor_e = NULL;

static void cleanup_curses(void) {
	endwin();
}


static void zap_windows(void)
{
	if (title_bar_window) {
		delwin(title_bar_window);
		title_bar_window = NULL;
	}
	if (global_window) {
		delwin(global_window);
		global_window = NULL;
	}
	if (process_window) {
		delwin(process_window);
		process_window = NULL;
	}
	if (right_window) {
		delwin(right_window);
		right_window = NULL;
	}
}


int maxx, maxy;

void setup_windows(void) 
{
	getmaxyx(stdscr, maxy, maxx);

	zap_windows();	

	title_bar_window = subwin(stdscr, 1, maxx, 0, 0);
	global_window = subwin(stdscr, 10, maxx, 2, 0);
	process_window = subwin(stdscr, 1, maxx, 24, 0);
	right_window = subwin(stdscr, 10, maxx, 13, 0);

	werase(stdscr);
	refresh();
}

void initialize_curses(void) 
{
	initscr();
	start_color();
	keypad(stdscr, TRUE);	/* enable keyboard mapping */
	nonl();			/* tell curses not to do NL->CR/NL on output */
	cbreak();		/* take input chars one at a time, no wait for \n */
	noecho();		/* dont echo input */
	curs_set(0);		/* turn off cursor */
	use_default_colors();

	init_pair(PT_COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
	init_pair(PT_COLOR_HEADER_BAR, COLOR_BLACK, COLOR_WHITE);
	init_pair(PT_COLOR_ERROR, COLOR_BLACK, COLOR_RED);
	init_pair(PT_COLOR_RED, COLOR_WHITE, COLOR_RED);
	init_pair(PT_COLOR_YELLOW, COLOR_WHITE, COLOR_YELLOW);
	init_pair(PT_COLOR_GREEN, COLOR_WHITE, COLOR_GREEN);
	init_pair(PT_COLOR_BRIGHT, COLOR_WHITE, COLOR_BLACK);
	
	atexit(cleanup_curses);
}

void show_title_bar(void) 
{
	wattrset(title_bar_window, COLOR_PAIR(PT_COLOR_HEADER_BAR));
	wbkgd(title_bar_window, COLOR_PAIR(PT_COLOR_HEADER_BAR));   
	werase(title_bar_window);

	mvwprintw(title_bar_window, 0, 0,  "   LatencyTOP version 0.1       (C) 2008 Intel Corporation");

	wrefresh(title_bar_window);
}



void print_global_list(void)
{
	GList *item;
	struct latency_line *line;
	int i = 1;

	mvwprintw(global_window, 0, 0, "Cause");
	mvwprintw(global_window, 0, 50, "  Maximum          Average\n");
	item = g_list_first(lines);
	while (item && i < 10) {
		line = item->data;
		item = g_list_next(item);
		mvwprintw(global_window, i, 0, "%s", translate(line->reason));
		mvwprintw(global_window, i, 50, "%5.1f msec        %5.1f msec\n",
				line->max * 0.001,
				(line->time * 0.001 +0.0001) / line->count);
		i++;
	}
	wrefresh(global_window);

}

void display_process_list(unsigned int cursor_pid)
{
	GList *entry, *start = NULL;
	struct process *proc;
	int i = 0, xpos = 0;
	werase(process_window);

	entry = procs;
	
retry:
	xpos = 0;
	start = cursor_e;	
	if (!start) {
		start = g_list_first(procs);
		cursor_e = start;
	}
		
		
	proc = start->data;
	while (proc->pid > cursor_pid && cursor_pid > 0) {
		start = g_list_previous(start);
		proc = start->data;
		cursor_e = start;
	}

	/* and print 7 */
	i = 0;
	while (start) {
		proc = start->data;	

		if (proc->pid == cursor_pid) {
			if (xpos + strlen(proc->name) + 2 > maxx && cursor_e) {
				cursor_e = g_list_next(cursor_e);
				goto retry;
			}
			wattron(process_window, A_REVERSE);
		}
		
		if (xpos + strlen(proc->name) + 2< maxx) 
			mvwprintw(process_window, 0, xpos, " %s ", proc->name);
		xpos += strlen(proc->name)+2;
		
		wattroff(process_window, A_REVERSE);

		start = g_list_next(start);

		i++;
	}	
	wrefresh(process_window);
}

int one_pid_back(unsigned int cursor_pid)
{
	GList *entry, *start = NULL;
	struct process *proc;

	entry = procs;
	while (entry) {
		proc = entry->data;
		if (proc->pid == cursor_pid) {
			start = entry;
			break;
		}	
		entry = g_list_next(entry);
	}
	if (g_list_previous(start))
			start = g_list_previous(start);
	if (start) {
		proc=start->data;
		return proc->pid;
	}
	return 0;
}

int one_pid_forward(unsigned int cursor_pid)
{
	GList *entry, *start = NULL;
	struct process *proc;

	entry = procs;
	while (entry) {
		proc = entry->data;
		if (proc->pid == cursor_pid) {
			start = entry;
			break;
		}	
		entry = g_list_next(entry);
	}
	if (g_list_next(start))
			start = g_list_next(start);
	if (start) {
		proc=start->data;
		return proc->pid;
	}
	return 0;
}

void print_process(unsigned int pid)
{
	struct process *proc;
	GList *item;
	werase(right_window);

	item = g_list_first(procs);
	while (item) {
		char header[4096];
		int i = 0;
		GList *item2;
		struct latency_line *line;
		proc = item->data;
		item = g_list_next(item);
		if (proc->pid != pid)
			continue;
		wattron(right_window, A_REVERSE);
		sprintf(header, "Process %s (%i) ", proc->name, proc->pid);
		while ((int)strlen(header) < maxx)
			strcat(header, " ");
		mvwprintw(right_window, 0, 0, "%s", header);
		wattroff(right_window, A_REVERSE);
		item2 = g_list_first(proc->latencies);
		while (item2 && i < 6) {
			line = item2->data;
			item2 = g_list_next(item2);
			mvwprintw(right_window, i+1, 0, "%s", translate(line->reason));
			mvwprintw(right_window, i+1, 50, "%5.1f msec        %5.1f msec",
				line->max * 0.001,
				(line->time * 0.001 +0.0001) / line->count
				);
			i++;
		}

	}
	wrefresh(right_window);
}

int done_yet(int time, struct timeval *p1)
{
	int seconds;
	int usecs;
	struct timeval p2;
	gettimeofday(&p2, NULL);
	seconds = p2.tv_sec - p1->tv_sec;
	usecs = p2.tv_usec - p1->tv_usec;

	usecs += seconds * 1000000;
	if (usecs > time * 1000000)
		return 1;
	return 0;
}



void update_display(int duration)
{
	struct timeval start,end,now;
	int key;
	int repaint = 1;
	fd_set rfds;
	gettimeofday(&start, NULL);
	setup_windows();
	show_title_bar();
	print_global_list();
	while (!done_yet(duration, &start)) {
		if (repaint) {
			display_process_list(pid_with_max);
			print_process(pid_with_max);
		}
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		gettimeofday(&now, NULL);
		end.tv_sec = start.tv_sec + duration - now.tv_sec;
		end.tv_usec = start.tv_usec - now.tv_usec;
		while (end.tv_usec < 0) {
			end.tv_sec --;
			end.tv_usec += 1000000;
		};
		key = select(1, &rfds, NULL, NULL, &end);
		repaint = 1;

		if (key) {
			char keychar;
			keychar = fgetc(stdin);		
			if (keychar == 27) {
				keychar = fgetc(stdin);	
				if (keychar==79)
					keychar = fgetc(stdin);	
			}
			keychar = toupper(keychar);
			if (keychar == 'Z' || keychar == 'A' || keychar == 'D')
				pid_with_max = one_pid_back(pid_with_max);
			if (keychar == 'X' || keychar == 'B' || keychar == 'C') 
				pid_with_max = one_pid_forward(pid_with_max);
			if (keychar == 'Q') 
				exit(EXIT_SUCCESS);
			if (keychar == 'R') {
				cursor_e = NULL;
				return;
			}
			if (keychar < 32)
				repaint = 0;
		}
	}
	cursor_e = NULL;
}
