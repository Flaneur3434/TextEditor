#ifndef MARISA_FRAME
#define MARISA_FRAME

typedef struct frame frame;

struct frame {
	WINDOW *frame;
	textEditorBuffer *currentBuffer;
	int cx, cy; /* cursor position relative to the file */
	int rx;     /* cursor's file position after render */
	int rowoff; /* cursor row (y) position relative to the window */
	int coloff; /* cursor column (x) position relative to the window */
	int screenrows; /* max frame row */
	int screencols; /* max frame column */
};

void editorRefreshScreen (void);
void editorDrawRows (void);
void editorScroll (void);
void drawWelcome (void);
void editorDrawStatusBar (void);
void editorSetStatusMessage (const char *, ...);
void editorDrawMessageBar (void);
void newFrame (WINDOW *, textEditorBuffer *);
void switchBuffer (char *);
void switchFrame(void);
/* TODO: use delwin() to free window, use glib what ever to free frame struct */
void killFrame (void);
/* TODO: draw window in right dimension and size and boarder */
void drawFrame (WINDOW *);
/* TODO: resize all windows function */
void resizeWindow (void);
WINDOW *createNewWin (int, int, int, int, unsigned int);

/*
 * TODO: frame layout functions
 * Split vertical
 * Split horizontal
 */

#endif /* MARISA_FRAME header guard */
