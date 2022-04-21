#ifndef MARISA_BUFFER
#define MARISA_BUFFER

typedef struct flags flags;
typedef struct erow erow;
typedef struct state state;
typedef struct stateStack stateStack;
typedef struct textEditorBuffer textEditorBuffer;
typedef enum operation operation;

enum operation {DELETE, INSERT};

struct flags {
	unsigned int dirty : 1; /* DIRTY or CLEAN */
	unsigned int mode : 1;  /* INSERT or VISUAL */
	unsigned int state: 1;  /* REDO or UNDO */
};

struct erow {
	int size;
	int rsize;
	wchar_t *chars;
	wchar_t *render;
};

/*
 * state is added to the stack when going from insertion to visual mode
 * every buffer modifying operation done in visual mode is added as state
 */
struct state {
	int cx;                /* column */
	int cy;                /* row */
	int textSize;
	char *text;            /* the text that was either inserted or deleted */
	operation op;          /* either DELETE or INSERT */
	textEditorBuffer *buffer;
};

/*
 * will have at most 2 pointers pointing to it
 * one from editorConfig and one from frame
 */
struct textEditorBuffer {
	unsigned long size;    /* buffer size in bytes */
	unsigned long length;  /* number of lines in buffer */
	int numrows;           /* number of rows in file */
	flags flags;           /* flags */
	char *filename;
	char *buffername;
	erow *row;             /* array of the file rows in memory */
	state *stateStack;
};

int bufferRowCxToRx (erow *, int);
void bufferUpdateRow (erow *);
void bufferInsertRow (int, wchar_t *, size_t);
void bufferFreeRow (erow *);
void bufferDelRow (int);
void bufferRowInsertChar (erow *, int, int);
void bufferRowAppendString (erow *, wchar_t *, size_t);
void bufferDelRegion (int, int, int, int);
void bufferRowDelChar (erow *, int);
void killBuffer (textEditorBuffer *);                                       /* TODO */
void readOnlyToggle (void);                                                 /* TODO */
/* if 2 buffers with same name add the working directory name */
void userBufferName (void);                                                 /* TODO */
void saveBuffer (void);                                                     /* TODO */
/* for running shell operations on a buffer, add some prefix to it like ~#${filename} */
void saveBufferToTemp (void);                                               /* TODO */
/*
 * listBuffers() will open a mini buffer which the user can see all the buffers (in alpha order) open
 * Then at the bottom there will be a user input section where user can autocomplete there way to a buffer
 * then it calls gotoBuffer, and the window will switch to the new buffer
 */
void listBuffers (void);                                                    /* TODO */
void newBuffer (char **, char *);
gboolean bufferNamesEquals (gconstpointer, gconstpointer);
gboolean bufferFileEquals (gconstpointer, gconstpointer);
/* add a new state to the stateStack */
void updateState (void);
/* doesn't acutal pop off the stack, just move the current state down in the stack */
void reverseState (void);
/* doesn't acutal pop off the stack, just move the current state up in the stack */
void forwardState (void);
/* free the stateStack */
void cleanState (void);

/* get back the argument buffer's index by searching for the buffername */
#define getBufferIndexName(p, i) g_ptr_array_find_with_equal_func(E.buffers, p, bufferNamesEquals, i)
/* get back the argument buffer's index by searching for the filename */
#define getBufferIndexFile(p, i) g_ptr_array_find_with_equal_func(E.buffers, p, bufferFileEquals, i)

#endif /* MARISA_BUFFER header guard */
