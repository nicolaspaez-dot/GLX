CC=gcc
CFLAGS=-Iinclude -Wall
SRC=src/main.c src/lexer.c src/parser.c src/interpreter.c src/utils.c
OUT=build/gx

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -rf build
