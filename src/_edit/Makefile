CC=gcc

#          gprof        
# optimize   |     warnings
# lvl 3 \    |     /    
CFLAGS=-O3 -pg -Wall 
LDFLAGS=-pg -lmenu -lpanel -lform -lncursesw
#        /     |       \       \        |
#       /    curses   curses  curses   ncurses
#    gprof   menu     panels  forms    with wide-character support

SOURCES=edit.c 
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=edit

all: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE) 

clean: 
	rm -f $(OBJECTS) $(EXECUTABLE) gmon.out
