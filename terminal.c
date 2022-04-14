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
	/* use_default_colors(); */

	if (can_change_color() == FALSE)
		die(L"Can not change colors");

	/* to get ncurses rgb colors do: regular rgb * 1000 / 255 */
	init_color(COLOR_BLACK, 0, 0, 0);
	init_color(COLOR_WHITE, 670, 654, 639);
	init_color(COLOR_CYAN,  0, 400, 1000);

	if (has_colors())
	{
		/*                          foreground   background   */
		init_pair(TEXTWINDOW_COLOR, COLOR_WHITE, COLOR_BLACK);
		init_pair(STATUSBAR_COLOR,  COLOR_BLACK, COLOR_CYAN);
	}
}

void
die (const wchar_t *s)
{
	endwin();

	/* perror(s); */
	fwprintf(stderr, L"%ls\n", s);
	exit(1);
}

/* TODO: keychord parsing */
int
readKeyChord (void)
{
	return 0;
}
