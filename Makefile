CC = clang
CFLAGS = -Wall -std=gnu2y -O3 -ffast-math -march=native -lm -fenable-matrix
CFLAGS += -fuse-ld=lld -lSDL3 -lGLESv2
all: main
	./main

main:
