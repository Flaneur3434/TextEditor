#include "marisa.h"

void
initEditor (editorConfig *E)
{
	initscr(); /* ncurses make initial window */

	E->frames = NULL;
	E->buffers = NULL;

	newBuffer("*first*", NULL);
	newBuffer("*second*", NULL);
	newBuffer("*third*", NULL);
	newFrame(createNewWin(LINES - 2, COLS/2, 0, 0, TEXTWINDOW_COLOR), (textEditorBuffer *) g_ptr_array_index(E->buffers, 0));
	newFrame(createNewWin(LINES/2 - 2, COLS/2, 0, COLS/2, TEXTWINDOW_COLOR), (textEditorBuffer *) g_ptr_array_index(E->buffers, 1));
	newFrame(createNewWin(LINES/2 - 2, COLS/2, LINES/2, COLS/2, TEXTWINDOW_COLOR), (textEditorBuffer *) g_ptr_array_index(E->buffers, 2));

	E->currentFrame = (frame *) g_ptr_array_index(E->frames, 0);
	E->bar = malloc(sizeof(statusBar) * 1);
	/* E->bar->statusBarFrame = createNewWin(1, COLS, LINES - 1, 0, 2); */
	/* E->bar->statusmsg[0] = '\0'; */
	/* E->bar->statusmsg_time = 0;  */
	E->numOfFrames = 0;
	E->numOfBuffer = 0;

   	((frame *) g_ptr_array_index(E->frames, 0))->currentBuffer = (textEditorBuffer *) g_ptr_array_index(E->buffers, 0);
}
