/**
 * libx utils function file
 *
 * bekars <bekars@gmail.com>
 *
 * History:
 * 	2010.08.02  bekars   implement utils functions.
 *
 */

#include "libx.h"

/**
 * strip_str - strip string by tags
 * 
 * @str: string wait for strip
 * @buf: store the resule
 * @tag: string char
 */ 
void
strip_str(const char * const str, char *buf, char *tag)
{
	int i, j = 0;
	int len = strlen(str);

	for (i = 0; i < len; ++i) {
		if (!strchr(tag, str[i])) {
			buf[j++] = str[i];
		}
	}
}

/**
 * list_dir - list dir file and call cb func
 *
 * @path: the dir path
 * @func: call back routing
 */ 
int
list_dir(const char *path, lscb func)
{
	DIR *dp;
	struct dirent *dirp;
	struct stat statbuf;
	char fullpath[1024];

	if ((dp = opendir(path)) == NULL) {
		xerror("open module path %s error!\n", path);
		return -1;
	}

	while ((dirp = readdir(dp)) != NULL) {
		
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) {
			continue;
		}

		memset(fullpath, 0, sizeof(fullpath));
		sprintf(fullpath, "%s/%s", path, dirp->d_name);

		if (lstat(fullpath, &statbuf) < 0) {
			continue;
		}

		if (!S_ISDIR(statbuf.st_mode)) {
			func(fullpath);
		}
	}

	closedir(dp);

	return 0;
}

/**
 * xdaemon - process run on daemon mode
 * @name: log name
 *
 * use it in main function.
 */
void
xdaemon(const char * const name)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	int status;
	
	/* create file creation mask */
	umask(0);

	/* get maximum number of file descriptors */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
		xerror("can not get file limit\n");
		exit(1);
	}

	/* become a session leader to loss controlling ttys */
	if ((pid = fork()) < 0) {
		xerror("can not fork\n");
		exit(1);
	} else if (pid != 0) {	/* parent */
		exit(0);
	}

	setsid();
	
	/* ensure future opens won't allocate controlling ttys */
	if ((pid = fork()) < 0) {

		xerror("fork error !\n");
		exit(1);

	} else if (pid != 0) {	/* parent */

		for (;;) {
			
			/* wait for child, when error occur wake up a new child */
			if (waitpid(pid, &status, 0) < 0) {
				xerror("waitpid error !\n");
				exit(-1);
			}

			if (WIFEXITED(status)) {
				xsyslog("child process normal termination, status = %d\n", WEXITSTATUS(status));
			} else if (WIFSIGNALED(status)) {
				xsyslog("child process signal termination, signal = %d\n", WTERMSIG(status));
			} else if (WIFSTOPPED(status)) {
				xsyslog("child process stopped, signal = %d\n", WSTOPSIG(status));
			}

			if ((pid = fork()) < 0) {

				xerror("fork error !\n");
				exit(1);

			} else if (pid == 0) {
				/* new child begin run */
				break;
			}
		}
	}

	/* change the current working directory to the root */
	if (chdir("/") < 0) {
		xerror("can not chdir\n");
		exit(1);
	}

	/* close all open file descriptors */
	if (rl.rlim_max == RLIM_INFINITY) {
		rl.rlim_max = 1024;
	}
	
	for (i=0; i<rl.rlim_max; ++i) {
		close(i);
	}

	/* attach file descriptor 0, 1, 2 to /dev/null */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		xerror("unexpected file descriptors %d %d %d\n", fd0, fd1, fd2);
		exit(1);
	}

	/* initialize the log system */
	openlog(name, LOG_CONS, LOG_DAEMON);

}

/**
 * lockfile - test lock flag
 * @fd: lock fd
 */ 
int
lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return fcntl(fd, F_SETLK, &fl);
}

#define LOCK_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
/**
 * single_process - run sigle process
 * @file: lock file name
 *
 * let only one process run, lock file like "/var/run/rslogd_daemon.pid".
 */ 
int
single_process(const char * const file)
{
	int fd, ret;
	char buf[16];
	ssize_t size;

	fd = open(file, O_RDWR|O_CREAT, LOCK_MODE);
	if (fd < 0) {
		xerror("can not open file %s - %s\n", file, strerror(errno));
		exit(1);
	}

	if (lockfile(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return 1;
		}

		xerror("can not lock file %s - %s\n", file, strerror(errno));
		exit(1);
	}

	ret = ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	size = write(fd, buf, strlen(buf));

	close(fd);
	return 0;
}

/**
 * parse_arg_example - parse arguments example
 * @argc: argument number
 * @argv: argument array 
 *
 * just an example, write your own parse function
 */ 
int
parse_arg_example(int argc, char **argv)
{
	int next_option;
	const char * const short_options = "dh:";
	static const struct option long_options[] = {
		{"daemon",      0, NULL, 'd'},
		{"help",        1, NULL, 'h'},
		{NULL,          0, NULL, 0}
	};

	do {
		next_option = getopt_long(argc, argv, short_options, long_options, NULL);

		switch (next_option) {
			case 'd':
				//ac_daemon = 1;
				break;

			case 'h':
				printf("auto check by bekars 2009\n");
				exit(0);
				break;

			case -1:
				/* args parse end */
				break;

			case '?':
			default:
				/* args occur error */
				exit(1);
				break;
		}
	} while (next_option != -1);

	return 0;
}

