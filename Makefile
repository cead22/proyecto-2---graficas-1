UNAME := $(shell uname -s)
INCLUDES = -Itrimesh/include
LIBDIR = -Ltrimesh/lib.$(UNAME)
CC=g++
CFLAGS=-g -lGL -lglut -lGLU -ltrimesh
ifeq ($(UNAME),Darwin)
	CFLAGS=-framework GLUT -framework OpenGL -ltrimesh
endif

tron: tron.o Trimesh.o
	$(CC) -o $@ $(CFLAGS) $(LIBDIR) $(INCLUDES) tron.o

tron.o: tron.cpp Trimesh.o
	$(CC) -c tron.cpp $(CFLAGS) $(LIBDIR) $(INCLUDES) Trimesh.o
	
Trimesh.o: Trimesh.cc
	$(CC) -c Trimesh.cc $(CFLAGS) $(LIBDIR) $(INCLUDES)