default: clihex

clihex:	files gui edit
	gcc main.c -o clihex files.o gui.o edit.o -I/usr/include/ncurses -lncurses -g
files:	files.c
	gcc files.c -o files.o -c -g
gui:	gui.c
	gcc -lncurses gui.c -o gui.o -c -g
edit: edit.c
	gcc edit.c -o edit.o -c -g
clean:
	rm *.o clihex
