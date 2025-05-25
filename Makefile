CC=gcc
CFLAGS=-Iinclude -Wall
SRC=src/main.c src/lexer.c
OUT=build/nico

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -rf build
