# main needs library, watcher needs library, compile library first (library.h, library.c)
all: library main watcher
library:
	gcc -g -Wall -c library.c -o library.o

main: library.o main.c
	gcc -g -Wall main.c library.o -o main
watcher: library.o watcher.c
	gcc -g -Wall watcher.c library.o -o watcher
