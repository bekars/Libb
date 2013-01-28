/**
 * file read/mmap implements
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   implement file read/mmap function.
 *
 */

#ifndef __LIBX_FILE_H__
#define __LIBX_FILE_H__


struct file_memory {
	char *content;
	long length;
	int mmap_p;
};

long file_size(const char *filename);

struct file_memory * read_file(const char *file);

void read_file_free(struct file_memory *fm);

#endif /* __LIBX_FILE_H__ */

