#include "marisa.h"

extern editorConfig E;

void
switchModes (void)
{
	if (BUFFER->flags.mode == NORMAL_MODE)
	{
		BUFFER->flags.mode = VISUAL_MODE;
	}
	else if (BUFFER->flags.mode == VISUAL_MODE)
	{
		BUFFER->flags.mode = NORMAL_MODE;
	}
}
