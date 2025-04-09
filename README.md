# Dirent 🐬

Dirent a Linux/UNIX programming interface for retrieving information about
files and directories.  This project provides a compatible programming
interface for Microsoft Visual Studio compiler and Microsoft Windows operating
system.  Using this interface, your C or C++ application can scan files and
directories on Microsoft Windows and Linux/UNIX!

Learn more about Dirent from the UNIX specification on
[opengroup.org](https://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html)!


# Requirements 🔥

In order to build unit tests and example programs, you will need:

- [Microsoft Visual Studio](https://visualstudio.microsoft.com/) 2017 or later
- [CMake](https://cmake.org/) version 3.13 or later


# Installation ⚓

Dirent can be installed with four alternative methods.  Method 1 is preferred
for projects built without CMake, method 2 is preferred for projects where
Dirent needs to be customized, method 3 is preferred for projects with
multiple developers, and method 4 is preferred for organizations using Dirent
in a number of projects.

**Method 1**: Download the latest Dirent installation package from
[GitHub](https://github.com/tronkko/dirent/releases) and unpack the
installation file with 7-zip, for example.  Find the `include/dirent.h` file
from the package, copy the file to a separate directory in your project, and
add the directory to include path on Windows.  Having `dirent.h` in a separate
directory allows your project to be compiled against native `dirent.h` on
Linux/UNIX while substituting the functionality on Microsoft Windows.

**Method 2**: Download the latest Dirent installation package from
[GitHub](https://github.com/tronkko/dirent/releases) and unpack the whole
package into your project's main directory.  Source Dirent from your own
`CMakeLists.txt` file as

    add_subdirectory(dirent)

and link Dirent to executables as

    add_executable(app app.c)
    target_link_libraries(app dirent)

**Method 3**: Edit your project's CMakeLists.txt file and load Dirent
automatically from GitHub by adding the following code

    include(FetchContent)
    FetchContent_Declare(
        dirent
        URL https://github.com/tronkko/dirent/archive/refs/tags/1.25.zip
    )
    FetchContent_MakeAvailable(dirent)

Link Dirent to executables as

    add_executable(app app.c)
    target_link_libraries(app dirent)

**Method 4**: Download the latest Dirent installation package from
[GitHub](https://github.com/tronkko/dirent/releases) and unpack files to a
separate directory.  Cd to the directory and generate build files as

    cmake -B build -D CMAKE_INSTALL_PREFIX="%USERPROFILE%\dist" .

where `build` is a new directory for build files, `%USERPROFILE%\dist` is a
directory for installable Dirent files and `.` is the root directory of the
Dirent package containing this README.md file.  Open the generated
`build/dirent.sln` file in Visual Studio and build the INSTALL target.  This
needs to be done only once.

In order to use Dirent in your own projects, update each project's
CMakeLists.txt file by adding line

    find_package(Dirent 1.25 REQUIRED)

Link Dirent to executables as

    add_executable(app app.c)
    target_link_libraries(app dirent)

If CMake cannot find Dirent, then use `-DCMAKE_INSTALL_PREFIX` option while
configuring your own project as

    cmake "-DCMAKE_INSTALL_PREFIX=%USERPROFILE%\dist" src

where `%USERPROFILE%\dist` is the location of installable Dirent files and src
is the location of your own source files.


# Use UTF-8 File Names ⚡

By default, file and directory names in the Dirent API are expressed in the
current Windows codepage.  If you wish to use UTF-8 character encoding at all
times, then replace the `main` function with `_main` function and convert
wide-character arguments to UTF-8 strings as demonstrated in the snippet
below.

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <locale.h>

    /* This is your true main function */
    static int
    _main(int argc, char *argv[])
    {
        /* ... */
        return EXIT_SUCCESS;
    }

    /* Convert arguments to UTF-8 */
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

        /* Convert each argument to UTF-8 */
        for (int i = 0; i < argc; i++) {
            /* Compute the size of corresponding UTF-8 string */
            size_t n;
            wcstombs_s(&n, NULL, 0, argv[i], 0);

            /* Allocate room for UTF-8 string */
            mbargv[i] = (char*) malloc(n + 1);
            if (!mbargv[i]) {
                puts("Out of memory");
                exit(3);
            }

            /* Convert ith argument to UTF-8 */
            wcstombs_s(NULL, mbargv[i], n + 1, argv[i], n);
        }

        /* Pass UTF-8 arguments to the real main program */
        int errorcode = _main(argc, mbargv);

        /* Release UTF-8 arguments */
        for (int i = 0; i < argc; i++) {
            free(mbargv[i]);
        }

        /* Release the multi-byte argv table */
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

For more information on UTF-8 support, please see setlocale in Visual Studio
[C runtime library reference](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-160#utf-8-support).


# Examples 🎓

The source package contains the following example programs.

Program  | Purpose
-------- | -----------------------------------------------------------------
[ls.c](examples/ls.c) | List files in a directory, e.g. `ls "c:\Program Files"`
[dir.c](examples/dir.c) | List files in a directory, e.g. `dir "c:\Program Files"`
[find.c](examples/find.c) | Find files in subdirectories, e.g. `find "c:\Program Files\CMake"`
[updatedb.c](examples/updatedb.c) | Build database of files in a drive, e.g. `updatedb c:\`
[locate.c](examples/locate.c) | Locate a file from database, e.g. `locate notepad`
[scandir.c](examples/scandir.c) | Printed sorted list of file names in a directory, e.g. `scandir .`
[du.c](examples/du.c) | Compute disk usage, e.g. `du "C:\Program Files"`
[cat.c](examples/cat.c) | Print a text file to screen, e.g. `cat include/dirent.h`
[extension\_lookup.cpp](examples/extension_lookup.cpp) | Search files with specific extension

In order to build example programs, unpack the source package to your desktop,
for example, open command prompt and cd to the root directory of the source
package.  Generate build files as

    cmake -B build .

where `build` is a directory for build files and `.` is the root directory of
the source package containing this README.md file.  Once CMake is finished,
open Visual Studio, load the generated `dirent.sln` file from the build
directory and build the whole solution.  Once the build completes, return to
command prompt and cd to the Debug directory to run the example programs.  For
example, run ls from the Debug directory as

    cd Debug
    .\ls .

Examples are not built by default when Dirent is embedded into another
project.  If you want to build examples, then add `-DDIRENT_EXAMPLES=ON`
option to CMake command line when configuring your own project.


# Testing 🔬

This project contains unit tests.  In order to build and run the unit tests,
first unpack the source package to your desktop, open command prompt and cd to
the root directory of the source package.  Generate build files as

    cmake -B build .

where `build` is a directory for build files and `.` is the root directory of
the Dirent package containing this README.md file.  Once CMake is finished,
open Visual Studio, load the generated `dirent.sln` file from the build
directory and build/rebuild solution named "check" in Visual Studio.

Tests are not built by default when Dirent is embedded into another project.
If you want to build tests, then add `-DDIRENT_TESTS=ON` option to CMake
command line when configuring your own project.


# Contributing 🐾

We love to receive contributions from you.  See the
[CONTRIBUTING](CONTRIBUTING.md) file for details.


# Copying 📜

Dirent may be freely distributed under the MIT license.  See the
[LICENSE](LICENSE) file for details.
