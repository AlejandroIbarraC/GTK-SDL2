CC 			?= gcc
PKGCONFIG	= $(shell which pkg-config)
CFLAGS		= $(shell $(PKGCONFIG) --cflags --libs gtk+-3.0) -lpthread `sdl2-config --cflags --libs` -lSDL2_image
RM			= rm -f

all: build

build: main.c
	$(CC) main.c -o main $(CFLAGS)

clean:
	$(RM) main