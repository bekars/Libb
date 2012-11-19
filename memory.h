/**
 * libx memory functions file
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   implement memory control functions.
 *
 */

#ifndef __LIBX_MEMORY_H__
#define __LIBX_MEMORY_H__

STATIC_INLINE void
memerror(const char *what)
{
	xerror("OOM - %s\n", what);
	exit(1);
}

STATIC_INLINE void *
xmalloc_real(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr) {
		memerror("malloc");
	}

	memset(ptr, 0, size);

	return ptr;
}

STATIC_INLINE void *
xrealloc_real(void *ptr, size_t newsize)
{
	void *newptr;

	if (ptr) {
		newptr = realloc(ptr, newsize);
	} else {
		newptr = malloc(newsize);
	}

	if (!newptr) {
		memerror("realloc");
	}

	return newptr;
}

STATIC_INLINE char *
xstrdup_real(const char *s)
{
	char *copy;

#ifndef HAVE_STRDUP
	int l = strlen(s);
	copy = malloc(l + 1);
	if (!copy) {
		memerror("strdup");
	}
	memcpy(copy, s, l + 1);
#else  /* HAVE_STRDUP */
	copy = strdup(s);
	if (!copy) {
		memerror("strdup");
	}
#endif /* HAVE_STRDUP */

	return copy;
}

STATIC_INLINE void
xfree_real(void *ptr)
{
	free(ptr);
	ptr = NULL;
}

#ifndef __X_DEBUG__

#define xmalloc  	xmalloc_real
#define xfree    	xfree_real
#define xrealloc 	xrealloc_real
#define xstrdup  	xstrdup_real

#else

#define xmalloc(s)     	xmalloc_debug(s, __FILE__, __LINE__)
#define xfree(p)       	xfree_debug(p, __FILE__, __LINE__)
#define xrealloc(p, s) 	xrealloc_debug(p, s, __FILE__, __LINE__)
#define xstrdup(p)     	xstrdup_debug(p, __FILE__, __LINE__)

#define MM_DEBUG_MAX 1000000
extern unsigned int malloc_count, free_count;

struct memrecord {
	char *ptr;
	unsigned int size;
	const char *file;
	int line;
};

extern struct memrecord malloc_debug[MM_DEBUG_MAX];

STATIC_INLINE void
register_abort(const char *what)
{
	xprintf("register malloc error - %s.\n", what);
}

STATIC_INLINE void
register_ptr(void *ptr, unsigned int size, const char *file, int line)
{
	int i;
	
	malloc_count += size;
	
	for (i = 0; i < ARRAY_SIZE(malloc_debug); ++i) {
		if (malloc_debug[i].ptr == NULL) {
			malloc_debug[i].ptr = ptr;
			malloc_debug[i].size = size;
			malloc_debug[i].file = file;
			malloc_debug[i].line = line;
			return;
		}
	}
	
	register_abort("no record space");
}

STATIC_INLINE void
unregister_ptr(void *ptr)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(malloc_debug); ++i) {
		if (malloc_debug[i].ptr == ptr) {
			free_count += malloc_debug[i].size;
			malloc_debug[i].ptr = NULL;
			return;
		}
	}
	
	register_abort("no find the ptr");
}

STATIC_INLINE void
dump_malloc_stats(void)
{
	int i;
	printf("\nMalloc:  %d\nFree:    %d\nBalance: %d\n\n",
			malloc_count, 
			free_count, 
			malloc_count - free_count);

	for (i = 0; i < ARRAY_SIZE(malloc_debug); ++i) {
		if (malloc_debug[i].ptr != NULL) {
			printf("%2d> 0x%08ld:%3d   %s:%d\n",
					i + 1,
					(long)malloc_debug[i].ptr,
					malloc_debug[i].size,
					malloc_debug[i].file, 
					malloc_debug[i].line);
		}
	}
}

STATIC_INLINE void *
xmalloc_debug(size_t size, const char *source_file, int source_line)
{
	void *ptr = xmalloc_real(size);
	register_ptr(ptr, size, source_file, source_line);
	return ptr;
}

STATIC_INLINE void
xfree_debug(void *ptr, const char *source_file, int source_line)
{
	assert(ptr != NULL);
	unregister_ptr(ptr);
	xfree_real(ptr);
}

STATIC_INLINE void *
xrealloc_debug(void *ptr, size_t newsize, const char *source_file, int source_line)
{
	void *newptr = xrealloc_real(ptr, newsize);

	if (!ptr) {
		register_ptr(newptr, newsize, source_file, source_line);
	} else if (newptr != ptr) {
		unregister_ptr(ptr);
		register_ptr(newptr, newsize, source_file, source_line);
	}

	return newptr;
}

STATIC_INLINE char *
xstrdup_debug(const char *s, const char *source_file, int source_line)
{
	char *copy = xstrdup_real(s);
	register_ptr(copy, strlen(copy), source_file, source_line);
	return copy;
}

#endif /* __X_DEBUG__ */

#endif /* __LIBX_MEMORY_H__ */

