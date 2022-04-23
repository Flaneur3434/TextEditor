#include "marisa.h"

extern editorConfig E;

static void markRegionForward (int, wint_t);

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

/* TODO: moving the marking back, right now it only goes forward */
/* TODO: highlight does not work well with wide characters */
void
editorMarkRegion (void)
{
	int begY = FRAME->cy;
	int begX = FRAME->cx;

	editorSetStatusMessage(L"Marking time");
	editorDrawMessageBar();

	/* the first character needs to be processed out of the loop */
	mvwchgat(FRAME->frame, FRAME->cy, FRAME->cx, 1, A_BOLD | A_REVERSE, STATUSBAR_COLOR, NULL);

	wint_t c;
	while ((wget_wch(FRAME->frame, &c) != ERR) && (c != CTRL_KEY('q')))
	{
		int startX = FRAME->cx;
		editorProcessKeypressMark(c);
		if (FRAME->cx > begX || FRAME->cy > begY)
			markRegionForward(startX, c);
		else
			goto CLEAN_UP;

	wrefresh(FRAME->frame);
	}

 CLEAN_UP:
	editorSetStatusMessage(L"Marking time done");
	FRAME->cy = begY; FRAME->cx = begX;
	editorDrawMessageBar();
}

static void
markRegionForward (int startX, wint_t c)
{
	/* keybinds for nextWord and prevWord */
	if (c == L'u' || c == L'o')
	{
		for (int i = 0; i <= FRAME->cx - startX; i++)
			mvwchgat(FRAME->frame, FRAME->cy, startX + i, 1, A_BOLD | A_REVERSE, STATUSBAR_COLOR, NULL);
	}

	int renderCol = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
	for (int i = 0; i <= renderCol - FRAME->cx; i++)
		mvwchgat(FRAME->frame, FRAME->cy, FRAME->cx + i, 1, A_BOLD | A_REVERSE, STATUSBAR_COLOR, NULL);
}
