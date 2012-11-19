/**
 * libx utils function file
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   implement utils functions.
 *
 */

#ifndef __LIBX_UTILS_H__
#define __LIBX_UTILS_H__

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define LOCK_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

typedef unsigned int 	u32;
typedef unsigned short 	u16;
typedef unsigned char 	u8;

extern void strip_str(const char * const str, char *buf, char *tag);

void xdaemon(const char * const cmd);

int lockfile(int fd);

int single_process(const char * const lockfile);

int parse_arg(int argc, char **argv);

typedef int (*lscb)(const char *pname);
extern int list_dir(const char *path, lscb func);

#endif /* __LIBX_UTILS_H__ */

