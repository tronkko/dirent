# Dirent
Dirent is a C/C++ programming interface that allows programs to retrieve
information on files and directories residing on a hard disk.  Dirent is
commonly found in Linux and other UNIX operating systems.

This project provides Linux-compatible Dirent interface for Microsoft Visual
Studio and Open Watcom to be used on Microsoft Windows.


# Installation

Download the latest Dirent installation package from
[softagalleria.net](http://softagalleria.net/download/dirent/?C=M;O=D)

Unpack the installation file with 7-zip, for example.  The installation
package contains dirent.h file as well as a few example programs and a
Visual Studio 2008 project for compiling them.  You will definitely need the
``include/dirent.h`` file.


## Install Dirent for All Programs

To make dirent.h available for all programs, copy the ``include/dirent.h``
file from the installation package to system include directory.  System
include directory contains header files such as assert.h and windows.h.  In
Visual Studio 2008, for example, the system include directory may be found at
``C:\Program Files\Microsoft Visual Studio 9.0\VC\include``.

Everything you need is included in the single dirent.h file.  Thus, you can
start using Dirent immediately -- there is no need to add files to your
Visual Studio project.


## Embed Dirent into Your Own Program

As an alternative to the above installation method, you can also embed
dirent.h into your own program -- just copy the ``include/dirent.h`` file to
a suitable include directory in your project.  However, if you do so, please
make sure that you include dirent.h by

```
#include <dirent.h> /*good*/
```

and not 

```
#include "dirent.h" /*bad*/
```

The former code instructs the compiler to look for dirent.h first from the
system include directory and only then from your project specific include
directory.  This arrangement allows your program to compile cleanly under
Linux against the dirent.h file provided by the operating system.


# Copying

Dirent may be freely distributed under the MIT license.  See the LICENSE
file for details.


# Alternatives to Dirent

I ported Dirent to Microsoft Windows in 1998 when only a few alternatives
were available.  However, the situation has changed since then and nowadays
both [Cygwin](http://www.cygwin.com) and [MingW](http://www.mingw.org)
allow you to compile a great number of UNIX programs in Microsoft Windows.
They both provide a full dirent API as well as many other UNIX APIs.  MingW
can even be used for commercial applications!
