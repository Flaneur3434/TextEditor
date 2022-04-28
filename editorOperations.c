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
	int anchorXFrame = bufferRowCxToRx(&BUFFER->row[FRAME->cy], FRAME->cx)  - FRAME->coloff;
	int anchorYFrame = FRAME->cy - FRAME->rowoff;

	int anchorX = FRAME->cx;
	int anchorY = FRAME->cy;

	editorSetStatusMessage(L"Marking time");
        editorDrawMessageBar();
	cursor_set_color_rgb(MARK_CURSOR);
	/* move back to the text window */
        wmove(FRAME->frame, anchorYFrame, anchorXFrame);

        wint_t c;
        while ((wget_wch(FRAME->frame, &c) != ERR) && (c != CTRL_KEY('q')))
	{
		editorProcessKeypressMark(c);
		editorRefreshScreen();

		int currX = 0, currY = 0;
		getyx(FRAME->frame, currY, currX);

		if (currY == anchorYFrame)
		{
			int beg = anchorXFrame < currX ? anchorXFrame: currX;
			int end = currX > anchorXFrame ? currX : anchorXFrame;
			for (int i = beg; i <= end; i++)
				mvwchgat(FRAME->frame, anchorYFrame, i - FRAME->coloff, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);
		}
		else if (currY > anchorYFrame)
		{
			for (int i = anchorXFrame; i < BUFFER->row[anchorYFrame].rsize; i++)
				mvwchgat(FRAME->frame, anchorYFrame, i - FRAME->coloff, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

			for (int i = anchorYFrame + 1; i < currY; i++)
				for (int j = 0; j < BUFFER->row[i].rsize; j++)
					mvwchgat(FRAME->frame, i, j - FRAME->coloff, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

			for (int i = 0; i <= currX; i++)
				mvwchgat(FRAME->frame, currY, i - FRAME->coloff, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

		}
		else {
			/* for some reason the cursor position is wrong, only in this block of code */
			for (int i = anchorXFrame; i >= 0; i--)
				mvwchgat(FRAME->frame, anchorYFrame, i - FRAME->coloff, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

			for (int i = currY + 1; i < anchorYFrame; i++)
				for (int j = 0; j < BUFFER->row[i].rsize; j++)
					mvwchgat(FRAME->frame, i, j - FRAME->coloff, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);

			for (int i = currX; i <= BUFFER->row[currY].rsize; i++)
				mvwchgat(FRAME->frame, currY, i - FRAME->coloff, 1, A_REVERSE, TEXTWINDOW_COLOR, NULL);
		}

		wrefresh(FRAME->frame);
	}

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
