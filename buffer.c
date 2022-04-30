#include "marisa.h"

extern editorConfig E;
extern wchar_t *cwd;   /* current working directory of the program */

int
bufferRowCxToRx (erow *row, int beg, int cx)
{
	int rx = 0;
	for (int i = beg; i < cx; i++)
	{
		wchar_t w = row->chars[i];
		if (w == '\t')
		{
			rx += (MARISA_TAB_STOP) - (rx % MARISA_TAB_STOP);
		}
		else
		{
			rx += wcwidth(w);
		}
	}
	return rx;
}

void
bufferUpdateRow (erow *row)
{
	int tabs = 0;
	int idx = 0;

	for (int i = 0; i < row->size; i++)
		if (row->chars[i] == '\t')
			tabs++;

	free(row->render);
	row->render = (wchar_t *) malloc((row->size + tabs * MARISA_TAB_STOP + 1) * sizeof(wchar_t));

	for (int i = 0; i < row->size; i++)
	{
		if (row->chars[i] == '\t')
		{
			row->render[idx++] = ' ';
			while (idx % MARISA_TAB_STOP != 0) row->render[idx++] = ' ';
		}
		else
		{
			row->render[idx++] = row->chars[i];
		}
	}
	row->render[idx] = L'\0';
	row->rsize = idx;
}

void
bufferInsertRow (int at, wchar_t *s, size_t len)
{
	if (at < 0 || at > BUFFER->numrows) return;

	BUFFER->row = (erow *) realloc(BUFFER->row, sizeof(erow) * (BUFFER->numrows + 1));
	memmove(&BUFFER->row[at + 1], &BUFFER->row[at], sizeof(erow) * (BUFFER->numrows - at));

	BUFFER->row[at].size = len;
	BUFFER->row[at].chars = (wchar_t *) malloc((len + 1) * sizeof(wchar_t));

	wmemcpy(BUFFER->row[at].chars, s, len);
	BUFFER->row[at].chars[len] = L'\0';

	BUFFER->row[at].rsize = 0;
	BUFFER->row[at].render = NULL;
	bufferUpdateRow (&BUFFER->row[at]);

	BUFFER->numrows++;
	BUFFER->flags.dirty = DIRTY;
}


void
bufferFreeRow (erow *row)
{
	free(row->render);
	free(row->chars);
}

void
bufferDelRow (int at)
{
	if (at < 0 || at >= BUFFER->numrows)
		return;

	bufferFreeRow(&BUFFER->row[at]);
	memmove(&BUFFER->row[at], &BUFFER->row[at + 1], sizeof(erow) * (BUFFER->numrows -at -1));
	BUFFER->numrows--;
	BUFFER->flags.dirty = DIRTY;
}

void
bufferRowInsertChar (erow *row, int lineIndex, int c)
{
	if (lineIndex < 0 || lineIndex > row->size)
		lineIndex = row->size;

	row->chars = (wchar_t *) realloc(row->chars, (row->size + 2) * sizeof(wchar_t));
	wmemmove(&row->chars[lineIndex + 1], &row->chars[lineIndex], row->size - lineIndex + 1);
	row->size++;
	row->chars[lineIndex] = c;
	bufferUpdateRow (row);
	BUFFER->flags.dirty = DIRTY;
}

void
bufferRowAppendString (erow *row, wchar_t *s, size_t len)
{
	row->chars = (wchar_t *) realloc(row->chars, (row->size + len + 1) * sizeof(wchar_t));
	wmemcpy(&row->chars[row->size], s, len);
	row->size += len;
	row->chars[row->size] = L'\0';
	bufferUpdateRow (row);
	BUFFER->flags.dirty = DIRTY;
}

/* TODO; without bound checking, it segfaults */
void
bufferDelRegion (void)
{
#define region E.regionMarked

	/* region[4] = {begY, begX, endY, endX} */
	int minRow = region[0] <= region[2] ? region[0] : region[2];
	int minCol = region[0] <= region[2] ? region[1] : region[3];
	int maxRow = region[0] > region[2] ? region[0] : region[2];
	int maxCol = region[0] > region[2] ? region[1] : region[3];

#undef region

	/* first row in a multirow region */
	int firstRowColEnd = minRow == maxRow ? maxCol : BUFFER->row[minRow].size;
	wmemmove(&BUFFER->row[minRow].chars[minCol],
	    &BUFFER->row[minRow].chars[firstRowColEnd], BUFFER->row[maxRow].size - firstRowColEnd);
	BUFFER->row[minRow].size -= firstRowColEnd - minCol;

	if (BUFFER->row[minRow].size == 0)
	{
		bufferDelRow(minRow);
	}
	else
	{
		bufferUpdateRow(&BUFFER->row[minRow]);
	}

	/* Inbetween rows in a multirow region */
	while(minRow < maxRow)
	{
		bufferDelRow(minRow);
		maxRow--;
	}

	/* Last row in a multirow region */
	if (minRow != maxRow)
	{
		wmemmove(&BUFFER->row[maxRow].chars[0],
		    &BUFFER->row[maxRow].chars[maxCol], BUFFER->row[maxRow].size - maxCol);
		BUFFER->row[maxRow].size -= maxCol;
		bufferUpdateRow(&BUFFER->row[maxRow]);

		if (BUFFER->row[maxRow].size == 0)
			bufferDelRow(maxRow);
	}

	FRAME->cx = minCol;
	FRAME->cy = minRow;
}

void
bufferRowDelChar (erow *row, int at)
{
	if (at < 0 || at >= row->size)
		return;

	wmemmove(&row->chars[at], &row->chars[at + 1], row->size - at);
	row->size--;
	bufferUpdateRow(row);
	BUFFER->flags.dirty = DIRTY;
}

/*
 * free the buffer, might need to free the dymanically allocated memory manually with free()
 * close the file
*/
void killBuffer (textEditorBuffer *bufToBeKilled)
{
	if (bufToBeKilled->flags.dirty == DIRTY)
	{
		/* TODO: send a editorMessage using the Message interface */
		editorSave();
	}

	guint bufIndex;
	getBufferIndexName(bufToBeKilled, &bufIndex);
	g_ptr_array_remove_index_fast(E.buffers, bufIndex);
}

void readOnlyToggle (void)
{
	return;
}

/* this function is called when user input is needed */
void userBufferName (void)
{
	return;
}

void saveBuffer (void)
{
	return;
}

void saveBufferToTemp (void)
{
	return;
}

void listBuffers (void)
{
	return;
}

void newBuffer (char **bufferName, char *fileName)
{
	assert(bufferName != NULL);
	if (E.buffers == NULL)
		E.buffers = g_ptr_array_sized_new(10);

	textEditorBuffer *newBuffer = g_new(textEditorBuffer, 1);
	newBuffer->size = 0;
	newBuffer->length = 0;
	newBuffer->numrows = 0;
	newBuffer->flags.dirty = CLEAN;
	newBuffer->flags.dirty = NORMAL_MODE;
	newBuffer->filename = fileName;
	/*
	 * TODO: just append a hash or something to the bufferName to make it unique
	 * Have a hashtable with the corisponding bufferList buffer name and the actual buffer name (the one with the has)
	 * Show the user something like /home/john/Documents/programming/ -- test.c so they can distinguish between files with the same name
	 * The actual buffer name in the program would be like test.c_asx823
	 */
	newBuffer->buffername = *bufferName;
	newBuffer->row = NULL;
	g_ptr_array_add(E.buffers, newBuffer);
}

gboolean
bufferNamesEquals (gconstpointer a, gconstpointer b)
{
	if (strcmp(((const textEditorBuffer *) a)->buffername, ((const textEditorBuffer *) b)->buffername) == 0)
		return TRUE;

	return FALSE;
}

gboolean
bufferFileEquals (gconstpointer a, gconstpointer b)
{
      	if (strcmp(((const textEditorBuffer *) a)->filename, ((const textEditorBuffer *) b)->filename) == 0)
		return TRUE;

	return FALSE;
}

/* TODO: not used  */
/* static gint */
/* sortBufferName (gconstpointer a, gconstpointer b) */
/* { */
/* 	const textEditorBuffer *bufferA = *((textEditorBuffer **) a); */
/* 	const textEditorBuffer *bufferB = *((textEditorBuffer **) b); */

/* 	return g_ascii_strcasecmp (bufferA->buffername, bufferB->buffername); */
/* } */
