/*
 * A test program to make sure that dirent can identify symlinks.
 *
 * Copyright (C) 1998-2019 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */

#include <dirent.h>
#include <assert.h>
#include <stdio.h>

int countSymlink(const char* dirName) {
    DIR* dir = opendir(dirName);
    if (!dir) {
        fprintf(stderr, "Open directory error: %d", GetLastError());
        return 1;
    }

    struct dirent *ent;
    int found = 0;
    /* Read the directory and count symlinks */
    while ((ent = readdir(dir)) != NULL)
        found += (ent->d_type == DT_LNK);
    return found;
}

int main(int argc, char** argv) {
    /* -s makes link creation failures fatal. Symlink creation requires
     * root access, unless a so-called "Developer Mode" is on, therefore
     * by default creation failure is not a fatal error. */
    int failRet = (argc >= 2 && strcmp(argv[1], "-s") == 0) ? 127 : 0;
    DWORD flag = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;

    /* Chances are that the links are created in previous runs. */
    if (countSymlink("tests\\1") >= 2)
        return 0;

    /* Create symlinks */
    if (!CreateSymbolicLinkA("tests\\1\\link_file", ".\\file", flag)) {
        fprintf(stderr, "Create file symlink error: %d", GetLastError());
        return failRet;
    }
    flag |= SYMBOLIC_LINK_FLAG_DIRECTORY;
    if (!CreateSymbolicLinkA("tests\\1\\link_dir", ".\\dir", flag)) {
        fprintf(stderr, "Create file symlink error: %d", GetLastError());
        return failRet;
    }

    assert(countSymlink("tests\\1") >= 2);
    return 0;
}
