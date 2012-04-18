CC=gcc
#
#          gprof        
# optimize   |     warnings
# lvl 3 \    |     /    
CFLAGS=-O3 -pg -Wall            
LDFLAGS=-pg -lm -lncursesw
#       /      \       |
#      /       math    ncurses
#  gprof 
#       
#                                  

SOURCES=test.c unittests.c morton.c matrix.c pathfind.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=test

all: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE) 

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) gmon.out 
