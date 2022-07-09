/*
 * An example demonstrating recursive directory listing.
 *
 * Compile this file with Visual Studio and run the produced command in
 * console with a directory name argument.  For example, command
 *
 *     du c:\temp\test
 *
 * might output something like
 *
 *     8383	c:\temp\test/budget.xlsx
 *     11972	c:\temp\test/essay.docx
 *     6188	c:\temp\test/test2.1/
 *     8328	c:\temp\test/test2.2/
 *     22	c:\temp\test/to_do_list.txt
 *     34893	c:\temp\test/
 *
 * The du command provided by this file is only an example: the command
 * does not have any fancy options like in Linux.
 *
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


static int list_directory(const char *dirname, const char *sub_dirname,
	double *total_size, double *sub_dir_size, int dir_count);


int main(int argc, char *argv[])
{
	printf("\n");

	/* Select default locale */
	setlocale(LC_ALL, "");

	/* Keep track of where we are at */
	int dir_count = 0;

	/*
	 * Keep track of the size of everything in the directory or directories
	 * given in the command line arguments
	 */
	double t = 0;
	double *total_size = &t;

	/*
	 * Keep track of the size of everything in each subdirectory of the
	 * director or directories given in the command line arguments
	 */
	double s = 0;
	double *sub_dir_size = &s;
	
	/* For each directory in command line */
	int i = 1;
	while (i < argc) {
		if (!list_directory(argv[i], argv[i], total_size, sub_dir_size,
			dir_count))
			exit(EXIT_FAILURE);

		/*
		 * Before going to the next directory argument (if any), print the
		 * directory name and the total size at the end of the list
		 */
		printf("%15.0f\t\t%s/\n\n", *total_size, argv[i]);

		/*
		 * Reset the top directory and subdirectory total sizes in preparation
		 * for the next directory argument
		 */
		*sub_dir_size = 0;
		*total_size = 0;

		i++;
	}

	/* List current working directory if no arguments on command line */
	if (argc == 1)
		list_directory(".", ".", total_size, sub_dir_size, dir_count);

	return EXIT_SUCCESS;
}

/* Find files and subdirectories recursively; list their sizes */
static int list_directory(const char *dirname, const char *sub_dirname,
	double *total_size, double *sub_dir_size, int dir_count)
{
	dir_count = 0;
	struct stat stbuf;
	struct dirent *ent;

	char buffer[PATH_MAX + 2];
	char *p = buffer;
	char *end = &buffer[PATH_MAX];
	
	/* Copy directory name to buffer */
	const char *src = sub_dirname;

	while (p < end && *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	DIR *dir = opendir(sub_dirname);
	if (!dir) {
		/* Could not open directory */
		fprintf(stderr,
			"\t\t\tCannot open %s (%s)\n", sub_dirname, strerror(errno));

		/* Failure */
		return 0;
	}

	/* Print all files and directories within the directory */
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

				if (stat(buffer, &stbuf) == -1) {
					printf("\t\t\t%s %s\n", "Can't access", buffer);
				}
				else {
					/*
					 * If we are in the top directory and not in a subdirectory.
					 * We don't want to list every file and directory, just the files
					 * and directories inside our top directory
					 */
					if (strcmp(dirname, sub_dirname) == 0) {
						/* Output file name and file size with directory */
						printf("%15d\t\t%s\n", stbuf.st_size, buffer);
					}
					*sub_dir_size += stbuf.st_size;
					*total_size += stbuf.st_size;
					dir_count = 0;
				}

				break;

			case DT_DIR:
				/* Scan sub-directory recursively */
				if (strcmp(ent->d_name, ".") != 0
					&&  strcmp(ent->d_name, "..") != 0) {
					dir_count++;

					/*
		 			 * If we are back in the top directory and not in a subdirectory
		 			 * We need to reset the subdirectory size totals
		  			 */
					if (strcmp(dirname, sub_dirname) == 0) {
						*sub_dir_size = 0;
					}

					list_directory(dirname, buffer, total_size, sub_dir_size,
						dir_count);
				}
				break;

			default:
				/* Ignore device entries */
				/* NOP */;
		}

		/*
		 * If we are in the top directory and not in a subdirectory.
		 * We don't want to list every file and folder, just the files
		 * and directories inside our top directory
		 */
		if (strcmp(dirname, sub_dirname) == 0 && dir_count > 0) {
			/* Output file name and file size with directory */
			printf("%15.0f\t\t%s/\n", *sub_dir_size, buffer);
	
			/* Reset the running total for our sub directory size */
			*sub_dir_size = 0;
		}
	}

	closedir(dir);

	/* Success */
	return 1;
}
