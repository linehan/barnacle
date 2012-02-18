SDL_CFLAGS := $(shell sdl-config --cflags) 
SDL_LDFLAGS := $(shell sdl-config --libs)

all: gltest.c
	gcc $(SDL_CFLAGS) $(SDL_LDFLAGS) -lGL -lGLU -lglut gltest.c -o gltest

clean:
	rm -f gltest.o gltest
