CC = clang++
CXX = clang++
COMMON = -Wall -O3 -ffast-math -march=native -fenable-matrix 
CXXFLAGS = $(COMMON) -I/usr/include/opencv4/
CFLAGS = $(COMMON) -xc -std=gnu2y
LDFLAGS = -fuse-ld=lld -lm -lSDL3 -lGLESv2 
LDFLAGS += -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc
all: main
	./main

main: main.o cv.o

clean:
	rm *.o main
