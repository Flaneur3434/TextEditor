#include "marisa.h"

extern editorConfig E;

void
editorRefreshScreen (void)
{
	if (is_term_resized(LINES, COLS) == TRUE)
		/* TODO: Resize handling */
		return;

	editorScroll(); /* can be optimized by moving function call into a magically conditional */
	editorDrawRows();
	editorDrawStatusBar();
	editorDrawMessageBar();
	/* TODO: editorDrawFrame() which will draw a different frame for the active window */

	wmove(FRAME->frame, FRAME->cy - FRAME->rowoff, FRAME->rx - FRAME->coloff);
	wrefresh(FRAME->frame);
}

void
editorScroll (void)
{
	FRAME->rx = 0; /* TODO: understand what this does.
	FIXES: When scrolling to the end of a line, the cursor moves to the next row even if no line is there */

	if (FRAME->cy < BUFFER->numrows)
	{
		FRAME->rx = bufferRowCxToRx(&BUFFER->row[FRAME->cy], 0, FRAME->cx);
	}

	FRAME->rowoff = FRAME->cy - FRAME->screenrows < 0 ? 0 : FRAME->cy - FRAME->screenrows + 2;
	FRAME->coloff = FRAME->rx - FRAME->screencols < 0 ? 0 : FRAME->rx - FRAME->screencols + 1;
}

void
editorDrawRows (void)
{
	for (int y = 0; y < FRAME->screenrows; y++)
	{
		int filerow = y + FRAME->rowoff;
		if (filerow >= BUFFER->numrows)
		{
			if (BUFFER->numrows == 0 && y == FRAME->screenrows / 3 && BUFFER->filename == NULL)
			{
				drawWelcome();
			} else {
				mvwprintw(FRAME->frame, y, 0, "%lc", L'~');
			}
		} else {
			if (FRAME->coloff == 0)
			{
				mvwprintw(FRAME->frame, y, 0, "%ls", BUFFER->row[filerow].render);
			}
			else if (FRAME->coloff <= BUFFER->row[filerow].rsize)
			{
				mvwprintw(FRAME->frame, y, 0, "%ls", &BUFFER->row[filerow].render[FRAME->coloff]);
			}
			else
			{
				mvwprintw(FRAME->frame, y, 0, "%c", '\n');
			}
		}
		wprintw(FRAME->frame, "%c", '\n');
	}
}

void
drawWelcome (void)
{
	char welcome[80];
	int welcomelen = snprintf(welcome, sizeof(welcome),
	    "MARISA editor -- version %s", MARISA_VERSION);
	if (welcomelen > FRAME->screencols)
		welcomelen = FRAME->screencols;

	int padding = (FRAME->screencols - welcomelen) / 2;
	if (padding)
	{
		wprintw(FRAME->frame,"%lc", L'~');
		padding--;
	}

	while (padding--)
		wprintw(FRAME->frame,"%lc", L' ');

	wprintw(FRAME->frame, "%s", welcome);
}

/* TODO: status bar is not complete, some how need to show all of the modes */
void
editorDrawStatusBar (void)
{
	memset(E.bar->statusmsg, 0, E.bar->statusmsgSize);
	swprintf(E.bar->statusmsg, E.bar->statusmsgSize, L"%.20s - %d lines offset: (%d, %d) (%d, %d) %s %s | %s",
	    (BUFFER->filename == NULL) ?  "[No Name]" : BUFFER->filename,
	    BUFFER->numrows,
	    FRAME->rowoff,
	    FRAME->coloff,
	    FRAME->cy + 1,
	    FRAME->cx + 1,
	    (BUFFER->flags.dirty == CLEAN) ? "(clean)" : "(modified)",
	    (BUFFER->flags.mode == NORMAL_MODE) ? "(NORMAL)" : "(VISUAL)",
	    BUFFER->buffername);

	werase(E.bar->statusBarFrame);
	waddwstr(E.bar->statusBarFrame, E.bar->statusmsg);
	wrefresh(E.bar->statusBarFrame);
}

void
editorSetStatusMessage (const wchar_t *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vswprintf(E.mes->message, sizeof(E.mes->message), fmt, ap);
	va_end(ap);
	E.mes->messageTime = time(NULL);
}

void
editorDrawMessageBar (void)
{
	int msglen;
	msglen = wcslen(E.mes->message);
	if (msglen > FRAME->screencols)
		msglen = FRAME->screencols;

	if (msglen && time(NULL) - E.mes->messageTime < 5)
	{
		E.mes->messageLen = msglen;
		werase(E.mes->messageBarFrame);
		waddwstr(E.mes->messageBarFrame, E.mes->message);
		wrefresh(E.mes->messageBarFrame);
	}
}

void
newFrame (WINDOW *ncursesWindow, textEditorBuffer *buffer)
{
	if (E.frames == NULL)
		E.frames = g_ptr_array_sized_new(10);

	frame *newFrame = g_new(frame, 1);
	newFrame->frame = ncursesWindow;
	newFrame->currentBuffer = buffer;
	newFrame->cx = 0;
	newFrame->cy = 0;
	newFrame->rx = 0;
	newFrame->rowoff = 0;
	newFrame->coloff = 0;
	getmaxyx(ncursesWindow, newFrame->screenrows, newFrame->screencols);
	/* padding is needed because ncurses bug that moves cursor to wrong position when it hits the end of screen */
	newFrame->screencols -= 1;
	keypad(newFrame->frame, TRUE);
	clearok(newFrame->frame, TRUE);
	g_ptr_array_add(E.frames, newFrame);
}

static void
changeCurrentBuffer (gpointer buffer, gpointer targetName)
{
	if (strcmp(((textEditorBuffer *) buffer)->buffername, (char *) targetName) == 0)
	{
		FRAME->currentBuffer = (textEditorBuffer *) buffer;
	}
}

void
switchBuffer (char *bufferName)
{
	g_ptr_array_foreach(E.buffers, changeCurrentBuffer, bufferName);
}

void
switchFrame (void)
{
	guint frameIndex;
	if (g_ptr_array_find(E.frames, (gconstpointer) FRAME, &frameIndex) == FALSE)
		die(L"couldnt switch frame ;-;");

	if (++frameIndex == E.frames->len)
		FRAME = (frame *) g_ptr_array_index(E.frames, 0);
	else
		FRAME = (frame *) g_ptr_array_index(E.frames, frameIndex);
}

void
killFrame (void)
{
	wborder(FRAME->frame, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(FRAME->frame);
	delwin(FRAME->frame);
	if (E.frames->len == 1)
	{
		if (BUFFER->flags.dirty == DIRTY)
			/* TODO: ask if buffer needs to be saved */
			return;

		g_ptr_array_remove(E.frames, FRAME);
		die(L"Program exited because last frame was killed");
	}

	guint frameIndex;
	if (g_ptr_array_find(E.frames, (gconstpointer) FRAME, &frameIndex) == FALSE)
		die(L"couldnt kill frame ;-;");

	if (frameIndex == E.frames->len - 1)
		frameIndex--;

	g_ptr_array_remove(E.frames, FRAME);
	FRAME = (frame *) g_ptr_array_index(E.frames, frameIndex);
}


WINDOW *createNewWin (int height, int width, int starty, int startx, unsigned int colorPairNum)
{	WINDOW *localWin;

	localWin = newwin(height, width, starty, startx);
	keypad(localWin, TRUE);
	box(localWin, 0 , 0);
	wbkgd(localWin, COLOR_PAIR(colorPairNum));
	wrefresh(localWin);

	return localWin;
}
