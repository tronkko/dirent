/*
 * A test program to make sure that dirent works correctly.
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#	include <direct.h>
#	define chdir(x) _chdir(x)
#else
#	include <unistd.h>
#endif
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#undef NDEBUG
#include <assert.h>

static void test_retrieval(void);
static void test_nonexistent(void);
static void test_isfile(void);
static void test_zero(void);
static void test_rewind(void);
static void test_chdir(void);
static void test_filename(void);
static void test_readdir(void);
static void test_wreaddir(void);
static void initialize(void);
static void cleanup(void);

int
main(void)
{
	initialize();

	test_retrieval();
	test_nonexistent();
	test_isfile();
	test_zero();
	test_rewind();
	test_chdir();
	test_filename();
	test_readdir();
	test_wreaddir();

	cleanup();
	return EXIT_SUCCESS;
}

/* Test basic directory retrieval */
static void
test_retrieval(void)
{
	/* Open directory */
	DIR *dir = opendir("tests/1");
	if (dir == NULL) {
		fprintf(stderr, "Directory tests/1 not found\n");
		abort();
	}

	/* Read entries */
	struct dirent *ent;
	int found = 0;
	while ((ent = readdir(dir)) != NULL) {
		/* Check each file */
		if (strcmp(ent->d_name, ".") == 0) {
			/* Directory itself */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(ent->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(ent->d_namlen == 1);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(ent) == 1);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(ent) > 1);
#endif
			found += 1;
		} else if (strcmp(ent->d_name, "..") == 0) {
			/* Parent directory */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(ent->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(ent->d_namlen == 2);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(ent) == 2);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(ent) > 2);
#endif
			found += 2;
		} else if (strcmp(ent->d_name, "file") == 0) {
			/* Regular file */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(ent->d_type == DT_REG);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(ent->d_namlen == 4);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(ent) == 4);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(ent) > 4);
#endif
			found += 4;
		} else if (strcmp(ent->d_name, "dir") == 0) {
			/* Just a directory */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(ent->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(ent->d_namlen == 3);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(ent) == 3);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(ent) > 3);
#endif
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file %s\n", ent->d_name);
			abort();
		}
	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	closedir(dir);
}

/* Function opendir() fails if directory doesn't exist */
static void
test_nonexistent(void)
{
	DIR *dir = opendir("tests/invalid");
	assert(dir == NULL);
	assert(errno == ENOENT);
}

/* Function opendir() fails if pathname is really a file */
static void
test_isfile(void)
{
	DIR *dir = opendir("tests/1/file");
	assert(dir == NULL);
	assert(errno == ENOTDIR);
}

/* Function opendir() fails if pathname is a zero-length string */
static void
test_zero(void)
{
	DIR *dir = opendir("");
	assert(dir == NULL);
	assert(errno == ENOENT);
}

/* Test rewind of directory stream */
static void
test_rewind(void)
{
	/* Open directory */
	DIR *dir = opendir("tests/1");
	assert(dir != NULL);

	/* Read entries */
	int found = 0;
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
		/* Check each file */
		if (strcmp(ent->d_name, ".") == 0) {
			/* Directory itself */
			found += 1;
		} else if (strcmp(ent->d_name, "..") == 0) {
			/* Parent directory */
			found += 2;
		} else if (strcmp(ent->d_name, "file") == 0) {
			/* Regular file */
			found += 4;
		} else if (strcmp(ent->d_name, "dir") == 0) {
			/* Just a directory */
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file %s\n", ent->d_name);
			abort();
		}
	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	/* Rewind stream and read entries again */
	rewinddir(dir);
	found = 0;

	/* Read entries */
	while ((ent = readdir(dir)) != NULL) {
		/* Check each file */
		if (strcmp(ent->d_name, ".") == 0) {
			/* Directory itself */
			found += 1;
		} else if (strcmp(ent->d_name, "..") == 0) {
			/* Parent directory */
			found += 2;
		} else if (strcmp(ent->d_name, "file") == 0) {
			/* Regular file */
			found += 4;
		} else if (strcmp(ent->d_name, "dir") == 0) {
			/* Just a directory */
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file %s\n", ent->d_name);
			abort();
		}
	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	closedir(dir);
}

/* Test rewind with intervening change of working directory */
static void
test_chdir(void)
{
	/* Open directory */
	DIR *dir = opendir("tests/1");
	assert(dir != NULL);

	/* Read entries */
	struct dirent *ent;
	int found = 0;
	while ((ent = readdir(dir)) != NULL) {
		/* Check each file */
		if (strcmp(ent->d_name, ".") == 0) {
			/* Directory itself */
			found += 1;
		} else if (strcmp(ent->d_name, "..") == 0) {
			/* Parent directory */
			found += 2;
		} else if (strcmp(ent->d_name, "file") == 0) {
			/* Regular file */
			found += 4;
		} else if (strcmp(ent->d_name, "dir") == 0) {
			/* Just a directory */
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file %s\n", ent->d_name);
			abort();
		}

	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	/* Change working directory */
	int errorcode = chdir("tests");
	assert(errorcode == 0);

	/* Rewind stream and read entries again */
	rewinddir(dir);
	found = 0;

	/* Read entries */
	while ((ent = readdir(dir)) != NULL) {
		/* Check each file */
		if (strcmp(ent->d_name, ".") == 0) {
			/* Directory itself */
			found += 1;
		} else if (strcmp(ent->d_name, "..") == 0) {
			/* Parent directory */
			found += 2;
		} else if (strcmp(ent->d_name, "file") == 0) {
			/* Regular file */
			found += 4;
		} else if (strcmp(ent->d_name, "dir") == 0) {
			/* Just a directory */
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file %s\n", ent->d_name);
			abort();
		}
	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	/* Restore working directory */
	errorcode = chdir("..");
	assert(errorcode == 0);

	closedir(dir);
}

/* Test long file name */
static void
test_filename(void)
{
	/* Open directory */
	DIR *dir = opendir("tests/2");
	if (dir == NULL) {
		fprintf(stderr, "Directory tests/2 not found\n");
		abort();
	}

	/* Read entries */
	struct dirent *ent;
	int found = 0;
	while ((ent = readdir(dir)) != NULL) {
		/* Check each file */
		if (strcmp(ent->d_name, ".") == 0) {
			/* Directory itself */
			found += 1;
		} else if (strcmp(ent->d_name, "..") == 0) {
			/* Parent directory */
			found += 2;
		} else if (strcmp(ent->d_name, "file.txt") == 0) {
			/* Regular 8+3 filename */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(ent->d_type == DT_REG);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(ent->d_namlen == 8);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(ent) == 8);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(ent) > 8);
#endif
			found += 4;
		} else if (strcmp(ent->d_name, "Testfile-1.2.3.dat") == 0) {
			/* Long file name with multiple dots */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(ent->d_type == DT_REG);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(ent->d_namlen == 18);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(ent) == 18);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(ent) > 18);
#endif
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file %s\n", ent->d_name);
			abort();
		}
	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	closedir(dir);
}

/* Test basic directory retrieval with readdir_r */
static void
test_readdir(void)
{
	/* Open directory */
	DIR *dir = opendir("tests/1");
	if (dir == NULL) {
		fprintf(stderr, "Directory tests/1 not found\n");
		abort();
	}

	/* Read entries to table */
	struct dirent ent[10];
	struct dirent *entry;
	size_t i = 0;
	size_t n = 0;
	while (n < 10 && readdir_r(dir, &ent[n], &entry) == /*OK*/0 && entry != NULL)
		n++;

	/* Make sure that we got all the files from directory */
	assert(n == 4);

	/* Check entries in memory */
	int found = 0;
	for (i = 0; i < n; i++) {
		entry = &ent[i];

		/* Check each file */
		if (strcmp(entry->d_name, ".") == 0) {
			/* Directory itself */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 1);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 1);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 1);
#endif
			found += 1;
		} else if (strcmp(entry->d_name, "..") == 0) {
			/* Parent directory */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 2);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 2);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 2);
#endif
			found += 2;
		} else if (strcmp(entry->d_name, "file") == 0) {
			/* Regular file */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_REG);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 4);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 4);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 4);
#endif
			found += 4;
		} else if (strcmp(entry->d_name, "dir") == 0) {
			/* Just a directory */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 3);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 3);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 3);
#endif
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file %s\n", entry->d_name);
			abort();
		}

	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	closedir(dir);
}

/* Basic directory retrieval with _wreaddir_r */
static void
test_wreaddir(void)
{
#ifdef WIN32
	/* Open directory */
	_WDIR *dir = _wopendir(L"tests/1");
	if (dir == NULL) {
		fprintf(stderr, "Directory tests/1 not found\n");
		abort();
	}

	/* Read entries to table */
	struct _wdirent ent[10];
	struct _wdirent *entry;
	size_t i = 0;
	size_t n = 0;
	while (n < 10 && _wreaddir_r(dir, &ent[n], &entry) == /*OK*/0 && entry != NULL)
		n++;

	/* Make sure that we got all the files from directory */
	assert(n == 4);

	/* Check entries in memory */
	int found = 0;
	for (i = 0; i < n; i++) {
		entry = &ent[i];

		/* Check each file */
		if (wcscmp(entry->d_name, L".") == 0) {
			/* Directory itself */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 1);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 1);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 1);
#endif
			found += 1;
		} else if (wcscmp(entry->d_name, L"..") == 0) {
			/* Parent directory */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 2);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 2);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 2);
#endif
			found += 2;
		} else if (wcscmp(entry->d_name, L"file") == 0) {
			/* Regular file */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_REG);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 4);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 4);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 4);
#endif
			found += 4;
		} else if (wcscmp(entry->d_name, L"dir") == 0) {
			/* Just a directory */
#ifdef _DIRENT_HAVE_D_TYPE
			assert(entry->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
			assert(entry->d_namlen == 3);
#endif
#ifdef _D_EXACT_NAMLEN
			assert(_D_EXACT_NAMLEN(entry) == 3);
#endif
#ifdef _D_ALLOC_NAMLEN
			assert(_D_ALLOC_NAMLEN(entry) > 3);
#endif
			found += 8;
		} else {
			/* Other file */
			fprintf(stderr, "Unexpected file\n");
			abort();
		}
	}

	/* Make sure that all files were found */
	assert(found == 0xf);

	_wclosedir(dir);
#endif
}

static void
initialize(void)
{
	/*NOP*/;
}

static void
cleanup(void)
{
	printf("OK\n");
}
