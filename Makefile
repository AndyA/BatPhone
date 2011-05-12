.PHONY: all clean dist install

PREFIX=/alt/local
INCDIR=$(PREFIX)/include
LIBDIR=$(PREFIX)/lib

LIBS=-lsndfile

INCLUDES=-I/opt/local/include -I$(INCDIR)
LDFLAGS=-L/opt/local/lib $(LIBS) -L$(LIBDIR)
OPTIMIZE=-O3
CPPFLAGS=$(EXTRAFLAGS) -Wall $(OPTIMIZE) -Wno-unused-function $(INCLUDES)

INSTALL_PREFIX=$(PREFIX)

BATTY_OBJS=batty.o pitchshift.o
BATTY=batty

all: $(BATTY)

$(BATTY): $(BATTY_OBJS)

clean:
	rm -rf $(BATTY_OBJS) $(BATTY) *.dSYM
	rm -f *.gcov *.gcda *.gcno

install: $(PROG)
	install -d $(INSTALL_PREFIX)/bin
	install $(BATTY) $(INSTALL_PREFIX)/bin

tags:
	ctags -R *.c inc

test: $(BATTY)
	prove t/*.t
