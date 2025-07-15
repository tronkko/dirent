/*
 * Test program to make sure that dirent compiles cleanly with winsock.
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */
#include <dirent.h>
#ifdef WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#undef NDEBUG
#include <assert.h>

static void test_properties(void);
static void test_length(void);
static void test_defines(void);
static void test_dttoif(void);
static void test_iftodt(void);
static void initialize(void);
static void cleanup(void);

int
main(void)
{
	initialize();

	test_properties();
	test_length();
	test_defines();
	test_dttoif();
	test_iftodt();

	cleanup();
	return EXIT_SUCCESS;
}

static void
test_properties(void)
{
#ifdef _DIRENT_HAVE_D_TYPE
	printf("Has d_type\n");
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
	printf("Has d_namlen\n");
#endif
#ifdef _D_EXACT_NAMLEN
	printf("Has _D_EXACT_NAMLEN\n");
#endif
#ifdef _D_ALLOC_NAMLEN
	printf("Has _D_ALLOC_NAMLEN\n");
#endif
}

static void
test_length(void)
{
	struct dirent *dirp = NULL;
	printf("Length of d_name with terminator: %d\n",
		(int) sizeof(dirp->d_name));
}

static void
test_defines(void)
{
	assert(DT_UNKNOWN != DT_FIFO);
	assert(DT_UNKNOWN != DT_CHR);
	assert(DT_UNKNOWN != DT_BLK);
	assert(DT_UNKNOWN != DT_REG);
	assert(DT_UNKNOWN != DT_LNK);
	assert(DT_UNKNOWN != DT_SOCK);
	assert(DT_UNKNOWN != DT_WHT);

	assert(DT_FIFO != DT_CHR);
	assert(DT_FIFO != DT_BLK);
	assert(DT_FIFO != DT_REG);
	assert(DT_FIFO != DT_LNK);
	assert(DT_FIFO != DT_SOCK);
	assert(DT_FIFO != DT_WHT);

	assert(DT_CHR != DT_BLK);
	assert(DT_CHR != DT_REG);
	assert(DT_CHR != DT_LNK);
	assert(DT_CHR != DT_SOCK);
	assert(DT_CHR != DT_WHT);

	assert(DT_BLK != DT_REG);
	assert(DT_BLK != DT_LNK);
	assert(DT_BLK != DT_SOCK);
	assert(DT_BLK != DT_WHT);

	assert(DT_REG != DT_LNK);
	assert(DT_REG != DT_SOCK);
	assert(DT_REG != DT_WHT);

	assert(DT_LNK != DT_SOCK);
	assert(DT_LNK != DT_WHT);

	assert(DT_SOCK != DT_WHT);
}

static void
test_dttoif(void)
{
	assert(DTTOIF(DT_REG) == S_IFREG);
	assert(DTTOIF(DT_DIR) == S_IFDIR);
	assert(DTTOIF(DT_FIFO) == S_IFIFO);
	assert(DTTOIF(DT_SOCK) == S_IFSOCK);
	assert(DTTOIF(DT_CHR) == S_IFCHR);
	assert(DTTOIF(DT_BLK) == S_IFBLK);
	assert(DTTOIF(DT_LNK) == S_IFLNK);
}

static void
test_iftodt(void)
{
	int perm = S_IRWXU | S_IRWXG | S_IRWXO;

	assert(IFTODT(S_IFREG | perm) == DT_REG);
	assert(IFTODT(S_IFDIR | perm) == DT_DIR);
	assert(IFTODT(S_IFIFO | perm) == DT_FIFO);
	assert(IFTODT(S_IFSOCK | perm) == DT_SOCK);
	assert(IFTODT(S_IFCHR | perm) == DT_CHR);
	assert(IFTODT(S_IFBLK | perm) == DT_BLK);
	assert(IFTODT(S_IFLNK | perm) == DT_LNK);
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
