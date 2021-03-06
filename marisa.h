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
#include <stddef.h>
#include <wchar.h>
#include <wctype.h>
#include <glib.h>

#include "buffer.h"
#include "frame.h"
#include "utils.h"

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
#define NORMAL_CURSOR 171, 167, 163
#define MARK_CURSOR 0, 102, 255
#define SWITCH_MODE (E.currentFrame->currentBuffer->flags.mode = E.currentFrame->currentBuffer->flags.mode == NORMAL_MODE ? VISUAL_MODE : NORMAL_MODE)

/*** data ***/
typedef struct editorConfig editorConfig;

typedef struct statusBar statusBar;
typedef struct messageBar messageBar;
typedef struct popupFrame popupFrame;


/* TODO: rename these */
struct statusBar {
	WINDOW *statusBarFrame;
	wchar_t statusmsg[80];
	size_t statusmsgSize;
	time_t statusmsg_time;
};

struct messageBar {
	WINDOW *messageBarFrame;
	wchar_t message[80];
	size_t messageLen;
	time_t messageTime;
};

struct popupFrame {
	int i;
};

struct editorConfig {
	GPtrArray *frames;         /* pointer to head of frame linked list */
	GPtrArray *buffers;        /* pointer to head of  buffer linked list */
	frame *currentFrame;
	statusBar *bar;
	messageBar *mes;
	int numOfFrames;           /* number of frames starting from 0 */
	int numOfBuffer;           /* number of frames starting from 0 */
	int regionMarked[4];       /* coordinates of marked region */
};

#define FRAME E.currentFrame
#define BUFFER E.currentFrame->currentBuffer

void editorInsertChar (int);
void editorInsertNewline (void);
void editorDelChar (void);
void editorMarkRegion (void);

wchar_t *editorRowsToString (int *);
void editorOpen (char *);
void editorSave (void);

void initEditor (editorConfig *);

char *editorPrompt (wchar_t *);
void editorProcessKeypressNormal (void);
void editorProcessKeypressVisual (void);
void editorMoveCursor (int);
void editorProcessKeypressMark (wint_t);
void editorProcessMarkActions (wint_t);

void switchModes (void);

void setupTerm (void);
void die (const wchar_t *);
int readKeyChord (void);
void readSpaceMap (void);
int editorReadKeyNormal (void);
int editorReadKeyVisual (void);

#endif /* MARISA header guard */
