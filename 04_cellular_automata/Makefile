CC = gcc
CFLAGS = -O2 -Wall -std=c99 -I"C:/raylib/include"
LDFLAGS = -L"C:/raylib/lib" -lraylib -lopengl32 -lgdi32 -lwinmm

build:
	$(CC) src/main.c $(CFLAGS) $(LDFLAGS) -o game.exe

clean:
	del game.exe


