struct latency_line;

struct latency_line {
	char reason[1024];
	int count;
	double time;
	double max;
};

struct process {
	unsigned int pid;
	int kernelthread;
	char name[64];
	unsigned int max;
	GList *latencies;
	int used;
	int exists;
	/* scheduler stats */
        double maxdelay;
        double totaldelay;
        int delaycount;
};

extern GList *lines;
extern GList *procs;
extern GList *translations;
extern int total_time;
extern int total_count;

extern unsigned int pid_with_max;
extern unsigned int pidmax;
extern int noui;
extern int dump_unknown;


extern int update_display(int duration, char *filter);


#define PT_COLOR_DEFAULT    1
#define PT_COLOR_HEADER_BAR 2
#define PT_COLOR_ERROR      3
#define PT_COLOR_RED        4
#define PT_COLOR_YELLOW     5
#define PT_COLOR_GREEN      6
#define PT_COLOR_BRIGHT     7
extern void initialize_curses(void);
extern char *translate(char *line);
extern void init_translations(char *filename);
extern void cleanup_curses(void);
extern int fsync_display(int duration);
extern int enable_fsync_tracer(void);
extern int disable_fsync_tracer(void);

