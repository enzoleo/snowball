SRCS = main.cpp
OBJS = $(SRCS:.cpp = .o)
PROG = demo
CC = g++
CFFLAGS = -g
LIBS = -lGL -lGLU -lSDL2 -lSDL2_image -lassimp -lGLEW -lglfw -std=c++11

$(PROG): $(OBJS)
	$(CC) $(CFFLAGS) -o $@ $^ $(LIBS)

