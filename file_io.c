#include "marisa.h"

extern editorConfig E;

wchar_t *
editorRowsToString (int *buflen)
{
	int totlen = 0;
	wchar_t *buf, *p;

	if (BUFFER->numrows == 0)
	{
		return NULL;
	}
	else
	{
		for (int i = 0; i < BUFFER->numrows; i++)
			totlen += BUFFER->row[i].size + 1; /* need to add one to add the new line character at the end of each row */
	}

	*buflen = totlen;
	buf = (wchar_t *) malloc(totlen * sizeof(wchar_t));
	p = buf;
	for (int i = 0; i < BUFFER->numrows; i++)
	{
		memcpy(p, BUFFER->row[i].chars, BUFFER->row[i].size);
		p += BUFFER->row[i].size;
		*p = '\n';
		p++;
	}

	return buf;
}

void
editorOpen (char *filename)
{
	FILE *fp;
	wchar_t *line = NULL;
	char *fileLine = NULL;
	size_t linecap = 0;
	ssize_t linelen;

	free(BUFFER->filename);
	BUFFER->filename = strdup(filename);

	if ((fp = fopen(filename, "r")) == NULL)
		die(L"fopen");

	/* TODO: set buffer name to filename */
	char *newBufferName;
	strncpy(newBufferName, filename, sizeof(char) * strlen(filename));
	newBuffer(&newBufferName, filename);
	switchBuffer(newBufferName);

	while ((linelen = getline(&fileLine, &linecap, fp)) != -1)
	{
		line = stows(fileLine, linelen);
		while (linelen > 0 && (line[linelen - 1] == '\n' ||
		    line[linelen - 1] == '\r'))
			linelen--;
		bufferInsertRow(BUFFER->numrows, line, linelen);
	}

	free(line);
	fclose(fp);
	BUFFER->flags.dirty = CLEAN;
}

void
editorSave (void)
{
	int len;
	int fd;
	wchar_t *buf;

	if (BUFFER->filename == NULL)
		BUFFER->filename = editorPrompt(L"Save as: %s (ESC to cancel)");

	if (BUFFER->filename == NULL) {
		editorSetStatusMessage(L"Save aborted");
		return;
	}

	if ((buf = editorRowsToString(&len)) == NULL || BUFFER->flags.dirty == CLEAN)
	{
		editorSetStatusMessage(L"Nothing to save...");
		return;
	}

	if ((fd = open(BUFFER->filename, O_RDWR | O_CREAT, 0644)) != -1)
	{
		if (ftruncate(fd, len) != -1)
		{
			if (write(fd, buf, len) == len)
			{
				close(fd);
				free(buf);
				BUFFER->flags.dirty = CLEAN;
				editorSetStatusMessage(L"%d bytes written to disk", len);
				return;
			}
		}
		close(fd);
	}

	free(buf);
	editorSetStatusMessage(L"Can't save! I/O error: %s", strerror(errno));
}