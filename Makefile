clihex:	files.o gui.o edit.o
	gcc -lncurses main.c -o clihex files.o gui.o edit.o
files:	files.c
	gcc files.c -o files.o -c 
gui:	gui.c
	gcc -lncurses gui.c -o gui.o -c
edit: edit.c
	gcc edit.o -o edit.o -c
clean:
	rm *.o clihex
