clihex:	files gui
	gcc-7 -lncurses main.c -o clihex files.o gui.o
files:	files.c
	gcc-7 files.c -o files.o -c 
gui:	gui.c
	gcc-7 -lncurses gui.c -o gui.o -c
clean:
	rm *.o
