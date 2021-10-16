/*
 * An example demonstrating basic file information listing.
 *
 * Compile this file with Visual Studio and run the produced command in
 * console with a directory name argument.  For example, command
 *
 *     dirls "C:\Users\User 1\Documents"
 *
 * might output something like
 *
 *     ./
 *     ../
 *     budget.xlsx 19180
 *     directory_1/
 *     directory_2/
 *     notes.txt 320
 *     to_do_list.txt 245
 *
 * The dirls command provided by this file is only an example: the command
 * does not have any fancy options.
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

#define ERR_MSG_LEN 256

static void list_directory(const char* dirname);
static void list_size(const char* full_path);
static void print_error_msg(const char* dirname, char* msg_buffer);
static void get_full_path(const char* dirname, const char* filename, char* buffer);


int main(int argc, char* argv[])
{
    /* Select default locale */
    setlocale(LC_ALL, "");

    /* For each directory in command line. */
    int i = 1;
    while (i < argc) {
        list_directory(argv[i]);
        i++;
        printf("\n");
    }

    /* List current working directory if no arguments on command line */
    if (argc == 1)
        list_directory(".");

    return EXIT_SUCCESS;
}

/*
 * List files and file sizes in directory.
 */
static void list_directory(const char* dirname)
{
    /* Open directory stream */
    DIR* dir = opendir(dirname);
    if (!dir) {
        /* Could not open directory; print error message */
        char msg_buff[ERR_MSG_LEN];

        /* Function replaces strerror with strerror_s */
        print_error_msg(dirname, msg_buff);

        exit(EXIT_FAILURE);
    }

    struct dirent* ent;
    /* Print all files, file sizes, and directories within the directory */
    while ((ent = readdir(dir)) != NULL) {
        char buffer[PATH_MAX + 2];

        switch (ent->d_type) {
        case DT_REG:
            printf("%s ", ent->d_name);
            /* Get full file path */
            get_full_path(dirname, ent->d_name, buffer);
            /* Print file size next to file name */
            list_size(buffer);
            break;

        case DT_DIR:
            printf("%s/\n", ent->d_name);
            break;

        case DT_LNK:
            printf("%s@ ", ent->d_name);
            /* Get full file path */
            get_full_path(dirname, ent->d_name, buffer);
            /* Print file size next to file name */
            list_size(buffer);
            break;

        default:
            printf("%s* ", ent->d_name);
            /* Get full file path */
            get_full_path(dirname, ent->d_name, buffer);
            /* Print file size next to file name */
            list_size(buffer);
            break;
        }
    }

    closedir(dir);
}

/*
 * Print file size next to file name.
 */
static void list_size(const char* full_path)
{
    struct stat stbuf;

    if (stat(full_path, &stbuf) == -1)
        printf("%s %s\n", "Can't access", full_path);
    else
        printf("%d\n", stbuf.st_size);
}

/*
 * Enforce error message size limit and ensure valid error number.
 * Print error message.
 */
static void print_error_msg(const char* dirname, char* msg_buff)
{
    int error_num;
    error_num = strerror_s(msg_buff, ERR_MSG_LEN, errno);

    switch (error_num) {
    case 0:
        printf("Cannot open %s (%s)\n", dirname, msg_buff);
        break;

    case EINVAL:
        printf("strerror_s() failed: invalid error code, %d\n",
            error_num);
        break;

    case ERANGE:
        printf("strerror_s() failed: buffer too small: %d\n",
            ERR_MSG_LEN);
        break;
    }
}

/*
 * Combine directory and file name.
 * Write full path to memory.
 */
static void get_full_path(const char* dirname, const char* filename, char* buffer)
{
    char* dest = buffer;
    char* end = &buffer[PATH_MAX];

    /* Copy directory name to buffer */
    const char* src = dirname;
    while (dest < end && *src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';

    char* new_dest = dest;
    char c;

    /* Get final character of directory name */
    if (buffer < new_dest)
        c = new_dest[-1];
    else
        c = ':';

    /* Append directory separator if not already there */
    if (c != ':' && c != '/' && c != '\\')
        *new_dest++ = '/';

    /* Append file name */
    src = filename;
    while (new_dest < end && *src != '\0') {
        *new_dest++ = *src++;
    }
    *new_dest = '\0';
}
