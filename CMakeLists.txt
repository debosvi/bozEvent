cmake_minimum_required (VERSION 2.8)
project (bozEvent)

include(project_ver.cmake)
include(ext_build/boz-build.cmake)
include(CTest)

include(tools.cmake)

#set (CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(-DBEVT_PROG_PREFIX="${BINARY_INSTALL_DIR}/")
add_definitions(-D_POSIX_C_SOURCE=200809L)

include_directories(${CMAKE_BINARY_DIR}/include)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)

add_test_if_enable(test BOZ_GLOBAL_TEST)

include(package/package.cmake)
