#include "marisa.h"

void
initEditor (editorConfig *E)
{
	E->frames = NULL;
	E->buffers = NULL;

	cursor_set_color_rgb(0xff, 0xff, 0xff);

	char *firstName = "*first*";
	char *secondName = "*second*";
	char *thirdName = "*third*";

	newBuffer(&firstName, NULL);
	newBuffer(&secondName, NULL);
	newBuffer(&thirdName, NULL);
	newFrame(createNewWin(LINES - 2, COLS/2 - 1, 0, 0, TEXTWINDOW_COLOR), (textEditorBuffer *) g_ptr_array_index(E->buffers, 0));
	newFrame(createNewWin(LINES/2 - 1, COLS/2 - 1, 0, COLS/2, TEXTWINDOW_COLOR), (textEditorBuffer *) g_ptr_array_index(E->buffers, 1));
	newFrame(createNewWin(LINES/2 - 2, COLS/2 - 1, LINES/2, COLS/2, TEXTWINDOW_COLOR), (textEditorBuffer *) g_ptr_array_index(E->buffers, 2));

	E->currentFrame = (frame *) g_ptr_array_index(E->frames, 0);

	E->bar = malloc(sizeof(statusBar) * 1);
	E->bar->statusBarFrame = createNewWin(1, COLS, LINES - 2, 0, STATUSBAR_COLOR);
	E->bar->statusmsg[0] = L'\0';
	E->bar->statusmsgSize = (size_t) (sizeof(char) * 80);
	E->bar->statusmsg_time = 0;

	E->mes = malloc(sizeof(messageBar) * 1);
	E->mes->messageBarFrame = createNewWin(1, COLS, LINES - 1, 0, STATUSBAR_COLOR);
	E->mes->message[0] = L'\0';
	E->mes->messageLen = (size_t) (sizeof(char) * 80);
	E->mes->messageTime = 0;

	E->numOfFrames = 0;
	E->numOfBuffer = 0;

   	((frame *) g_ptr_array_index(E->frames, 0))->currentBuffer = (textEditorBuffer *) g_ptr_array_index(E->buffers, 0);
}
