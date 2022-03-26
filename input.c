#include "marisa.h"

extern editorConfig E;

char *
editorPrompt (char *prompt)
{
	size_t bufsize;
	char *userInput;
	size_t userInputLen;

	bufsize = 128;
	if ((userInput = (char *) malloc(bufsize)) == NULL) return NULL;
	userInputLen = 0;

	userInput[0] = '\0';
	while (1)
	{
		editorSetStatusMessage(prompt, userInput);
		editorRefreshScreen();
		BUFFER->flags.mode = NORMAL_MODE;
		int c = wgetch(FRAME->frame);
		if (c == KEY_DC || c == KEY_BACKSPACE)
		{
			if (userInputLen != 0) userInput[--userInputLen] = '\0';
		}
		/* cancle on Esc */
		else if (c == '\x1b' || (c == KEY_ENTER && userInputLen == 0))
		{
			editorSetStatusMessage("");
			free(userInput);
			return NULL;
		}
		else if (c == KEY_ENTER)
		{
			if (userInputLen != 0)
			{
				editorSetStatusMessage("");
				return userInput;
			}
		}
		/* only ascii characters */
		else if (!iscntrl(c) && c < 128)
		{
			if (userInputLen == bufsize - 1)
			{
				bufsize *= 2;
				userInput = (char *) realloc(userInput, bufsize);
			}

			userInput[userInputLen++] = c;
			userInput[userInputLen] = '\0';
		}
	}
}

void
editorProcessKeypressNormal (void)
{
	int times;
	int c = wgetch(FRAME->frame);

	switch (c)
	{
	case KEY_ENTER:
		editorInsertNewline();
		break;
	case CTRL_KEY('q'):
		die("User has exitted the program");
		exit(0);
		break;
	case CTRL_KEY('s'):
		editorSave();
		break;
	case KEY_HOME:
		SWITCH_MODE;
		break;
	case KEY_DC:
		editorMoveCursor(KEY_RIGHT);
		__attribute__ ((fallthrough));
	case KEY_BACKSPACE:
		editorDelChar();
		break;
	case KEY_NPAGE:
	case KEY_PPAGE:
		{
			if (c == KEY_PPAGE)
			{
				FRAME->cy = FRAME->rowoff;
			}
			else if (c == KEY_NPAGE)
			{
				FRAME->cy = FRAME->rowoff + FRAME->screenrows - 1;
				if (FRAME->cy > BUFFER->numrows)
					FRAME->cy = BUFFER->numrows;
			}

			times = FRAME->screenrows;
			while (times--)
				editorMoveCursor(c == KEY_PPAGE ? ARROW_UP : ARROW_DOWN);
		}
		break;

	case KEY_UP:
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:
		editorMoveCursor(c);
		break;
	case CTRL_KEY('l'):
	case '\x1b':
		break;
	default:
		editorInsertChar(c);
		break;
	}
}

void
editorProcessKeypressVisual (void)
{
	int c = wgetch(FRAME->frame);

	switch (c)
	{
	case KEY_ENTER:
		break;
	case CTRL_KEY('f'):
		break;
	case CTRL_KEY('q'):
		die("Didnt't save? HAHAHAHA");
		break;
	case CTRL_KEY('s'):
		editorSave();
		break;
	case KEY_HOME:
		SWITCH_MODE;
		break;
	case KEY_UP:
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:
		editorMoveCursor(c);
		break;
	case 'h':
		FRAME->cx = 0;
		break;
	case ';':
		if (FRAME->cy < BUFFER->numrows)
			FRAME->cx = BUFFER->row[FRAME->cy].size;
		break;
	case ' ':
		readSpaceMap();
		break;
	case ',':
		switchFrame();
		break;
	case '.':
		/* TODO: Call switchBuffer */
		break;
	}
}

void
editorMoveCursor (int key)
{
	int rowlen;
	erow *row = (FRAME->cy >= BUFFER->numrows) ? NULL : &BUFFER->row[FRAME->cy];

	switch (key)
	{
	case KEY_LEFT:
		if (FRAME->cx != 0)
		{
			FRAME->cx--;
		}
		else if (FRAME->cy > 0)
		{
			FRAME->cy--;
			FRAME->cx = BUFFER->row[FRAME->cy].size;
		}
		break;
	case KEY_RIGHT:
		if (row && FRAME->cx < row->size)
		{
			FRAME->cx++;
		}
		else if (row && FRAME->cx == row->size)
		{
			FRAME->cy++;
			FRAME->cx = 0;
		}
		break;
	case KEY_UP:
		if (FRAME->cy != 0)
		{
			FRAME->cy--;
		}
		break;
	case KEY_DOWN:
		if (FRAME->cy < BUFFER->numrows)
		{
			FRAME->cy++;
		}
		break;
	}

	row = (FRAME->cy >= BUFFER->numrows) ? NULL : &BUFFER->row[FRAME->cy];
	rowlen = (row != NULL) ? row->size : 0;
	if (FRAME->cx > rowlen)
	{
		FRAME->cx = rowlen;
	}
}
