/*
 * Print file permissions.
 *
 * Compile this file with Visual Studio and run the produced command in
 * console with a file name argument.  For example, command
 *
 *     stat include\dirent.h
 *
 * will output permissions of dirent.h to screen.
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#ifdef _MSC_VER
#	include <direct.h>
#	include <io.h>
#	define stat _stat64
#else
#	include <unistd.h>
#endif

static void stat_file(const char *fn);
static int _main(int argc, char *argv[]);

static int
_main(int argc, char *argv[])
{
	/* Require at least one file */
	if (argc == 1) {
		fprintf(stderr, "Usage: stat filename(s)\n");
		return EXIT_FAILURE;
	}

	/* For each file name argument in command line */
	int i = 1;
	while (i < argc) {
		stat_file(argv[i]);
		i++;
	}
	return EXIT_SUCCESS;
}

/*
 * Output file to screen
 */
static void
stat_file(const char *fn)
{
	struct stat buf;

	/* Get file permissions */
	if (stat(fn, &buf) != /*OK*/0) {
		fprintf(stderr, "Cannot stat %s (%s)\n", fn, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Determine file type */
	const char *type;
	if (S_ISFIFO(buf.st_mode))
		type = "pipe";
	else if (S_ISDIR(buf.st_mode))
		type = "directory";
	else if (S_ISREG(buf.st_mode))
		type = "file";
	else if (S_ISLNK(buf.st_mode))
		type = "link";
	else if (S_ISSOCK(buf.st_mode))
		type = "socket";
	else if (S_ISCHR(buf.st_mode))
		type = "character device";
	else if (S_ISBLK(buf.st_mode))
		type = "block device";
	else
		type = "unknown";

	/* Print file info */
	printf("name: %s\n", fn);
	printf("type: %s\n", type);
	printf("size: %lld\n", (long long) buf.st_size);
	printf("mode: 0%03o (%c%c%c%c%c%c%c%c%c%c)\n",
		buf.st_mode & (S_IRWXU|S_IRWXG|S_IRWXO),
		S_ISDIR(buf.st_mode) ? 'd' : '-',
		(buf.st_mode & S_IRUSR) ? 'r' : '-',
		(buf.st_mode & S_IWUSR) ? 'w' : '-',
		(buf.st_mode & S_IXUSR) ? 'x' : '-',
		(buf.st_mode & S_IRGRP) ? 'r' : '-',
		(buf.st_mode & S_IWGRP) ? 'w' : '-',
		(buf.st_mode & S_IXGRP) ? 'x' : '-',
		(buf.st_mode & S_IROTH) ? 'r' : '-',
		(buf.st_mode & S_IWOTH) ? 'w' : '-',
		(buf.st_mode & S_IXOTH) ? 'x' : '-'
	);
	printf("atime: %s", ctime(&buf.st_ctime));
	printf("ctime: %s", ctime(&buf.st_ctime));
	printf("mtime: %s", ctime(&buf.st_mtime));
}

/* Convert arguments to UTF-8 */
#ifdef _MSC_VER
int
wmain(int argc, wchar_t *argv[])
{
	/* Select UTF-8 locale */
	setlocale(LC_ALL, ".utf8");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	/* Allocate memory for multi-byte argv table */
	char **mbargv;
	mbargv = (char**) malloc(argc * sizeof(char*));
	if (!mbargv) {
		puts("Out of memory");
		exit(3);
	}

	/* Convert each argument to UTF-8 */
	for (int i = 0; i < argc; i++) {
		/* Compute the size of corresponding UTF-8 string */
		size_t n;
		wcstombs_s(&n, NULL, 0, argv[i], 0);

		/* Allocate room for UTF-8 string */
		mbargv[i] = (char*) malloc(n + 1);
		if (!mbargv[i]) {
			puts("Out of memory");
			exit(3);
		}

		/* Convert ith argument to UTF-8 */
		wcstombs_s(NULL, mbargv[i], n + 1, argv[i], n);
	}

	/* Pass UTF-8 arguments to the real main program */
	int errorcode = _main(argc, mbargv);

	/* Release UTF-8 arguments */
	for (int i = 0; i < argc; i++) {
		free(mbargv[i]);
	}

	/* Release the multi-byte argv table */
	free(mbargv);
	return errorcode;
}
#else
int
main(int argc, char *argv[])
{
	return _main(argc, argv);
}
#endif
