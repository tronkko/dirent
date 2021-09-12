
/*
 * An example demonstrating basic directory listing.
 *
 * Compile this file with Visual Studio and run the produced command in
 * console with a directory name argument.  For example, command
 *
 *     ls "c:\Program Files"
 *
 * might output something like
 *
 *     ./
 *     ../
 *     7-Zip/
 *     Internet Explorer/
 *     Microsoft Visual Studio 9.0/
 *     Microsoft.NET/
 *     Mozilla Firefox/
 *
 * The ls command provided by this file is only an example: the command has
 * one option, "-s", which lists files, files sizes, and directories.
 * Recursive directory searching is not yet available with the "-s" option.
 * 
 * It does not have any fancy options like "ls -al" in Linux, and the command
 * does not support file name matching like "ls *.c".
 *
 * Copyright (C) 2006-2012 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>


static void list_directory (const char *dirname);
static void list_size(const char *dirname);
static int count_s(const int count, const char* arg_list[]);
static void get_size(const char* full_path);
static void get_full_path(const char* dirname, const char* filename,
    char* path_pointer);


int main(int argc, char *argv[]) 
{
    /* Select default locale */
    setlocale(LC_ALL, "");

    /* Check for option -s at command line */
    int s_count = count_s(argc, argv);

    /* 
     * Option "-s" is not found on command line; list files
     * and directories without sizes
     */
    if (s_count == 0) {
        
        /* For each directory in command line */
        int i = 1;
        while (i < argc) {
            list_directory(argv[i]);
            i++;
        }
        /* List current working directory if no arguments on command line */
        if (argc == 1)
            list_directory(".");
    }
    /* 
     * Option -s is found on command line; list files with sizes, and list
     * directories without sizes.
     */
    else {
        /* 
         * List current working directory if "-s" is the only argument
         */
        if (argc == 2)
            list_size(".");
        /*
         * List current working directory if "-s" is found multiple times
         * with no directories, for example, ls -s -s -s.
         */
        else if (s_count == argc - 1) {
            list_size(".");
        }
        else {
            /* For each directory in command line. */
            int j = 1;
            while (j < argc) {
                /* Skip "-s" so the option is not treated like a directory */
                if (strcmp(argv[j], "-s") == 0)
                    j++;
                else {
                    list_size(argv[j]);
                    j++;
                }
            }
        }
    }
    
    return EXIT_SUCCESS;
}

/*
 * List files and directories within a directory.
 */
static void list_directory(const char *dirname)
{
    /* Open directory stream */
    DIR *dir = opendir(dirname);
    if (!dir) {
        /* Could not open directory */
        fprintf(stderr,
            "Cannot open %s (%s)\n", dirname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct dirent *ent;
    if (dir != NULL) {

        /* Print all files and directories within the directory */
        while ((ent = readdir (dir)) != NULL) {
            switch (ent->d_type) {
            case DT_REG:
                printf ("%s\n", ent->d_name);
                break;

            case DT_DIR:
                printf ("%s/\n", ent->d_name);
                break;

            case DT_LNK:
                printf ("%s@\n", ent->d_name);
                break;

            default:
                printf ("%s*\n", ent->d_name);
            }
        }

        closedir (dir);

    } else {
        /* Could not open directory */
        printf ("Cannot open directory %s\n", dirname);
        exit (EXIT_FAILURE);
    }
}

 /*
  * List files, directories, and file sizes within a directory.
  */
static void list_size(const char* dirname)
{
    /* Open directory stream */
    DIR* dir = opendir(dirname);
    if (!dir) {
        /* Could not open directory */
        fprintf(stderr,
            "Cannot open %s (%s)\n", dirname, strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct dirent* ent;
    if (dir != NULL) {
        char path[1024];
        /* Print all files, file sizes, and directories within the directory */
        while ((ent = readdir(dir)) != NULL) {
            switch (ent->d_type) {
            case DT_REG:
                printf("%s ", ent->d_name);
		/* Get the full path to the file */
                get_full_path(dirname, ent->d_name, path);
		/* Print the file size next to the file name */
                get_size(path);
                break;

            case DT_DIR:
                printf("%s/ %s\n", ent->d_name, "TODO: Add recursive function"
                    " to get directory size.");
                break;

            case DT_LNK:
                printf("%s@ ", ent->d_name);
		/* Get the full path to the file */
                get_full_path(dirname, ent->d_name, path);
		/* Print the file size next to the file name */
                get_size(path);
                break;

            default:
                printf("%s* ", ent->d_name);
		/* Get the full path to the file */
                get_full_path(dirname, ent->d_name, path);
		/* Print the file size next to the file name */
                get_size(path);
                break;
            }
        }

        closedir(dir);

    }
    else {
        /* Could not open directory */
        printf("Cannot open directory %s\n", dirname);
        exit(EXIT_FAILURE);
    }
}

/*
 * Count if option -s is repeated on commandl ine
 */
static int count_s(const int count, const char* arg_list[])
{
    int i = 1;
    int s_count = 0;
    while (i < count) {
        if (strcmp(arg_list[i], "-s") == 0)
            s_count++;
        i++;
    }
    return s_count;
}

/*
 * List file size.
 */
static void get_size(const char* full_path)
{
    struct stat stbuf;

    if (stat(full_path, &stbuf) == -1)
        printf("%s %s\n", "Can't access", full_path);
    else
        printf("%d\n", stbuf.st_size);
}

/*
 * Combine directory and file name; return full path to file.
 * Add backslash '\' if not added in command line by user.
 */
static void get_full_path(
    const char* dirname, const char* filename, char* path_pointer)
{
    /* If '\' is included in command line argument at end of file path */
    if (dirname[strlen(dirname) - 1] == '\\') {
        /* Put the path and filename together to create the full path */
        size_t total = strlen(dirname) + strlen(filename);
        int j;
        for (j = 0; j < strlen(dirname); j++) {
            path_pointer[j] = dirname[j];
        }

        int i = 0;
        for (i = 0; i < strlen(filename); i++) {
            path_pointer[j] = filename[i];
            j++;
        }
        path_pointer[j] = '\0';
    }
    /* Add '\' to end of file path if not included in command line argument */
    else {
        /* Put the path and filename together to create the full path */
        size_t total = strlen(dirname) + strlen(filename);
        int j;
        for (j = 0; j < strlen(dirname); j++) {
            path_pointer[j] = dirname[j];
        }

        /* Add backslash after directory path, before file name. */
        path_pointer[j] = '\\';
        int i = 0;
        j++;
        for (i = 0; i < strlen(filename); i++) {
            path_pointer[j] = filename[i];
            j++;
        }
        path_pointer[j] = '\0';
    }
}
