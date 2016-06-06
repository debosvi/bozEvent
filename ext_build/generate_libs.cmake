
set(DEFAULT_GENERATE_LIBRARY_TYPE "static")

function(GENERATE_LIBRARY)
    set(options OPTIONAL)
    set(oneValueArgs NAME TYPE)
    set(multiValueArgs SOURCES LIBS SYSLIBS DEPS)
    cmake_parse_arguments(GENERATE_LIBRARY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # message(STATUS "Library generation: ${GENERATE_LIBRARY_NAME}")
    # message(STATUS "Type: ${GENERATE_LIBRARY_TYPE}")
    # message(STATUS "Sources: ${GENERATE_LIBRARY_SOURCES}")
    # message(STATUS "Libraries (internal): ${GENERATE_LIBRARY_LIBS}")
    # message(STATUS "Libraries (system): ${GENERATE_LIBRARY_SYSLIBS}")
    # message(STATUS "Dependencies: ${GENERATE_LIBRARY_DEPS}")

    if("${GENERATE_LIBRARY_NAME}" STREQUAL "")
        message(FATAL_ERROR "Must set library name")
    endif()  

    set(OUT_TARGET ${LIBRARY_TARGET_PREFIX}${GENERATE_LIBRARY_NAME})
    if("${GENERATE_LIBRARY_TYPE}" STREQUAL "")
        set(GENERATE_LIBRARY_TYPE ${DEFAULT_GENERATE_LIBRARY_TYPE})
    else()
    endif()

    # build a library
    if("${GENERATE_LIBRARY_TYPE}" STREQUAL "static")
        message(STATUS "Build static library for '${OUT_TARGET}' target")
        add_library(${OUT_TARGET} STATIC ${GENERATE_LIBRARY_SOURCES})
    elseif("${GENERATE_LIBRARY_TYPE}" STREQUAL "dynamic")
        message(STATUS "Build dynamic library for '${OUT_TARGET}' target")
        add_library(${OUT_TARGET} SHARED ${GENERATE_LIBRARY_SOURCES})
    else()
        message(FATAL_ERROR "Build type unknown (${GENERATE_LIBRARY_TYPE}) for ${OUT_TARGET} target")
    endif()
 
 
    # link with declared libs
    foreach(lib ${GENERATE_LIBRARY_LIBS})
        target_link_libraries(${OUT_TARGET} ${LIBRARY_TARGET_PREFIX}${lib})
    endforeach()
    foreach(lib ${GENERATE_LIBRARY_SYSLIBS})
        target_link_libraries(${OUT_TARGET} ${lib})
    endforeach()

    # depends on other targetss
    foreach(tgt ${GENERATE_LIBRARY_DEPS})
        add_dependencies(${OUT_TARGET} ${tgt})
    endforeach()

    # rename library
    set_target_properties(${OUT_TARGET} PROPERTIES 
        OUTPUT_NAME ${GENERATE_LIBRARY_NAME}
    )

    # add to global libraries build list
    add_dependencies(libs ${OUT_TARGET})
endfunction()
