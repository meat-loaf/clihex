#define LOCAL_GENERAL_HEADER

#define NCURSES_EXIT(X) 	\
	endwin();				\
	exit(1);

#define ALLOCMEM(V, X)		\
	V = malloc(X);			\
	if (!(V)){				\
		NCURSES_EXIT(1);	\
	}


