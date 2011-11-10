UNAME := $(shell uname -s)
INCLUDES = -Itrimesh/include
LIBDIR = -Ltrimesh/lib.$(UNAME)
CC=g++
CFLAGS=-lGL -lglut -lGLU -ltrimesh
ifeq ($(UNAME),Darwin)
	CFLAGS=-framework GLUT -framework OpenGL -ltrimesh
endif

tron: tron.o
	$(CC) -o $@ $(CFLAGS) -g $(LIBDIR) $(INCLUDES) tron.o

tron.o: tron.cpp
	$(CC) -c tron.cpp -g $(CFLAGS) $(LIBDIR) $(INCLUDES)