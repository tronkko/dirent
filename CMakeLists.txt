cmake_minimum_required(VERSION 3.30)
project(dirent C CXX)

add_library(dirent INTERFACE)

if(NOT CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  return()
endif()

target_include_directories(dirent INTERFACE include)

include(CTest)

option(DIRENT_BUILD_EXAMPLES "" ON)

if(${DIRENT_BUILD_EXAMPLES})
  foreach(example IN ITEMS find ls locate updatedb scandir cat dir du)
    block()
      set(target ${PROJECT_NAME}_example_${example})
      add_executable(${target} examples/${example}.c)
      target_link_libraries(${target} dirent)
    endblock()
  endforeach()
endif()

if(${BUILD_TESTING})
  block()
    file(GLOB glob LIST_DIRECTORIES TRUE tests/*)
    foreach(entry IN LISTS glob)
      if(IS_DIRECTORY ${entry})
        file(COPY ${entry} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tests)
      endif()
    endforeach()
  endblock()

  foreach(test IN ITEMS compile dirent scandir unicode cplusplus telldir strverscmp utf8 symlink)
    block()
      set(source_path tests/t-${test})

      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${source_path}.c)
        set(source ${source_path}.c)
      else()
        set(source ${source_path}.cpp)
      endif()

      set(target ${PROJECT_NAME}_test_${test})
      add_executable(${target} ${source})
      target_link_libraries(${target} PRIVATE dirent)
      add_test(${target} ${target})
      set_tests_properties(${target} PROPERTIES SKIP_RETURN_CODE 77)
    endblock()
  endforeach()
endif()