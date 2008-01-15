CFLAGS = -O0 -g -Wall -W `pkg-config --cflags glib-2.0`  -D_FORTIFY_SOURCE=2 -Wno-sign-compare
LDF = -Wl,--as-needed `pkg-config --libs glib-2.0`   -lncursesw 

#
# The w in -lncursesw is not a typo; it is the wide-character version
# of the ncurses library, needed for multi-byte character languages
# such as Japanese and Chinese etc.
#
# On Debian/Ubuntu distros, this can be found in the
# libncursesw5-dev package. 
#
latencytop: latencytop.o display.o latencytop.h translate.o Makefile
	gcc $(CFLAGS) latencytop.o display.o translate.o $(LDF)-o latencytop 


clean:
	rm -f *~ latencytop DEADJOE *.o
