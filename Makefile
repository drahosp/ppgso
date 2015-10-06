#
# Makefile for ppgso examples
#
# "make program" to make one program
# "make" or "make all" to make all executables
# "make clean" to remove executables
#

CC = g++
CFLAGS = -O2 -std=c++11 -Wextra -Wall
LFLAGS = -lm
UNAME := $(shell uname -s)

ALL = raw_gradient gl_gradient gl_texture gl_animate gl_projection

all: $(ALL)

gl_%: gl_%.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS) $(GL_LFLAGS)

raw_%: raw_%.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS)

# Linux
ifeq ($(UNAME),Linux)
GL_LFLAGS = -lGLEW -lglfw -lGL -lGLU
clean:
	-rm $(ALL)

# OSX 
else ifeq ($(UNAME),Darwin)
GL_LFLAGS = -lGLEW -lglfw3 -framework OpenGL
clean:
	-rm $(ALL)

# Windows
else
GL_LFLAGS = -lglew32 -lglfw3 -lOpenGL32 -lglu32
clean:
	-del *.exe

endif
