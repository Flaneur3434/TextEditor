#include "marisa.h"

extern editorConfig E;
void
readSpaceMap (void)
{
    char seq[MAP_KEY_CHORD_MAX];

    if (read(STDIN_FILENO, &seq[MAP_KEY_LAYER_ONE], 1) != 1)
		return;

    if (read(STDIN_FILENO, &seq[MAP_KEY_LAYER_TWO], 1) != 1)
		return;

    switch (seq[0])
		{
		case ' ':
			SWITCH_MODE;
			break;
		case ';':
			editorSave();
			break;
		}
}
