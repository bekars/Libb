/**
 * libx test function
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   create the file.
 *
 */

#ifdef __X_DEBUG__
#include "libx.h"

int
test_main(int argc, char **argv)
{
	char *p = NULL;

	p = xmalloc(1000000000);

	dump_malloc_stats();
	
	xfree(p);

	dump_malloc_stats();

	exit(0);
}

#endif /* __X_DEBUG__ */
#include "libx.h"
