
macro(SET_AND_DEFINE var value)
    set(${var} ${value})
    add_definitions(-D${var}="${value}")
endmacro()

set(USR_DIR /usr/share/boz)
if(NOT "${BOZ_WIDE_GLOBAL_PATH}" STREQUAL "")
	set(USR_DIR ${BOZ_WIDE_GLOBAL_PATH})
endif()
message(STATUS "Global install path: " ${USR_DIR})

SET_AND_DEFINE(BINARY_INSTALL_DIR ${USR_DIR}/bin)
SET_AND_DEFINE(LIBRARY_INSTALL_DIR ${USR_DIR}/lib)
SET_AND_DEFINE(HEADER_INSTALL_DIR ${USR_DIR}/include)
SET_AND_DEFINE(LOG_INSTALL_DIR ${USR_DIR}/log)
SET_AND_DEFINE(BOZ_WEB_ROOT_DIR ${USR_DIR}/www)
SET_AND_DEFINE(CONFIG_INSTALL_DIR ${USR_DIR}/configs)
SET_AND_DEFINE(SCRIPT_INSTALL_DIR ${USR_DIR}/scripts)
SET_AND_DEFINE(SERVICES_INSTALL_DIR ${USR_DIR}/s6-services)
SET_AND_DEFINE(SERVICE_READY_DIR /tmp/s6-services)
SET_AND_DEFINE(SERVICE_SCAN_DIR /tmp/s6-service)
SET_AND_DEFINE(SERVICES_LOG_INSTALL_DIR ${LOG_INSTALL_DIR}/s6-services)

set(BOZ_CONFIG_DIR ${CMAKE_BINARY_DIR}/config)
set(LOAD_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)

SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH ${LIBRARY_INSTALL_DIR})
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

option(BUILD_TYPE "Which build type to generate" "debug")
         
if("${BUILD_TYPE}" STREQUAL "release")
    set(CMAKE_BUILD_TYPE RELEASE)
elseif("${BUILD_TYPE}" STREQUAL "debug")
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    set(CMAKE_BUILD_TYPE DEBUG)
endif()

message(STATUS "Build mode: "${CMAKE_BUILD_TYPE})

if(EXISTS "${LOAD_CONFIG_DIR}/Config.in")
add_definitions(-include ${CMAKE_BINARY_DIR}/config/config.h )
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    add_definitions(-D__DEBUG__)
endif()

add_definitions(-std=c99)
add_definitions(-Wall  -Wextra)

set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O2 -Werror")

set(CMAKE_C_FLAGS "-Wmissing-prototypes -Wbad-function-cast -Wshadow -Wfloat-equal")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default -Wno-unused-result")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdeclaration-after-statement")
add_definitions(-Wmissing-declarations -Wwrite-strings )
add_definitions(-Wunknown-pragmas)
add_definitions(-Wformat-security)
add_definitions(-Wstrict-aliasing -Wreturn-type -Wmissing-noreturn)
