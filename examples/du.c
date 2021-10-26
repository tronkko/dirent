/*
 * An example demonstrating basic directory listing.
 *
 * Compile this file with Visual Studio and run the produced command in
 * console with a directory name argument.  For example, command
 *
 *     du c:\temp\test
 *
 * might output something like
 *
 *     .\ 1218
 *     ..\ 2020130
 *     t\ 1
 *     test1.1\ 13
 *     c:\temp\test\test1.1.txt 4
 *     test1.2\ 12
 *     c:\temp\test\test1.2.txt 8
 *     c:\temp\test\test2.1.txt.lnk 1180
 *
 * The du command provided by this file is only an example: the command
 * does not have any fancy options like in Linux.
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


static int list_top_level(const char* dirname);
static int search_recurse(const char* dirname);
long size;


int main(int argc, char* argv[])
{
	/* Select default locale */
	setlocale(LC_ALL, "");

	/* For each directory in command line */
	int i = 1;
	while (i < argc) {
		if (!list_top_level(argv[i]))
			exit(EXIT_FAILURE);
		printf("\n");
		i++;
	}

	/* List current working directory if no arguments on command line */
	if (argc == 1)
		list_top_level(".");

	return EXIT_SUCCESS;
}

/*
 * Make initial pass through directory path(s) given at command
 * line, list file sizes for each file found, and call the recursive
 * directory search function for each directory found
 */
static int list_top_level(const char* dirname)
{
	char buffer[PATH_MAX + 2];
	char* p = buffer;
	char* end = &buffer[PATH_MAX];

	/* Copy directory name to buffer */
	const char* src = dirname;
	while (p < end && *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	DIR* dir = opendir(dirname);
	if (!dir) {
		/* Could not open directory */
		fprintf(stderr,
			"Cannot open %s (%s)\n", dirname, strerror(errno));
		return /*failure*/ 0;
	}

	/* Print all files and directories within the directory */
	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL) {
		size = 0;
		char* q = p;
		char c;

		/* Get final character of directory name */
		if (buffer < q)
			c = q[-1];
		else
			c = ':';

		/* Append directory separator if not already there */
		if (c != ':' && c != '/' && c != '\\')
			*q++ = '\\';

		/* Append file name */
		src = ent->d_name;
		while (q < end && *src != '\0') {
			*q++ = *src++;
		}
		*q = '\0';

		/* Decide what to do with the directory entry */
		struct stat stbuf;
		switch (ent->d_type) {
		case DT_LNK:
			/* Output file name with directory */
			printf("%s ", buffer);
			if (stat(buffer, &stbuf) == -1)
				printf("%s %s\n", "Can't access", buffer);
			/* Print the file size */
			else {
				printf("%d\n", stbuf.st_size);
			}
			break;

		case DT_REG:
			/* Output file name with directory */
			printf("%s ", buffer);
			if (stat(buffer, &stbuf) == -1)
				printf("%s %s\n", "Can't access", buffer);
			/* Print the file size */
			else {
				printf("%d\n", stbuf.st_size);
			}
			break;

		case DT_DIR:
			/* Scan sub-directory recursively */
			/* Output directory name */
			
			if (strcmp(ent->d_name, ".") == 0) {
				printf("\\. ");
				search_recurse(buffer);
				printf("%ld\n", size);
			}
			else if (strcmp(ent->d_name, "..") == 0) {
				printf("\\.. ");
				search_recurse(buffer);
				printf("%ld\n", size);
			}
			else {
				printf("%s\\ ", buffer);
				search_recurse(buffer);
				printf("%ld\n", size);
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

/*
 * Search each directory recursively and get a running
 * total of the sizes of everything in each directory
 */
static int search_recurse(const char* dirname)
{
	char buffer[PATH_MAX + 2];
	char* p = buffer;
	char* end = &buffer[PATH_MAX];

	/* Copy directory name to buffer */
	const char* src = dirname;
	while (p < end && *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	DIR* dir = opendir(dirname);
	if (!dir) {
		/* Could not open directory */
		fprintf(stderr,
			"Cannot open %s (%s)\n", dirname, strerror(errno));
		return /*failure*/ 0;
	}

	/* Print all files and directories within the directory */
	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL) {
		char* q = p;
		char c;

		/* Get final character of directory name */
		if (buffer < q)
			c = q[-1];
		else
			c = ':';

		/* Append directory separator if not already there */
		if (c != ':' && c != '/' && c != '\\')
			*q++ = '\\';

		/* Append file name */
		src = ent->d_name;
		while (q < end && *src != '\0') {
			*q++ = *src++;
		}
		*q = '\0';

		/* Decide what to do with the directory entry */
		struct stat stbuf;
		switch (ent->d_type) {
		case DT_LNK:

			if (stat(buffer, &stbuf) == -1)
				printf("%s %s\n", "Can't access", buffer);
			/* Sum the file size */
			else {
				size  += stbuf.st_size;
			}
			break;

		case DT_REG:
			
			if (stat(buffer, &stbuf) == -1)
				printf("%s %s\n", "Can't access", buffer);
			/* Sum the file size */
			else {
				size += stbuf.st_size;
			}
			break;

		case DT_DIR:
			/* Scan sub-directory recursively */
			if (strcmp(ent->d_name, ".") != 0
				&& strcmp(ent->d_name, "..") != 0) {
				search_recurse(buffer);
			}
			break;

		default:
			/* Ignore device entries */
			/*NOP*/;
		}
	}
	closedir(dir);
	return 1;
}
