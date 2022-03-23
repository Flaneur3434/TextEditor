#include "marisa.h"

editorConfig E;

int
main (int argc, char *argv[])
{
	setlocale(LC_CTYPE, "");  /* UTF-8 support */
	initEditor(&E);
	setupTerm();

	if (argc >= 2)
	{
		editorOpen(argv[1]); /* open file in first frame */
	}

	int x = 0;
	int y = 0;

	while(x < 20)
	{
		if (BUFFER->flags.mode == NORMAL_MODE)
			editorProcessKeypressNormal();
		else if (BUFFER->flags.mode == VISUAL_MODE)
			editorProcessKeypressVisual();

		switchFrame();
		mvwprintw(FRAME->frame, ++y, ++x, "Buffer name is: %s", BUFFER->buffername);
		editorRefreshScreen();
	}

	killFrame();

	/* need to free all memory the bufferArray and frameArray have */
	die("Program has ended");
	return 0;
}

/*
 * TODO: Refresh screen needs to be changed
 * TODO I need a text buffer (erow), which holds the whole file with user edits for each text editor buffer. The ncurses print function are just the screen buffers can shouldnt hold persistent data.
 * TODO: Change all abuf operations to ncurse operations
 *
 * There will be 1 text window which can switch between text editor buffers to show the user
 * There will be 1 status line window
 * There will be 1 command window where users can input commands into like %s/a/A/g
 * There will be mini-buffers (popups) which hide the status line and command window
 *
 * 1. regex searching
 * 2. Undo Redo
 * 3. Copy Passting
 * 4. Multiple Buffers
 * 5. Module Editing
 * 6. Multiple Windows (frames) https://www.viget.com/articles/c-games-in-ncurses-using-multiple-windows/
 */

/*
 * Look at mle cmd.c on how to implement external commands
 */

/*
 * For example, if you need to shift the entire file down a few hundred bytes in
 * order to insert an ID3 tag, one simple approach would be to expand the file
 * with ftruncate(), mmap the file, then memmove() the contents down a bit. This,
 * however, will destroy the file if your program crashes while it's running. You
 * could also copy the contents of the file into a new file - this is another
 * place where mmap() really shines; you can simply mmap() the old file, then
 * copy all of its data into the new file with a single write().
 *
 * In short, mmap() is great if you're doing a large amount of IO in terms of total
 * bytes transferred; this is because it reduces the number of copies needed, and
 * can significantly reduce the number of kernel entries needed for reading cached
 * data. However mmap() requires a minimum of two trips into the kernel (three if
 * you clean up the mapping when you're done!) and does some complex internal
 * kernel accounting, and so the fixed overhead can be high.
 *
 * read() on the other hand involves an extra memory-to-memory copy, and can thus
 * be inefficient for large I/O operations, but is simple, and so the fixed
 * overhead is relatively low. In short, use mmap() for large bulk I/O, and read()
 * or pread() for one-off, small I/Os.
 *
 * https://stackoverflow.com/questions/45972/mmap-vs-reading-blocks
 * https://stackoverflow.com/questions/5588605/mmap-vs-read
 * https://www.sublimetext.com/blog/articles/use-mmap-with-care
 * https://linuxhint.com/using_mmap_function_linux/
 */
