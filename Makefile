clihex:	files.o gui.o
	gcc -lncurses main.c -o clihex files.o gui.o
files:	files.c
	gcc files.c -o files.o -c 
gui:	gui.c
	gcc -lncurses gui.c -o gui.o -c
clean:
	rm *.o clihex
