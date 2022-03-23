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


	if (can_change_color())
	{
		init_color(COLOR_BLACK, 0, 0, 0);
		init_color(COLOR_WHITE, 255, 255, 255);
	}

	if (has_colors())
	{
		/*          foreground   background   */
		init_pair(1,COLOR_WHITE, COLOR_BLACK);
		init_pair(2,COLOR_BLACK, COLOR_WHITE);
	}
}

void
die (const char *s)
{
	endwin();

	perror(s);
	exit(1);
}

int
readEscKey (void)
{
	char seq[3];

	seq[0] = getch();
	seq[1] = getch();
	seq[2] = getch();

	if (seq[0] == '[')
	{

		if (seq[1] >= '0' && seq[1] <= '9')
		{
			if (seq[2] == '~')
			{
				switch (seq[1])
				{
				case '1': return HOME_KEY;
				case '3': return DEL_KEY;
				case '4': return END_KEY;
				case '5': return PAGE_UP;
				case '6': return PAGE_DOWN;
				case '7': return HOME_KEY;
				case '8': return END_KEY;
				}
			}
		}
		else if (seq[1] == '[')
		{
			switch (seq[2])
			{
			case 'H': return HOME_KEY;
			case 'F': return END_KEY;
			}
		}
		else
		{
			switch (seq[1])
			{
			case 'A': return ARROW_UP;
			case 'B': return ARROW_DOWN;
			case 'C': return ARROW_RIGHT;
			case 'D': return ARROW_LEFT;
			case 'H': return HOME_KEY;
			case 'F': return END_KEY;
			}

		}
	}
	else if (seq[0] == 'O')
	{
		switch (seq[1])
		{
		case 'H': return HOME_KEY;
		case 'F': return END_KEY;
		}
	}

	return '\x1b';
}

int
editorReadKeyNormal (void)
{
	char c = getch(); /* waits for user input */

	if (c == '\x1b')
	{
		return readEscKey();
	} else {
		return c;
	}
}

int
editorReadKeyVisual (void)
{
	char c = getch(); /* waits for user input */

	switch (c)
	{
	case '\x1b':
		return readEscKey();
	default:
		switch (c)
		{
		case 'i': return ARROW_UP;
		case 'k': return ARROW_DOWN;
		case 'j': return ARROW_LEFT;
		case 'l': return ARROW_RIGHT;
		case 'h': return c;
		case ';': return c;
		case ' ': return c;
		case CTRL_KEY('s'): return c;
		default: return 1000; /* return a unprocessable number when an invalid key is pressed */
		}
	}
}
