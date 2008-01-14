CFLAGS = -O0 -g -Wall -W `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0` -D_FORTIFY_SOURCE=2


#
# The w in -lncursesw is not a typo; it is the wide-character version
# of the ncurses library, needed for multi-byte character languages
# such as Japanese and Chinese etc.
#
# On Debian/Ubuntu distros, this can be found in the
# libncursesw5-dev package. 
#
latencytop: latencytop.c display.c latencytop.h translate.c Makefile
	gcc $(CFLAGS) latencytop.c display.c translate.c -lncursesw -o latencytop 


clean:
	rm -f *~ latencytop DEADJOE
