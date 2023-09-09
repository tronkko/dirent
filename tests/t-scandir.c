/*
 * Make sure that scandir function works OK.
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */

/* Silence warning about fopen being insecure (MS Visual Studio) */
#define _CRT_SECURE_NO_WARNINGS

/* Include prototype for versionsort (Linux) */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#undef NDEBUG
#include <assert.h>

static void test_filter(void);
static void test_sort(void);
static void test_custom(void);
static void test_enoent(void);
static void test_enotdir(void);
static void test_versionsort(void);
static void test_large(void);
static void test_match(void);
static void test_null(void);
static int only_readme(const struct dirent *entry);
static int no_directories(const struct dirent *entry);
static int reverse_alpha(const struct dirent **a, const struct dirent **b);
static int match(const char *name, const char *patt);
static void initialize(void);
static void cleanup(void);

int
main(void)
{
	initialize();

	test_filter();
	test_sort();
	test_custom();
	test_enoent();
	test_enotdir();
	test_versionsort();
	test_large();
	test_match();
	test_null();

	cleanup();
	return EXIT_SUCCESS;
}

static void
test_filter(void)
{
	/* Read directory entries with filter */
	struct dirent **files;
	int n = scandir("tests/3", &files, only_readme, alphasort);
	assert(n == 1);

	/* Make sure that the filter works */
	assert(strcmp(files[0]->d_name, "README.txt") == 0);

	/* Release file names */
	for (int i = 0; i < n; i++) {
		free(files[i]);
	}
	free(files);
}

static void
test_sort(void)
{
	/* Read directory entries in alphabetic order */
	struct dirent **files;
	int n = scandir("tests/3", &files, NULL, alphasort);
	assert(n == 13);

	/* Make sure that we got all the names in the proper order */
	assert(strcmp(files[0]->d_name, ".") == 0);
	assert(strcmp(files[1]->d_name, "..") == 0);
	assert(strcmp(files[2]->d_name, "3zero.dat") == 0);
	assert(strcmp(files[3]->d_name, "666.dat") == 0);
	assert(strcmp(files[4]->d_name, "Qwerty-my-aunt.dat") == 0);
	assert(strcmp(files[5]->d_name, "README.txt") == 0);
	assert(strcmp(files[6]->d_name, "aaa.dat") == 0);
	assert(strcmp(files[7]->d_name, "dirent.dat") == 0);
	assert(strcmp(files[8]->d_name, "empty.dat") == 0);
	assert(strcmp(files[9]->d_name, "sane-1.12.0.dat") == 0);
	assert(strcmp(files[10]->d_name, "sane-1.2.30.dat") == 0);
	assert(strcmp(files[11]->d_name, "sane-1.2.4.dat") == 0);
	assert(strcmp(files[12]->d_name, "zebra.dat") == 0);

	/* Release file names */
	for (int i = 0; i < n; i++) {
		free(files[i]);
	}
	free(files);
}

static void
test_custom(void)
{
	/* Read directory entries in alphabetic order */
	struct dirent **files;
	int n = scandir("tests/3", &files, no_directories, reverse_alpha);
	assert(n == 11);

	/* Make sure that we got file names in the reverse order */
	assert(strcmp(files[0]->d_name, "zebra.dat") == 0);
	assert(strcmp(files[1]->d_name, "sane-1.2.4.dat") == 0);
	assert(strcmp(files[2]->d_name, "sane-1.2.30.dat") == 0);
	assert(strcmp(files[3]->d_name, "sane-1.12.0.dat") == 0);
	assert(strcmp(files[4]->d_name, "empty.dat") == 0);
	assert(strcmp(files[5]->d_name, "dirent.dat") == 0);
	assert(strcmp(files[6]->d_name, "aaa.dat") == 0);
	assert(strcmp(files[7]->d_name, "README.txt") == 0);
	assert(strcmp(files[8]->d_name, "Qwerty-my-aunt.dat") == 0);
	assert(strcmp(files[9]->d_name, "666.dat") == 0);
	assert(strcmp(files[10]->d_name, "3zero.dat") == 0);

	/* Release file names */
	for (int i = 0; i < n; i++) {
		free(files[i]);
	}
	free(files);
}

static void
test_enoent(void)
{
	/* Trying to open non-existing file produces an error */
	struct dirent **files = NULL;
	int n = scandir("tests/invalid", &files, NULL, alphasort);
	assert(n == -1);
	assert(files == NULL);
	assert(errno == ENOENT);
}

static void
test_enotdir(void)
{
	/* Trying to open file as a directory produces ENOTDIR error */
	struct dirent **files = NULL;
	int n = scandir("tests/3/666.dat", &files, NULL, alphasort);
	assert(n == -1);
	assert(files == NULL);
	assert(errno == ENOTDIR);
}

static void
test_versionsort(void)
{
	/* Sort files using versionsort() */
	struct dirent **files = NULL;
	int n = scandir("tests/3", &files, no_directories, versionsort);
	assert(n == 11);

	/*
	 * Make sure that we got all the file names in the proper order:
	 * 1.2.4 < 1.2.30 < 1.12.0
	 */
	assert(strcmp(files[0]->d_name, "3zero.dat") == 0);
	assert(strcmp(files[1]->d_name, "666.dat") == 0);
	assert(strcmp(files[2]->d_name, "Qwerty-my-aunt.dat") == 0);
	assert(strcmp(files[3]->d_name, "README.txt") == 0);
	assert(strcmp(files[4]->d_name, "aaa.dat") == 0);
	assert(strcmp(files[5]->d_name, "dirent.dat") == 0);
	assert(strcmp(files[6]->d_name, "empty.dat") == 0);
	assert(strcmp(files[7]->d_name, "sane-1.2.4.dat") == 0);
	assert(strcmp(files[8]->d_name, "sane-1.2.30.dat") == 0);
	assert(strcmp(files[9]->d_name, "sane-1.12.0.dat") == 0);
	assert(strcmp(files[10]->d_name, "zebra.dat") == 0);

	/* Release file names */
	for (int i = 0; i < n; i++) {
		free(files[i]);
	}
	free(files);
}

static void
test_large(void)
{
	char dirname[PATH_MAX+1];
	int i;
	int ok;

	/* Copy name of temporary directory to variable dirname */
#ifdef WIN32
	i = GetTempPathA(PATH_MAX, dirname);
	assert(i > 0);
#else
	strcpy(dirname, "/tmp/");
	i = strlen(dirname);
#endif

	/* Append random characters to dirname */
	for (size_t j = 0; j < 10; j++) {
		char c;

		/* Generate random character */
		c = "abcdefghijklmnopqrstuvwxyz"[rand() % 26];

		/* Append character to dirname */
		assert(i < PATH_MAX);
		dirname[i++] = c;
	}

	/* Terminate directory name */
	assert(i < PATH_MAX);
	dirname[i] = '\0';

	/* Create directory */
#ifdef WIN32
	ok = CreateDirectoryA(dirname, NULL);
	assert(ok);
#else
	ok = mkdir(dirname, 0700);
	assert(ok == /*success*/0);
#endif

	/* Create one thousand files */
	assert(i + 5 < PATH_MAX);
	for (int j = 0; j < 1000; j++) {
		FILE *fp;

		/* Construct file name */
		dirname[i] = '/';
		dirname[i+1] = 'z';
		dirname[i+2] = '0' + ((j / 100) % 10);
		dirname[i+3] = '0' + ((j / 10) % 10);
		dirname[i+4] = '0' + (j % 10);
		dirname[i+5] = '\0';

		/* Create file */
		fp = fopen(dirname, "w");
		assert(fp != NULL);
		fclose(fp);

	}

	/* Cut out the file name part */
	dirname[i] = '\0';

	/* Scan directory */
	struct dirent **files;
	int n = scandir(dirname, &files, no_directories, alphasort);
	assert(n == 1000);

	/* Make sure that all 1000 files are read back */
	for (int j = 0; j < n; j++) {
		char match[100];

		/* Construct file name */
		match[0] = 'z';
		match[1] = '0' + ((j / 100) % 10);
		match[2] = '0' + ((j / 10) % 10);
		match[3] = '0' + (j % 10);
		match[4] = '\0';

		/* Make sure that file name matches that on the disk */
		assert(strcmp(files[j]->d_name, match) == 0);
	}

	/* Release file names */
	for (int j = 0; j < n; j++) {
		free(files[j]);
	}
	free(files);
}

static void
test_match(void)
{
	/* Empty string matches another empty string */
	assert(match("", "") == 1);

	/* Characters match identical characters */
	assert(match("abc", "abc") == 1);

	/* Question mark matches any character */
	assert(match("abc", "a?c") == 1);
	assert(match("abc", "?bc") == 1);
	assert(match("abc", "ab?") == 1);

	/* Question mark does not match NUL character */
	assert(match("abc", "abc?") == 0);

	/* Trailing asterisk matches anything, even empty string */
	assert(match("", "*") == 1);
	assert(match("abc", "*") == 1);
	assert(match("abc", "a*") == 1);
	assert(match("abc", "ab*") == 1);
	assert(match("abc", "abc*") == 1);

	/* Asterisk matches a sequence of zero or more characters */
	assert(match("ab", "a*b") == 1);
	assert(match("axb", "a*b") == 1);
	assert(match("axxb", "a*b") == 1);
	assert(match("axxxxxxb", "a*b") == 1);

	/* Asterisk matches the longest string */
	assert(match("ad", "a*d") == 1);
	assert(match("abcd", "a*d") == 1);
	assert(match("abcddddddddddd", "a*d") == 1);

	/* Multiple asterisks in a string are supported */
	assert(match("a(abcdabcd)bx(abcdabcdabcd)cx(abcdabcdabcd)dx", "a*bx*cx*dx") == 1);

	/* Suffix match */
	assert(match(".csv", "*.csv") == 1);
	assert(match("asta.csv", "*.csv") == 1);
	assert(match("asta.csv.kosta.rento.dat.csv", "*.csv") == 1);

	/* Match returns zero if no match is found */
	assert(match("abc", "ab") == 0);
	assert(match("abc", "abC") == 0);
	assert(match("ad", "*x") == 0);
	assert(match("baaaa", "a*") == 0);
	assert(match("asta.csv.kosta.rento.dat", "*.csv") == 0);

	/* Invalid patterns wont match anything */
	assert(match("/", "/") == 0);
	assert(match(":", ":") == 0);
	assert(match("\\", "\\") == 0);
	assert(match("abb", "a**") == 0);
	assert(match("abb", "a*?") == 0);
}

static void
test_null(void)
{
	/* Scandir can be used with null filter and compare functions */
	struct dirent **files = NULL;
	int n = scandir("tests/3", &files, NULL, NULL);
	assert(n == 13);

	/* Release file names */
	for (int i = 0; i < n; i++) {
		free(files[i]);
	}
	free(files);
}

/* Only pass README.txt file */
static int
only_readme(const struct dirent *entry)
{
	return strcmp(entry->d_name, "README.txt") == 0;
}

/* Only pass regular files */
static int
no_directories(const struct dirent *entry)
{
	return entry->d_type != DT_DIR;
}

/* Sort in reverse direction */
static int
reverse_alpha(const struct dirent **a, const struct dirent **b)
{
	return strcoll((*b)->d_name, (*a)->d_name);
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

static void
initialize(void)
{
	/* Initialize random number generator */
	srand((unsigned) time(NULL));
}

static void
cleanup(void)
{
	printf("OK\n");
}
