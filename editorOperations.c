#include "marisa.h"

extern editorConfig E;

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

/* TODO: broken, highlight skips over whitespace, ect. */
void
editorMarkRegion (void)
{
	/* int begY = FRAME->cy; */
	/* int begX = FRAME->cx; */

	editorSetStatusMessage(L"Marking time");
	/* the first character needs to be marked out side of loop */
	int renderCol = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
	mvwchgat(FRAME->frame, FRAME->cy, renderCol, 1, A_BOLD | A_REVERSE, STATUSBAR_COLOR, NULL);

	wint_t c;
	while ((wget_wch(FRAME->frame, &c) != ERR) && (c != CTRL_KEY('q')))
	{
		editorProcessKeypressMark(c);
		renderCol = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx);
		mvwchgat(FRAME->frame, FRAME->cy, renderCol, 1, A_BOLD | A_REVERSE, STATUSBAR_COLOR, NULL);
		wrefresh(FRAME->frame);
	}

	editorSetStatusMessage(L"Marking time done");
}
