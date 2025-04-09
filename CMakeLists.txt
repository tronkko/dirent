cmake_minimum_required(VERSION 3.10.2)
project(dirent C CXX)

# Set PROJECT_IS_TOP_LEVEL variable for CMake < 3.21
# https://cmake.org/cmake/help/latest/variable/PROJECT_IS_TOP_LEVEL.html
if (CMAKE_VERSION VERSION_LESS 3.21)
  string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} PROJECT_IS_TOP_LEVEL)
endif()

# Declare three-state DIRENT_EXAMPLES option to enable or disable building of
# examples.
set(DIRENT_EXAMPLES "AUTO" CACHE STRING "Build dirent example programs")
set_property(CACHE DIRENT_EXAMPLES PROPERTY STRINGS "AUTO" "ON" "OFF")

# Declare three-state DIRENT_TESTS option to enable or disable building of
# unit tests.
set(DIRENT_TESTS "AUTO" CACHE STRING "Build dirent test programs")
set_property(CACHE DIRENT_TESTS PROPERTY STRINGS "AUTO" "ON" "OFF")

# Current API version
set(DIRENT_VERSION 1.25)

# Publish dirent as a library
add_library(dirent INTERFACE)

# Include dirent.h only when compiling with Microsoft Visual Studio.  Other
# compilers use their own dirent.h or one provided by the operating system.
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  target_include_directories(dirent INTERFACE include)
  message(STATUS "Using dirent.h from ${PROJECT_SOURCE_DIR}/include")
endif()

# Build example programs when cmake is invoked with -DDIRENT_EXAMPLES=ON or
# when dirent is compiled as a top level project.
if(DIRENT_EXAMPLES STREQUAL "ON" OR (DIRENT_EXAMPLES STREQUAL "AUTO" AND PROJECT_IS_TOP_LEVEL))
  foreach(source IN ITEMS find.c ls.c locate.c updatedb.c scandir.c cat.c dir.c du.c extension_lookup.cpp)
    get_filename_component(target ${source} NAME_WE)
    add_executable(${target} examples/${source})
    target_link_libraries(${target} dirent)
  endforeach()
  message(STATUS "Dirent example programs included in build")
else()
  message(STATUS "Dirent example programs excluded from build")
endif()

# Build test programs when cmake is invoked with -DDIRENT_TESTS=ON or when
# dirent is compiled as a top level project.
if(DIRENT_TESTS STREQUAL "ON" OR (DIRENT_TESTS STREQUAL "AUTO" AND PROJECT_IS_TOP_LEVEL))
  # Add target "check" to build and run test programs
  enable_testing()
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_CFG_INTDIR})

  # Build test programs and add them as dependencies to the check target
  foreach(source IN ITEMS t-compile.c t-dirent.c t-scandir.c t-unicode.c t-cplusplus.cpp t-telldir.c t-strverscmp.c t-utf8.c t-symlink.c)
    get_filename_component(target ${source} NAME_WE)
    add_executable(${target} tests/${source})
    target_link_libraries(${target} PRIVATE dirent)
    add_test(NAME ${target} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    set_tests_properties(${target} PROPERTIES SKIP_RETURN_CODE 77)
    add_dependencies(check ${target})
  endforeach()
  message(STATUS "Dirent unit tests included in build")
else()
  message(STATUS "Dirent unit tests excluded from build")
endif()

# Install files to the installation directory specified with
# CMAKE_INSTALL_PREFIX variable.
include(CMakePackageConfigHelpers)
set(ConfigPackageLocation lib/cmake/Dirent-${DIRENT_VERSION})
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/Dirent/DirentConfigVersion.cmake" VERSION ${DIRENT_VERSION} COMPATIBILITY AnyNewerVersion)
configure_file(DirentConfig.cmake "${CMAKE_CURRENT_BINARY_DIR}/Dirent/DirentConfig.cmake")
install(
  FILES
    include/dirent.h
  DESTINATION
    include/dirent-${DIRENT_VERSION}
  COMPONENT
    dev
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Dirent/DirentConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Dirent/DirentConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    dev
)
