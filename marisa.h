#ifndef MARISA
#define MARISA

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>
#include <locale.h>
#include <glib.h>

#include "buffer.h"
#include "frame.h"

#define MARISA_VERSION "0.0.1"
#define MARISA_TAB_STOP 8
#define CTRL_KEY(k) ((k) & 0x1f)
#define NORMAL_MODE 0
#define VISUAL_MODE 1
#define TEXTWINDOW_COLOR 1
#define STATUSBAR_COLOR 2
#define MAP_KEY_CHORD_MAX 2
#define MAP_KEY_LAYER_ONE 0
#define MAP_KEY_LAYER_TWO 1
#define CLEAN 0
#define DIRTY 1
#define BLURT fprintf(stderr, "%s:%d\tfunction %s %s\n", __FILE__, __LINE__, __func__, E->filename);
#define SWITCH_MODE (E.currentFrame->currentBuffer->flags.mode = E.currentFrame->currentBuffer->flags.mode == NORMAL_MODE ? VISUAL_MODE : NORMAL_MODE)

enum editorKey {
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN
};

/*** data ***/
typedef struct editorConfig editorConfig;

typedef struct statusBar statusBar;
typedef struct popupFrame popupFrame;

struct statusBar {
	WINDOW *statusBarFrame;
	char statusmsg[80];
	size_t statusmsgSize;
	time_t statusmsg_time;
};

struct popupFrame {
	int i;
};

struct editorConfig {
	GPtrArray *frames;         /* pointer to head of frame linked list */
	GPtrArray *buffers;        /* pointer to head of  buffer linked list */
	frame *currentFrame;
	statusBar *bar;
	int numOfFrames;           /* number of frames starting from 0 */
	int numOfBuffer;           /* number of frames starting from 0 */

};

#define FRAME E.currentFrame
#define BUFFER E.currentFrame->currentBuffer

void editorInsertChar (int);
void editorInsertNewline (void);
void editorDelChar (void);

char *editorRowsToString (int *);
void editorOpen (char *);
void editorSave (void);

void initEditor (editorConfig *);

char *editorPrompt (char *);
void editorProcessKeypressNormal (void);
void editorProcessKeypressVisual (void);
void editorMoveCursor (int);

void switchModes (void);

void setupTerm (void);
void die (const char *);
int readKeyChord (void);
void readSpaceMap (void);
int editorReadKeyNormal (void);
int editorReadKeyVisual (void);

#endif /* MARISA header guard */
