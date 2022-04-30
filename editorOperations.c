#include "marisa.h"

extern editorConfig E;

/* static void markRegionForward (int); */
/* static void unMarkRegionForward (int); */
/* static void markRegionBackwards (int); */
/* static void unMarkRegionBackwards (int); */

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

/* TODO: highlight does not work well with wide characters */
/* TODO: Breaks when selection starts in the middle of a word */

/*
 * Every motion during mark-mode refreshes the screen with a section of the text highlighted
 * Every motion produces a coordinate to anywhere on the screen
 * We find the x-offset and y-offset of each action
 * We need to keep track of the previous coordinates (in case we need to unmark)
 * Once mark-mode is activated, we need to keep track of the anchors
 *
 *
 * We need to do 2 passes on the screen
 * 1. Refresh screen line by line from the top (0, 0)
 * 2. Back from the top, top to the coordinate of the last mark command
 *    Start moving character by character and mark the region until you get to the mark end coordinate
 *
 * No need for an unmark function because the screen gets refreshed every command
 *
 *
 * Variables to keep track of:
 * anchorX/Y: to know where mark-mode started. will ALWAYS be the beginning or end of a selection block
 * prevX/Y: to know where the previous mark-mode move ended
 * currX/Y: to know where the current mark-mode move will end
 * Screen min-point / max-point: to know the minimum and maximum coordinates the user can mark without scrolling
 */


void
editorMarkRegion (void)
{
	int anchorX = FRAME->cx, anchorRX = FRAME->rx, anchorY = FRAME->cy;

	editorSetStatusMessage(L"Marking time");
        editorDrawMessageBar();
	cursor_set_color_rgb(MARK_CURSOR);
	/* move back to the text window */
        wmove(FRAME->frame, anchorY, anchorRX);

#define MARK_CHAR(y, x) mvwchgat(FRAME->frame, y, x, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL)
        wint_t c;
        while ((wget_wch(FRAME->frame, &c) != ERR) && (c != CTRL_KEY('q')))
	{
		editorProcessKeypressMark(c);
		editorRefreshScreen();

		int currX = FRAME->rx, currY = FRAME->cy;

		if (currY == anchorY)
		{
			int beg = anchorRX < currX ? anchorRX: currX;
			int end = currX > anchorRX ? currX : anchorRX;
			for (int i = beg; i <= end; i++)
				MARK_CHAR(currY - FRAME->rowoff, i - FRAME->coloff);
		}
		else if (currY > anchorY)
		{
			for (int i = anchorRX; i < BUFFER->row[anchorY].rsize; i++)
				MARK_CHAR(anchorY - FRAME->rowoff, i - FRAME->coloff);

			for (int i = anchorY + 1; i < currY; i++)
				for (int j = 0; j < BUFFER->row[i].rsize; j++)
					MARK_CHAR(i - FRAME->rowoff, j - FRAME->coloff);

			for (int i = 0; i <= currX; i++)
				MARK_CHAR(currY - FRAME->rowoff, i - FRAME->coloff);

		}
		else {
			for (int i = anchorRX; i >= 0; i--)
				MARK_CHAR(anchorY - FRAME->rowoff, i - FRAME->coloff);

			for (int i = currY + 1; i < anchorY; i++)
				for (int j = 0; j < BUFFER->row[i].rsize; j++)
					MARK_CHAR(i - FRAME->rowoff, j - FRAME->coloff);

			for (int i = currX; i <= BUFFER->row[currY].rsize; i++)
				MARK_CHAR(currY - FRAME->rowoff, i - FRAME->coloff);
		}

		wrefresh(FRAME->frame);
	}
#undef MARK_CHAR

	cursor_set_color_rgb(NORMAL_CURSOR);
	editorSetStatusMessage(L"Marking time done");
	editorDrawMessageBar();

	E.regionMarked[0] = anchorY;
	E.regionMarked[1] = anchorX;
	E.regionMarked[2] = FRAME->cy;
	E.regionMarked[3] = FRAME->cx;

	/* TODO: this is terrible erognomics for the user */
	if (wget_wch(FRAME->frame, &c) != ERR)
		editorProcessMarkActions(c);
}
