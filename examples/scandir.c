/*
 * Find files with a pattern.
 *
 * Compile this file with Visual Studio and run the produced command in
 * console with a directory name / pattern argument.  For example, command
 *
 *     scandir "examples/*.c"
 *
 * might output something like
 *
 *     cat.c
 *     dir.c
 *     du.c
 *     find.c
 *     locate.c
 *     ls.c
 *     scandir.c
 *     updatedb.c
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

static int scan(const char *dirname);
static int _scan(const char *dirname, const char *pattern);
static int filter(const struct dirent *entry);
static int match(const char *name, const char *patt);
static int _main(int argc, char *argv[]);

static const char *filter_pattern = NULL;

static int
_main(int argc, char *argv[])
{
	/* For each argument in command line */
	int i = 1;
	while (i < argc) {
		if (!scan(argv[i])) {
			perror("Cannot open directory");
			return EXIT_FAILURE;
		}
		i++;
	}

	/* List current working directory if no arguments on command line */
	if (argc == 1) {
		if (!scan(".")) {
			perror("Cannot open directory");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/*
 * Scan files and directories in a directory.
 */
static int
scan(const char *dirname)
{
	/* Does the argument refer to an existing directory? */
	struct stat statbuf;
	if (stat(dirname, &statbuf) == /*OK*/0) {
		/* Yes, argument is a directory name without pattern */
		return _scan(dirname, "*");
	}

	/* Find the start of pattern */
	const char *pattern = NULL;
	const char *p = dirname;
	while (*p != '\0') {
		if (*p == '\\' || *p == '/' || *p == ':') {
			pattern = p + 1;
		}
		p++;
	}
	if (!pattern) {
		/* No separators so treat the whole name as a pattern */
		return _scan(".", dirname);
	}

	/* Extract directory name without pattern */
	size_t n = pattern - dirname;
	char *dup = malloc(n);
	if (!dup) {
		/* Out of memory */
		return 0;
	}
	strncpy(dup, dirname, n - 1);
	dup[n - 1] = '\0';

	/* Scan directory with directory name and pattern */
	int result = _scan(dup, pattern);

	/* Cleanup */
	free(dup);

	return result;
}

static int
_scan(const char *dirname, const char *pattern)
{
	/* Pass the pattern to filter function */
	filter_pattern = pattern;

	/* Scan files in directory */
	struct dirent **files;
	int n = scandir(dirname, &files, filter, alphasort);
	if (n < 0) {
		/* Cannot open directory */
		return 0;
	}

	/* Loop through file names */
	for (int i = 0; i < n; i++) {
		/* Get pointer to file entry */
		struct dirent *ent = files[i];

		/* Output file name */
		switch (ent->d_type) {
		case DT_REG:
			printf("%s\n", ent->d_name);
			break;

		case DT_DIR:
			printf("%s/\n", ent->d_name);
			break;

		case DT_LNK:
			printf("%s@\n", ent->d_name);
			break;

		default:
			printf("%s*\n", ent->d_name);
		}
	}

	/* Release file names */
	for (int i = 0; i < n; i++) {
		free(files[i]);
	}
	free(files);

	/* Success */
	return 1;
}

/*
 * Compare directory entry to pattern and return 1 if pattern matches the
 * entry.
 */
static int
filter(const struct dirent *entry)
{
	return match(entry->d_name, filter_pattern);
}

/* Compare name to a pattern and return 1 if pattern matches the name */
static int
match(const char *name, const char *patt)
{
	do {
		switch (*patt) {
		case '\0':
			/* Only end of string matches NUL */
			return *name == '\0';

		case '/':
		case '\\':
		case ':':
			/* Invalid pattern */
			return 0;

		case '?':
			/* Any character except NUL matches question mark */
			if (*name == '\0')
				return 0;

			/* Consume character and continue scanning */
			name++;
			patt++;
			break;

		case '*':
			/* Any sequence of characters match asterisk */
			switch (patt[1]) {
			case '\0':
				/* Trailing asterisk matches anything */
				return 1;

			case '*':
			case '?':
			case '/':
			case '\\':
			case ':':
				/* Invalid pattern */
				return 0;

			default:
				/* Find the next matching character */
				while (*name != patt[1]) {
					if (*name == '\0')
						return 0;
					name++;
				}

				/* Terminate sequence on trailing match */
				if (match(name, patt + 1))
					return 1;

				/* No match, continue from next character */
				name++;
			}
			break;

		default:
			/* Only character itself matches */
			if (*patt != *name)
				return 0;

			/* Character passes */
			name++;
			patt++;
		}
	} while (1);
	/*NOTREACHED*/
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
