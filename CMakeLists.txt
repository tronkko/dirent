cmake_minimum_required (VERSION 2.8)
project (dirent)

enable_language(C)

install(FILES include/dirent.h DESTINATION include)


include(CTest)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_CFG_INTDIR})
function(add_test_executable TEST_NAME)
    add_executable (${TEST_NAME} EXCLUDE_FROM_ALL ${ARGN})
    add_test(NAME ${TEST_NAME} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests COMMAND $<TARGET_FILE:${TEST_NAME}>)
    add_dependencies(check ${TEST_NAME})
    target_include_directories(${TEST_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
endfunction(add_test_executable)

add_test_executable(t-compile tests/t-compile.c)
add_test_executable(t-dirent tests/t-dirent.c)
