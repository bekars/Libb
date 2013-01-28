/**
 * file read/mmap implements
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   implement file read/mmap function.
 *
 */

#include "libx.h"

/**
 * file_size - get the file size
 * @filename: file name string
 */ 
long
file_size(const char *filename)
{
	long size;
	/**
	 * We use fseek rather than stat to determine the file size because
	 * that way we can also verify whether the file is readable.
	 **/
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	return size;
}

/** 
 * Read FILE into memory.  A pointer to `struct file_memory' are
 * returned; use struct element `content' to access file contents, and
 * the element `length' to know the file length.  `content' is *not*
 * zero-terminated, and you should *not* read or write beyond the [0,
 * length) range of characters.
 *
 * After you are done with the file contents, call read_file_free to
 * release the memory.
 *
 * Depending on the operating system and the type of file that is
 * being read, read_file() either mmap's the file into memory, or
 * reads the file into the core using read().
 *
 * If file is named "-", fileno(stdin) is used for reading instead.
 * If you want to read from a real file named "-", use "./-" instead.  
 * 
 * @file: file name string
 */
struct file_memory *
read_file(const char *file)
{
	int fd;
	struct file_memory *fm;
	long size;
	int inhibit_close = 0;

	/* Some magic in the finest tradition of Perl and its kin: if FILE
	 * is "-", just use stdin.  */
	//if (HYPHENP(file)) {
	//	fd = fileno (stdin);
	//	inhibit_close = 1;
		/* Note that we don't inhibit mmap() in this case.  If stdin is
		 * redirected from a regular file, mmap() will still work.  */
	//} else {
		fd = open(file, O_RDONLY);
	//}

	if (fd < 0) {
		return NULL;
	}
	fm = xmalloc(sizeof (struct file_memory));

#ifdef HAVE_MMAP
	{
		struct stat buf;
		if (fstat(fd, &buf) < 0)
			goto mmap_lose;
		fm->length = buf.st_size;
		/* NOTE: As far as I know, the callers of this function never
		 * modify the file text.  Relying on this would enable us to
		 * specify PROT_READ and MAP_SHARED for a marginal gain in
		 * efficiency, but at some cost to generality.  */
		fm->content = mmap(NULL, fm->length, PROT_READ | PROT_WRITE,
				MAP_PRIVATE, fd, 0);
		if (fm->content == (char *)MAP_FAILED)
			goto mmap_lose;
		if (!inhibit_close)
			close(fd);

		fm->mmap_p = 1;
		return fm;
	}

mmap_lose:
	/* The most common reason why mmap() fails is that FD does not point
	 * to a plain file.  However, it's also possible that mmap() doesn't
	 * work for a particular type of file.  Therefore, whenever mmap()
	 * fails, we just fall back to the regular method.  */
#endif /* HAVE_MMAP */

	fm->length = 0;
	size = 512;			/* number of bytes fm->contents can
					   hold at any given time. */
	fm->content = xmalloc(size);
	
	while(1)
	{
		long nread;
		if (fm->length > size / 2)
		{
			/* #### I'm not sure whether the whole exponential-growth
			 * thing makes sense with kernel read.  On Linux at least,
			 * read() refuses to read more than 4K from a file at a
			 * single chunk anyway.  But other Unixes might optimize it
			 * better, and it doesn't *hurt* anything, so I'm leaving
			 * it.  */

			/* Normally, we grow SIZE exponentially to make the number
			 * of calls to read() and realloc() logarithmic in relation
			 * to file size.  However, read() can read an amount of data
			 * smaller than requested, and it would be unreasonable to
			 * double SIZE every time *something* was read.  Therefore,
			 * we double SIZE only when the length exceeds half of the
			 * entire allocated size.  */
			size <<= 1;
			fm->content = xrealloc(fm->content, size);
		}
		nread = read(fd, fm->content + fm->length, size - fm->length);
		if (nread > 0)
			/* Successful read. */
			fm->length += nread;
		else if (nread < 0)
			/* Error. */
			goto lose;
		else
			/* EOF */
			break;
	}

	if (!inhibit_close) {
		close(fd);
	}

	if (size > fm->length && fm->length != 0) {
		/* Due to exponential growth of fm->content, the allocated region
		 * might be much larger than what is actually needed.  */
		fm->content = xrealloc(fm->content, fm->length);
	}

	fm->mmap_p = 0;

	return fm;

lose:
	if (!inhibit_close) {
		close(fd);
	}
	xfree(fm->content);
	xfree(fm);
	return NULL;
}

/** 
 * Release the resources held by FM.  Specifically, this calls
 * munmap() or xfree() on fm->content, depending whether mmap or
 * malloc/read were used to read in the file.  It also frees the
 * memory needed to hold the FM structure itself.  
 * @fm: file memory struct
 */
void
read_file_free(struct file_memory *fm)
{
#ifdef HAVE_MMAP
	if (fm->mmap_p) {
		munmap(fm->content, fm->length);
	}
	else
#endif
	{
		xfree(fm->content);
	}
	xfree(fm);
}


