# This Linux Makefile will pack dirent into a distributable zip
# package.  To build the package, invoke this Makefile as
#
#     make dist
#

# Current cersion number
VERSION=1.21

# List of files to include in zip package
DIST_FILES = include/*.h examples/*.c tests/*.c tests/1/file tests/1/dir/* tests/2/* vs2008/vs2008.sln vs2008/ls/ls.vcproj vs2008/find/find.vcproj vs2008/updatedb/updatedb.vcproj vs2008/locate/locate.vcproj vs2008/t-dirent/t-dirent.vcproj ChangeLog

# Default target
all: ls find t-dirent

# Build Linux versions of example programs
ls: examples/ls.c
	gcc -W -Wall -o ls examples/ls.c

find: examples/find.c
	gcc -W -Wall -o find examples/find.c

t-dirent: tests/t-dirent.c
	gcc -W -Wall -o t-dirent tests/t-dirent.c

# Show usage
help:
	@echo "Build targets available:"
	@echo "  clean   Remove temporary files"
	@echo "  dist    Build distributable package"

# Wipe out temporary files left behind by MS-Visual Studio
clean:
	rm -f ls find t-dirent
	rm -f vs2008/*.suo vs2008/*.ncb
	rm -f vs2008/*/*.user
	rm -fr vs2008/Debug vs2008/*/Debug
	rm -fr vs2008/Release vs2008/*/Release
	rm -fr dirent-$(VERSION)
	rm -f dirent-*.zip

# Build installation package
dist: dirent-$(VERSION).zip
dirent-$(VERSION).zip: $(DIST_FILES) Makefile
	rm -f dirent-$(VERSION).zip
	rm -fr dirent-$(VERSION)
	mkdir dirent-$(VERSION)
	for f in $(DIST_FILES); do \
	    dir=`echo "$$f" | sed -e 's:^[^/]*$$::' -e 's://*[^/]*$$::' -e 's:^$$:.:'`; \
	    if [ -d "dirent-$(VERSION)/$$dir" ]; then \
	        :; \
	    else \
	    	mkdir "dirent-$(VERSION)/$$dir"; \
	    	chmod 0755 "dirent-$(VERSION)/$$dir"; \
	    fi; \
	    cp "$$f" "dirent-$(VERSION)/$$dir/"; \
	    chmod 0644 "dirent-$(VERSION)/$$f"; \
	done
	( cd dirent-$(VERSION) && zip -r ../dirent-$(VERSION).zip . )
	rm -fr dirent-$(VERSION)

