#include "marisa.h"

extern editorConfig E;

void
setupTerm (void)
{
	initscr();                /* Start curses mode */
	raw();                    /* Line buffering disabled, feeds program raw key codes */
	noecho();                 /* Don't echo while going getch() */
	keypad(stdscr, TRUE);     /* We get F1, F2, etc. */
	start_color();
	use_default_colors();

	if (has_colors())
	{
		/*           foreground   background   */
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_BLACK, COLOR_WHITE);
	}
}

void
die (const char *s)
{
	endwin();

	perror(s);
	exit(1);
}

/* TODO: keychord parsing */
int
readKeyChord (void)
{
	return 0;
}
