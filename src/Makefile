ifeq ($(shell pkg-config --exists gtk+-2.0 && echo yes),yes)
  HAS_GTK_GUI = 1
endif

prefix= /usr
DESTDIR =
SBINDIR = $(prefix)/sbin
XCFLAGS = -W  -g `pkg-config --cflags glib-2.0` -D_FORTIFY_SOURCE=2 -O2 -Wno-sign-compare
XCFLAGS += -DPREFIX_DIR=\"$(prefix)\"
LDF = -Wl,--as-needed `pkg-config --libs glib-2.0`   -lncursesw 

OBJS= latencytop.o text_display.o translate.o fsync.o

ifdef HAS_GTK_GUI
  XCFLAGS += `pkg-config --cflags gtk+-2.0` -DHAS_GTK_GUI
  LDF += `pkg-config --libs gtk+-2.0`
  OBJS += gtk_display.o 
endif


#
# The w in -lncursesw is not a typo; it is the wide-character version
# of the ncurses library, needed for multi-byte character languages
# such as Japanese and Chinese etc.
#
# On Debian/Ubuntu distros, this can be found in the
# libncursesw5-dev package. 
#

# We write explicity this "implicit rule"
%.o : %.c
	gcc -c $(CFLAGS) $(XCFLAGS) $< -o $@

latencytop:  $(OBJS) latencytop.h Makefile
	gcc $(CFLAGS) $(OBJS) $(LDF) -o latencytop 

clean:
	rm -f *~ latencytop DEADJOE *.o

install: latencytop
	mkdir -p $(DESTDIR)$(prefix)/share/latencytop
	mkdir -p $(DESTDIR)$(SBINDIR)
	install -m 0644 latencytop.trans $(DESTDIR)$(prefix)/share/latencytop/latencytop.trans
	install -m 0644 *.png $(DESTDIR)$(prefix)/share/latencytop/
	install -m 0755 latencytop $(DESTDIR)$(SBINDIR)/
