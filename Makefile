CC = clang++
CXX = clang++
COMMON = -Wall -O3 -ffast-math -march=native -fenable-matrix 
CXXFLAGS = $(COMMON) -I/usr/include/opencv4/ -std=gnu++2c
CFLAGS = $(COMMON) -xc -std=gnu2y
LDFLAGS = -fuse-ld=lld -lm -lSDL3 -lGLESv2 
LDFLAGS += -lopencv_core -lopencv_videoio 
all: main
	./main

main: main.o cv.o

clean:
	rm *.o main
