CC=gcc
#
#          gprof        
# optimize   |     warnings
# lvl 3 \    |     /    
CFLAGS=-O3 -pg -Wall            
LDFLAGS=-pg 
#       /  
#      /  
#  gprof 
#       
#                                  

SOURCES=test.c matrix.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=test

all: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE) 

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) gmon.out 
