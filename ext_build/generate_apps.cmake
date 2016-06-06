
function(GENERATE_APPLICATION)
    set(options OPTIONAL)
    set(oneValueArgs NAME SUFFIX)
    set(multiValueArgs SOURCES LIBS SYSLIBS DEPS)
    cmake_parse_arguments(GENERATE_APPLICATION "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # message(STATUS "Application generation: ${GENERATE_APPLICATION_NAME}")
    # message(STATUS "Sources: ${GENERATE_APPLICATION_SOURCES}")
    # message(STATUS "Libraries (internal): ${GENERATE_APPLICATION_LIBS}")
    # message(STATUS "Libraries (system): ${GENERATE_APPLICATION_SYSLIBS}")
    # message(STATUS "Dependencies: ${GENERATE_APPLICATION_DEPS}")
    # message(STATUS "Suffix: ${GENERATE_APPLICATION_SUFFIX}")

    if("${GENERATE_APPLICATION_NAME}" STREQUAL "")
        message(FATAL_ERROR "Must set application name")
    endif()  

    set(OUT_TARGET ${APPLICATION_TARGET_PREFIX}${GENERATE_APPLICATION_NAME})

    message(STATUS "Build application '${OUT_TARGET}' target")

    add_executable(${OUT_TARGET} ${GENERATE_APPLICATION_SOURCES})

    foreach(lib ${GENERATE_APPLICATION_LIBS})
        target_link_libraries(${OUT_TARGET} ${LIBRARY_TARGET_PREFIX}${lib})
    endforeach()
    foreach(lib ${GENERATE_APPLICATION_SYSLIBS})
        target_link_libraries(${OUT_TARGET} ${lib})
    endforeach()

    ## depends on other targetss
    foreach(tgt ${GENERATE_APPLICATION_DEPS})
        add_dependencies(${OUT_TARGET} ${tgt})
    endforeach()

    set_target_properties(${OUT_TARGET} PROPERTIES 
        OUTPUT_NAME ${GENERATE_APPLICATION_NAME}
    )
    if(NOT "${GENERATE_APPLICATION_SUFFIX}" STREQUAL "")
        set_target_properties(${OUT_TARGET} PROPERTIES 
            SUFFIX ".${GENERATE_APPLICATION_SUFFIX}"
        )
    endif()
            
    ## add to global applications build list
    add_dependencies(apps ${OUT_TARGET})	
endfunction()
