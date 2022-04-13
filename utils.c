#include "marisa.h"

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
