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
		int c = editorReadKeyNormal();
		if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE)
		{
			if (userInputLen != 0) userInput[--userInputLen] = '\0';
		}
		/* cancle on Esc */
		else if (c == '\x1b' || (c == '\r' && userInputLen == 0))
		{
			editorSetStatusMessage("");
			free(userInput);
			return NULL;
		}
		else if (c == '\r')
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
	int c = editorReadKeyNormal ();

	switch (c)
	{
	case '\r':
		editorInsertNewline();
		break;
	case CTRL_KEY('q'):
		die("User has exitted the program");
		exit(0);
		break;
	case CTRL_KEY('s'):
		editorSave();
		break;
	case HOME_KEY:
		switchModes();
		break;
	case BACKSPACE:
	case CTRL_KEY('h'):
	case DEL_KEY:
		if (c == DEL_KEY)
			editorMoveCursor(ARROW_RIGHT);

		editorDelChar();
		break;
	case PAGE_UP:
	case PAGE_DOWN:
		{
			if (c == PAGE_UP)
			{
				FRAME->cy = FRAME->rowoff;
			}
			else if (c == PAGE_DOWN)
			{
				FRAME->cy = FRAME->rowoff + FRAME->screenrows - 1;
				if (FRAME->cy > BUFFER->numrows)
					FRAME->cy = BUFFER->numrows;
			}

			times = FRAME->screenrows;
			while (times--)
				editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
		}
		break;

	case ARROW_UP:
	case ARROW_DOWN:
	case ARROW_LEFT:
	case ARROW_RIGHT:
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
	int c = editorReadKeyVisual();

	switch (c)
	{
	case '\r':
		break;
	case CTRL_KEY('f'):
		break;
	case CTRL_KEY('q'):
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
		break;
	case CTRL_KEY('s'):
		editorSave();
		break;
	case HOME_KEY:
		switchModes();
		break;
	case ARROW_UP:
	case ARROW_DOWN:
	case ARROW_LEFT:
	case ARROW_RIGHT:
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
	}
}

void
editorMoveCursor (int key)
{
	int rowlen;
	erow *row = (FRAME->cy >= BUFFER->numrows) ? NULL : &BUFFER->row[FRAME->cy];

	switch (key)
	{
	case ARROW_LEFT:
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
	case ARROW_RIGHT:
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
	case ARROW_UP:
		if (FRAME->cy != 0)
		{
			FRAME->cy--;
		}
		break;
	case ARROW_DOWN:
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
