cmake_minimum_required (VERSION 2.8.11)
project (dirent)

enable_language (C)

install (FILES include/dirent.h DESTINATION include)

# Add include directory to path if current system omits dirent.h file.  This
# step is imporant for compiling the test programs under Linux.
include (CheckIncludeFiles)
check_include_files (dirent.h HAVE_DIRENT_H)
if (NOT HAVE_DIRENT_H)
    include_directories (${CMAKE_SOURCE_DIR}/include)
endif (NOT HAVE_DIRENT_H)

# Build example programs
add_executable (find examples/find.c)
add_executable (ls examples/ls.c)
add_executable (locate examples/locate.c)
add_executable (updatedb examples/updatedb.c)

# Build test programs
include (CTest)
add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_CFG_INTDIR})
function (add_test_executable TEST_NAME)
    add_executable (${TEST_NAME} EXCLUDE_FROM_ALL ${ARGN})
    add_test (NAME ${TEST_NAME} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} COMMAND $<TARGET_FILE:${TEST_NAME}>)
    add_dependencies (check ${TEST_NAME})
endfunction (add_test_executable)

add_test_executable (t-compile tests/t-compile.c)
add_test_executable (t-dirent tests/t-dirent.c)

