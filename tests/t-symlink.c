/*
 * A test program to make sure that dirent can identify symlinks.
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */

/* Silence warning about strcmp being insecure (MS Visual Studio) */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _MSC_VER
#	include <direct.h>
#	include <io.h>
#	define mkdir(path, mode) _mkdir(path)
#	define stat _stat64
#	define fstat _fstat64
#	define open _open
#	define close _close
#else
#	include <unistd.h>
#endif

#ifdef WIN32
#	define SEP "\\"
#else
#	define SEP "/"
#endif

#undef NDEBUG
#include <assert.h>

static void test_stat(void);
static void test_dirent(void);
static int is_link(const char *fn);
static int is_file(const char *fn);
static int is_directory(const char *fn);
static void initialize(void);
static void cleanup(void);

static char tmpdir[PATH_MAX+1];
static char subdir[PATH_MAX + 1];
static char file[PATH_MAX + 1];
static char link1[PATH_MAX + 1];
static char link2[PATH_MAX + 1];

int
main(void)
{
	initialize();

	test_stat();
	test_dirent();

	cleanup();
	return EXIT_SUCCESS;
}

static void
test_stat(void)
{
	assert(is_file(file));
	assert(!is_directory(file));
	assert(!is_link(file));

	assert(!is_file(subdir));
	assert(is_directory(subdir));
	assert(!is_link(subdir));

	assert(is_file(link1));
	assert(!is_directory(link1));
	assert(is_link(link1));

	assert(!is_file(link2));
	assert(is_directory(link2));
	assert(is_link(link2));
}

static void
test_dirent(void)
{
#ifdef _DIRENT_HAVE_D_TYPE
	/* Open directory */
	DIR *dir = opendir(tmpdir);
	if (dir == NULL) {
		fprintf(stderr, "Directory %s not found\n", tmpdir);
		abort();
	}

	/* Read entries in directory */
	struct dirent *ent;
	int found = 0;
	while ((ent = readdir(dir)) != NULL) {
		/* Check each file */
		if (strcmp(ent->d_name, ".") == 0) {
			assert(ent->d_type == DT_DIR);
			found += 1;
		} else if (strcmp(ent->d_name, "..") == 0) {
			assert(ent->d_type == DT_DIR);
			found += 2;
		} else if (strcmp(ent->d_name, "file") == 0) {
			assert(ent->d_type == DT_REG);
			found += 4;
		} else if (strcmp(ent->d_name, "dir") == 0) {
			assert(ent->d_type == DT_DIR);
			found += 8;
		} else if (strcmp(ent->d_name, "link1") == 0) {
			assert(ent->d_type == DT_LNK);
			found += 16;
		} else if (strcmp(ent->d_name, "link2") == 0) {
			assert(ent->d_type == DT_LNK);
			found += 32;
		} else {
			fprintf(stderr, "Unexpected file %s\n", ent->d_name);
			abort();
		}
	}

	/* Make sure that all files were found */
	assert(found == 1 + 2 + 4 + 8 + 16 + 32);

	closedir(dir);
#else
	fprintf(stderr, "Skipped test_dirent\n");
#endif
}

/* Returns true if file name is a symbolic link */
static int
is_link(const char *fn)
{
#ifdef WIN32
	/* In Windows, function lstat does not exist */
	DWORD attr = GetFileAttributesA(fn);
	if (attr == INVALID_FILE_ATTRIBUTES) {
		return 0;
	}
	return (attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
#else
	/* In Linux, function lstat returns information about the link */
	struct stat buf;
	if (lstat(fn, &buf) != /*OK*/0) {
		return 0;
	}
	return S_ISLNK(buf.st_mode);
#endif
}

/* Returns true if file name refers to a regular file */
static int
is_file(const char *fn)
{
	struct stat buf;

	if (stat(fn, &buf) != /*OK*/0) {
		return 0;
	}

	return S_ISREG(buf.st_mode);
}

/* Returns true if file name refers to a directory */
static int
is_directory(const char *fn)
{
	struct stat buf;

	if (stat(fn, &buf) != /*OK*/0) {
		return 0;
	}

	return S_ISDIR(buf.st_mode);
}

static void
initialize(void)
{
	size_t i;

	/*
	 * Select UTF-8 locale.  This will change the way how C runtime
	 * functions such as fopen() and mkdir() handle character strings.
	 * For more information, please see:
	 * https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-160#utf-8-support
	 */
	setlocale(LC_ALL, "LC_CTYPE=.utf8");

	/* Initialize random number generator from time */
	srand(((int) time(NULL)) * 257 + 5555);

	/* Get system temporary directory */
#ifdef WIN32
	i = GetTempPathA(PATH_MAX, tmpdir);
	assert(i > 0);
	assert(tmpdir[i - 1] == '\\');
#else
	strcpy(tmpdir, "/tmp/");
	i = strlen(tmpdir);
#endif

	/* Append random directory name to path name */
	for (size_t k = 0; k < 10; k++) {
		/* Generate random character */
		char c = "abcdefghijklmnopqrstuvwxyz"[rand() % 26];

		/* Append character to path */
		assert(i < PATH_MAX);
		tmpdir[i++] = c;
	}

	/* Zero-terminate the directory name */
	assert(i < PATH_MAX);
	tmpdir[i] = '\0';

	/* Create directory for test files */
	if (mkdir(tmpdir, S_IRWXU) != /*OK*/0) {
		fprintf(stderr, "Cannot create directory %s\n", tmpdir);
		abort();
	}
	printf("Created directory %s\n", tmpdir);

	/* Create subdirectory */
	strcpy(subdir, tmpdir);
	strcat(subdir, SEP "dir");
	if (mkdir(subdir, S_IRWXU) != /*OK*/0) {
		fprintf(stderr, "Cannot create directory %s\n", subdir);
		abort();
	}

	/* Create regular file */
	strcpy(file, tmpdir);
	strcat(file, SEP "file");
	FILE *fp = fopen(file, "w");
	if (!fp) {
		fprintf(stderr, "Cannot create file %s\n", file);
		abort();
	}
	fprintf(fp, "Hello world!\n");
	fclose(fp);

	/* Create symbolic link */
	strcpy(link1, tmpdir);
	strcat(link1, "/link1");
#ifdef WIN32
	DWORD flags1 = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
	if (!CreateSymbolicLinkA(link1, file, flags1)) {
		/* Developer mode not activated? */
		fprintf(stderr, "Create file symlink error: %d\n", GetLastError());
		fprintf(stderr, "Skipped\n");
		exit(/*Skip*/ 77);
	}
#else
	if (symlink(file, link1) != /*OK*/0) {
		fprintf(stderr, "Cannot create link %s->%s\n", file, link1);
		abort();
	}
#endif

	/* Create symbolic link to directory */
	strcpy(link2, tmpdir);
	strcat(link2, "/link2");
#ifdef WIN32
	DWORD flags2 = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
		| SYMBOLIC_LINK_FLAG_DIRECTORY;
	if (!CreateSymbolicLinkA(link2, subdir, flags2)) {
		fprintf(stderr, "Create file symlink error: %d\n", GetLastError());
		fprintf(stderr, "Skipped\n");
		exit(/*Skip*/ 77);
	}
#else
	if (symlink(subdir, link2) != /*OK*/0) {
		fprintf(stderr, "Cannot create link %s->%s\n", subdir, link2);
		abort();
	}
#endif
}

static void
cleanup(void)
{
	printf("OK\n");
}
