/*
 * List files and directories recursively.
 *
 * Compile this file with Visual Studio and run the produced command in
 * console with a directory name argument.  For example, command
 *
 *     find "C:\Program Files"
 *
 * will output thousands of file names such as
 *
 *     c:\Program Files/7-Zip/7-zip.chm
 *     c:\Program Files/7-Zip/7-zip.dll
 *     c:\Program Files/7-Zip/7z.dll
 *     c:\Program Files/Adobe/Reader 10.0/Reader/logsession.dll
 *     c:\Program Files/Adobe/Reader 10.0/Reader/LogTransport2.exe
 *     c:\Program Files/Windows NT/Accessories/wordpad.exe
 *     c:\Program Files/Windows NT/Accessories/write.wpc
 *
 * The find command provided by this file is only an example: the command does
 * not provide options to restrict the output to certain files as the Linux
 * version does.
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

static int find_directory(const char *dirname);
static int _main(int argc, char *argv[]);

int
_main(int argc, char *argv[])
{
	/* For each directory in command line */
	int i = 1;
	while (i < argc) {
		if (!find_directory(argv[i]))
			exit(EXIT_FAILURE);
		i++;
	}

	/* List current working directory if no arguments on command line */
	if (argc == 1)
		find_directory(".");

	return EXIT_SUCCESS;
}

/* Find files and subdirectories recursively */
static int
find_directory(const char *dirname)
{
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	char *end = &buffer[PATH_MAX];

	/* Copy directory name to buffer */
	const char *src = dirname;
	while (p < end && *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	DIR *dir = opendir(dirname);
	if (!dir) {
		/* Could not open directory */
		fprintf(stderr,
			"Cannot open %s (%s)\n", dirname, strerror(errno));
		return /*failure*/ 0;
	}

	/* Print all files and directories within the directory */
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
		char *q = p;
		char c;

		/* Get final character of directory name */
		if (buffer < q)
			c = q[-1];
		else
			c = ':';

		/* Append directory separator if not already there */
		if (c != ':' && c != '/' && c != '\\')
			*q++ = '/';

		/* Append file name */
		src = ent->d_name;
		while (q < end && *src != '\0') {
			*q++ = *src++;
		}
		*q = '\0';

		/* Decide what to do with the directory entry */
		switch (ent->d_type) {
		case DT_LNK:
		case DT_REG:
			/* Output file name with directory */
			printf("%s\n", buffer);
			break;

		case DT_DIR:
			/* Scan sub-directory recursively */
			if (strcmp(ent->d_name, ".") != 0
				&&  strcmp(ent->d_name, "..") != 0) {
				find_directory(buffer);
			}
			break;

		default:
			/* Ignore device entries */
			/*NOP*/;
		}

	}

	closedir(dir);
	return /*success*/ 1;
}

/* Stub for converting arguments to UTF-8 on Windows */
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

	/* Convert each argument in argv to UTF-8 */
	for (int i = 0; i < argc; i++) {
		size_t n;
		wcstombs_s(&n, NULL, 0, argv[i], 0);

		/* Allocate room for ith argument */
		mbargv[i] = (char*) malloc(n + 1);
		if (!mbargv[i]) {
			puts("Out of memory");
			exit(3);
		}

		/* Convert ith argument to utf-8 */
		wcstombs_s(NULL, mbargv[i], n + 1, argv[i], n);
	}

	/* Pass UTF-8 converted arguments to the main program */
	int errorcode = _main(argc, mbargv);

	/* Release UTF-8 arguments */
	for (int i = 0; i < argc; i++) {
		free(mbargv[i]);
	}

	/* Release the argument table */
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
