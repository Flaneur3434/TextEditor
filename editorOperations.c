#include "marisa.h"

extern editorConfig E;

/* TODO: Change to unicode */
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
		row = &BUFFER->row[FRAME->cy];
		row->size = FRAME->cx;
		row->chars[row->size] = L'\0';
		bufferUpdateRow (row);
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
