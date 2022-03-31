#include "marisa.h"

extern editorConfig E;

void
editorRefreshScreen (void)
{
	if (is_term_resized(LINES, COLS) == TRUE)
		/* TODO: Resize handling */
		return;

	editorScroll();
	editorDrawRows();
	editorDrawStatusBar();
	/* TODO: editorDrawMessageBar(); */
	/* TODO: editorDrawFrame() which will draw a different frame for the active window */

	if (BUFFER->flags.dirty == DIRTY)
		wrefresh(FRAME->frame);

	wrefresh(E.bar->statusBarFrame);
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
				wprintw(FRAME->frame, "%lc", L'あ');
			}
		} else {
			int len = BUFFER->row[filerow].rsize - FRAME->coloff;
			if (len < 0)
				len = 0;

			if (len > FRAME->screencols)
				len = FRAME->screencols;

			mvwprintw(FRAME->frame, y, 0, "%ls", &BUFFER->row[filerow].render[FRAME->coloff]);
		}

		wprintw(FRAME->frame,"%lc", L'\n');
	}
}

void
editorScroll (void)
{
	FRAME->rx = 0;
	if (FRAME->cy < BUFFER->numrows)
	{
		FRAME->rx = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
	}

	if (FRAME->cy < FRAME->rowoff)
	{
		FRAME->rowoff = FRAME->cy;
	}
	if (FRAME->cy >= FRAME->rowoff + FRAME->screenrows)
	{
		FRAME->rowoff = FRAME->cy - FRAME->screenrows + 1;
	}
	if (FRAME->rx < FRAME->coloff)
	{
		FRAME->coloff = FRAME->rx;
	}
	if (FRAME->rx >= FRAME->coloff + FRAME->screencols)
	{
		FRAME->coloff = FRAME->rx - FRAME->screencols + 1;
	}

	/* move cursor around the terminal window not the frame */
	move(FRAME->rowoff, FRAME->coloff);
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

void
editorDrawStatusBar (void)
{
	memset(E.bar->statusmsg, 0, E.bar->statusmsgSize);
	swprintf(E.bar->statusmsg, E.bar->statusmsgSize, L"%.20s - %d lines %s %s | %s",
	    (BUFFER->filename == NULL) ?  "[No Name]" : BUFFER->filename, BUFFER->numrows,
	    (BUFFER->flags.dirty == CLEAN) ? "(clean)" : "(modified)",
	    (BUFFER->flags.mode == NORMAL_MODE) ? "(NORMAL)" : "(VISUAL)",
	    BUFFER->buffername);

	werase(E.bar->statusBarFrame);
	waddwstr(E.bar->statusBarFrame, E.bar->statusmsg);
}

void
editorSetStatusMessage (const wchar_t *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vswprintf(E.bar->statusmsg, sizeof(E.bar->statusmsg), fmt, ap);
	va_end(ap);
	E.bar->statusmsg_time = time(NULL);
}


/* TODO: use ncurses to print status bar message */
void
editorDrawMessageBar (void)
{
	int msglen;
	/* abAppend(ab, "\x1b[K", 3); */
	msglen = wcslen(E.bar->statusmsg);
	if (msglen > FRAME->screencols)
		msglen = FRAME->screencols;

	if (msglen && time(NULL) - E.bar->statusmsg_time < 5)
		return;
		/* abAppend(ab, E.bar->statusmsg, msglen); */
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
	keypad(newFrame->frame, TRUE);
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
