#include "marisa.h"

extern editorConfig E;
wchar_t *
stows (const char *s, size_t n)
{
	char *mbs = strndup(s, n);
	/* used to count number of potential wide characters */
	size_t wcn = mbstowcs(NULL, mbs, 0);

	wchar_t *wcs = calloc(wcn + 1, sizeof(wchar_t));
	if (!wcs) return free(mbs), NULL;

	size_t wcr = mbstowcs(wcs, mbs, wcn);
	if (wcr == (size_t) -1) return free(mbs), free(wcs), NULL;

	free(mbs);
	return wcs;
}

char *
wstos (const wchar_t *s, size_t n)
{
    wchar_t *wcs = calloc(n + 1, sizeof(wchar_t));
    if (!wcs)
        return NULL;
    wcsncpy(wcs, s, n);

    size_t mbn = wcstombs(NULL, wcs, 0);
    char *mbs = calloc(mbn + 1, sizeof(char));
    if (!mbs)
        return free(wcs), NULL;

    size_t mbr = wcstombs(mbs, wcs, mbn);
    if (mbr == (size_t)-1)
        return free(mbs), free(wcs), NULL;

    free(wcs);
    return mbs;
}

wchar_t *
dupstr (const wchar_t *s, size_t n)
{
    wchar_t *r = calloc(n + 1, sizeof(wchar_t));
    if (!r)
        return r;
    wmemcpy(r, s, n);
    return r;
}

const char *
trimleft (const char *s)
{
   if (!s)
      return NULL;
   while (*s && isspace(*s))
      s++;
   return s;
}

void
cursor_set_color_rgb (unsigned char red, unsigned char green, unsigned char blue)
{
    printf("\e]12;#%.2x%.2x%.2x\a", red, green, blue);
    fflush(stdout);
}

/* end up on next word's last character */
void
nextWord (erow *currentRow, int *cursorPos)
{
	if (*cursorPos == currentRow->size)
	{
		editorMoveCursor(KEY_RIGHT);
		return;
	}

	/* skip consecutive whitescpace */
	while (iswspace(currentRow->chars[*cursorPos + 1]) || iswpunct(currentRow->chars[*cursorPos + 1]))
	{
		*cursorPos += 1;
	}

	/* move to beginning on the word */
	*cursorPos += 1;

	for (int i = *cursorPos; i < currentRow->size; i++)
	{
		if (iswspace(currentRow->chars[i]) || iswpunct(currentRow->chars[i]))
		{
			*cursorPos = i;
			return;
		}
	}
	*cursorPos = currentRow->size;
}

/* end up on previous word's beginning character */
void
prevWord (erow *currentRow, int *cursorPos)
{
	if (*cursorPos == 0)
	{
		editorMoveCursor(KEY_LEFT);
		return;
	}

	/* skip space and punctuation, needed because we want to end up at the beginning on the previous word */
	while (iswspace(currentRow->chars[*cursorPos - 1]) || iswpunct(currentRow->chars[*cursorPos - 1]))
	{
		*cursorPos -= 1;
	}

	/* move to ending of the word */
	*cursorPos -= 1;

	for (int i = *cursorPos; i > 0; i--)
	{
		if (iswspace(currentRow->chars[i]) || iswpunct(currentRow->chars[i]))
		{
			*cursorPos = i + 1;
			return;
		}
	}
	*cursorPos = 0;
}

void
deleteNextWord (erow *currentRow, int *cursorPos)
{
	int i = *cursorPos > 0 ? *cursorPos : 0;

	if (i == currentRow->size)
	{
		editorMoveCursor(KEY_RIGHT);
		editorDelChar();
		return;
	}

	/* delete consecutive whitescpace */
	while (iswspace(currentRow->chars[i]) || iswpunct(currentRow->chars[i]))
	{
		bufferRowDelChar(currentRow, i);
	}

	while(i < currentRow->size)
	{
		if (iswspace(currentRow->chars[i]) || iswpunct(currentRow->chars[i]))
		{
			bufferRowDelChar(currentRow, i);
			break;
		}

		bufferRowDelChar(currentRow, i);
	}

	*cursorPos = i;
}

void
deletePrevWord (erow *currentRow, int *cursorPos)
{
	int i = *cursorPos > 0 ? *cursorPos : 0;

	if (i == 0)
	{
		editorDelChar();
		return;
	}

	while (i > 1)
	{
		if (i == currentRow->size)
			i--;

		if (iswspace(currentRow->chars[i]) || iswpunct(currentRow->chars[i]))
		{
			bufferRowDelChar(currentRow, i);
			break;
		}

		bufferRowDelChar(currentRow, i);
	}

	if (i == currentRow->size)
		i--;

	/* delete consecutive whitescpace */
	while (iswspace(currentRow->chars[i]) || iswpunct(currentRow->chars[i]))
	{
		bufferRowDelChar(currentRow, i--);
	}

	*cursorPos = i;
}
