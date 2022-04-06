#ifndef MARISA_UTILS
#define MARISA_UTILS

void autoCompleteBuffers (void); /* TODO */
void autoCompleteFiles (void); /* TODO */
void highlighMatchingBracketPairs (void); /* TODO */
void findMatchingBracketPairs (void); /* TODO */

wchar_t *dupstr (const wchar_t *s, size_t n);
wchar_t *stows (const char *s, size_t n);
char *wstos (const wchar_t *s, size_t n);
const char *trimleft (const char *s);
void cursor_set_color_rgb (unsigned char, unsigned char, unsigned char);

#endif /* MARISA_UTILS header guard */
