#include "marisa.h"

extern editorConfig E;

static void markRegionForward (int);
static void unMarkRegionForward (int);
static void markRegionBackwards (int);
static void unMarkRegionBackwards (int);

void
editorInsertChar (int c)
{
	if (FRAME->cy == BUFFER->numrows)
	{
		bufferInsertRow(BUFFER->numrows, L"", 0);
	}

	bufferRowInsertChar(&BUFFER->row[FRAME->cy], FRAME->cx, c);
	FRAME->cx++;
}

void
editorInsertNewline (void)
{
	if (FRAME->cx == 0)
	{
		bufferInsertRow(FRAME->cy, L"", 0);
	}
	else
	{
		erow *row = &BUFFER->row[FRAME->cy];
		bufferInsertRow(FRAME->cy + 1, &row->chars[FRAME->cx], row->size - FRAME->cx);
		row = &BUFFER->row[FRAME->cy]; /* need to reassign after bufferInsertRow */
		row->size = FRAME->cx;
		row->chars[row->size] = L'\0';
		bufferUpdateRow(row);
	}

	FRAME->cy++;
	FRAME->cx = 0;
}

void
editorDelChar (void)
{
	if (FRAME->cy == BUFFER->numrows) return;
	if (FRAME->cx == 0 && FRAME->cy == 0) return;

	erow *row = &BUFFER->row[FRAME->cy];
	if (FRAME->cx > 0)
	{
		bufferRowDelChar (row, FRAME->cx - 1);
		FRAME->cx--;
	}
	else
	{
		FRAME->cx = BUFFER->row[FRAME->cy - 1].size;
		bufferRowAppendString (&BUFFER->row[FRAME->cy - 1], row->chars, row->size);
		bufferDelRow (FRAME->cy);
		FRAME->cy--;
	}
}

/* TODO: cursor disappears */
/* TODO: highlight does not work well with wide characters */
void
editorMarkRegion (void)
{
	int anchorY = FRAME->cy;
	int anchorX = FRAME->cx;

	editorSetStatusMessage(L"Marking time");
	editorDrawMessageBar();

	/* the first character needs to be processed out of the loop */
	/* this is a bug */
	mvwchgat(FRAME->frame, FRAME->cy, FRAME->cx, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

	wint_t c;
	while ((wget_wch(FRAME->frame, &c) != ERR) && (c != CTRL_KEY('q')))
	{
		int startY = FRAME->cy;
		int startX = FRAME->cx;
		editorProcessKeypressMark(c);
		fprintf(stderr, "%lc was pressed, moving to y:%d, x:%d\n", c, FRAME->cy, FRAME->cx);
		switch (c)
		{
		case L'o':
		case L'l':
		case L';':
			/* TODO: Fix conditions */
			if ((startX >= anchorX && startY > anchorY) || (startX <= anchorX && startY >= anchorY))
				markRegionForward(startX);
			else
				unMarkRegionForward(startX);
			break;
		case L'u':
		case L'j':
		case L'h':
			if ((startX <= anchorX && startY < anchorY) || (startX >= anchorX && startY <= anchorY))
				markRegionBackwards(startX);
			else
				unMarkRegionBackwards(startX);
			break;
		}

		wrefresh(FRAME->frame);
	}

	editorSetStatusMessage(L"Marking time done");
	FRAME->cy = anchorY; FRAME->cx = anchorX;
	editorDrawMessageBar();
}

static void
markRegionForward (int startX)
{
	if (FRAME->cx - startX > 1)
		for (int i = 1; i <= FRAME->cx - startX; i++)
			mvwchgat(FRAME->frame, FRAME->cy, startX + i, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

	int renderCol = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
	for (int i = 0; i <= renderCol - FRAME->cx; i++)
		mvwchgat(FRAME->frame, FRAME->cy, FRAME->cx + i, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);
}

static void
unMarkRegionForward (int startX)
{
	if (FRAME->cx - startX > 1)
		for (int i = 1; i <= FRAME->cx - startX; i++)
			mvwchgat(FRAME->frame, FRAME->cy, startX + i, 1, A_NORMAL, TEXTWINDOW_COLOR, NULL);

	int renderCol = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
	for (int i = 0; i <= renderCol - FRAME->cx; i++)
		mvwchgat(FRAME->frame, FRAME->cy, FRAME->cx + i, 1, A_NORMAL, TEXTWINDOW_COLOR, NULL);
}

static void
markRegionBackwards (int startX)
{
	if (startX - FRAME->cx > 1)
		for (int i = 0; i <= startX - FRAME->cx; i++)
			mvwchgat(FRAME->frame, FRAME->cy, startX - i, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

	int renderCol = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
	for (int i = 0; i <= renderCol - FRAME->cx; i++)
		mvwchgat(FRAME->frame, FRAME->cy, FRAME->cx - i, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);
}

static void
unMarkRegionBackwards (int startX)
{
	if (startX - FRAME->cx > 1)
		for (int i = 0; i <= startX - FRAME->cx; i++)
			mvwchgat(FRAME->frame, FRAME->cy, startX - i, 1, A_NORMAL, TEXTWINDOW_COLOR, NULL);

	int renderCol = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
	for (int i = 0; i <= renderCol - FRAME->cx; i++)
		mvwchgat(FRAME->frame, FRAME->cy, FRAME->cx - i, 1, A_NORMAL, TEXTWINDOW_COLOR, NULL);
}
