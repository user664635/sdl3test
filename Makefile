CC = clang
CFLAGS = -Wall -std=gnu2y -O3 -ffast-math -march=native -fenable-matrix
CFLAGS += -fuse-ld=lld -lm -lfftw3f -lSDL3 -lGL -lglad
all: main
	./main

main:
