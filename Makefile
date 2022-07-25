ifeq ($(OS),Windows_NT)
LDFLAGS=-lfreeglut -lopengl32 -Wl,--subsystem,windows
EXECUTABLE=main.exe
else
LDFLAGS=-lGL -lglut -lGLU
EXECUTABLE=main
endif

CFLAGS=-g -DDEBUG

all:
	g++ main.cpp chip_8.cpp chip_8.h  $(LDFLAGS) -o $(EXECUTABLE) 

clean:
	rm main.o $(EXECUTABLE)

