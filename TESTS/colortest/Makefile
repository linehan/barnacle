CC=gcc
#
#          gprof        
# optimize   |     warnings
# lvl 3 \    |     /    
CFLAGS=-O3 -pg -Wall            
LDFLAGS=-pg -lm -lmenu -lpanel -lncursesw 
#       /    /     |       \   `````|````
#      / math.h   menu    panels    |                  
#  gprof        (curses) (curses)  ncurses with       
#                                  wide character  
#                                  support

SOURCES=test.c colortest.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=test

all: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE) 

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) 
