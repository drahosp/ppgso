#
# Makefile for OpenGL demo programs
#
# "make program" to make one program
# "make" or "make all" to make all executables
# "make clean" to remove executables
#

CC		= g++
CFLAGS	= -O2 -std=c++11 -Wextra -Wall
UNAME := $(shell uname -s)

ALL =   raw_gradient gl_gradient gl_texture

all:  $(ALL)

%: %.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS)

# Windows
LFLAGS	= -lm -lglew32 -lglfw3 -lOpenGL32 -lglu32
clean:
	-del *.exe

# Linux
ifeq ($(UNAME),Linux)
LFLAGS = -lm -lGLEW -lglfw -lGL -lGLU
clean:
	-rm $(ALL)
endif

# OSX
ifeq ($(UNAME),Darwin)
LFLAGS = -lm -lGLEW -lglfw3 -framework OpenGL
clean:
	-rm $(ALL)
endif
