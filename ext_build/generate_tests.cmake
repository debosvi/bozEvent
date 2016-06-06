
option(GLOBAL_TEST_GENERATION "Activate global test generation" OFF)

function(GENERATE_TEST)
    if(${GLOBAL_TEST_GENERATION})
        set(options OPTIONAL)
        set(oneValueArgs NAME)
        set(multiValueArgs SOURCES LIBS SYSLIBS DEPS)
        cmake_parse_arguments(GENERATE_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

        # message(STATUS "Test generation: ${GENERATE_TEST_NAME}")
        # message(STATUS "Sources: ${GENERATE_TEST_SOURCES}")
        # message(STATUS "Libraries (internal): ${GENERATE_TEST_LIBS}")
        # message(STATUS "Libraries (system): ${GENERATE_TEST_SYSLIBS}")
        # message(STATUS "Dependencies: ${GENERATE_TEST_DEPS}")

        if("${GENERATE_TEST_NAME}" STREQUAL "")
            message(FATAL_ERROR "Must set test name")
        endif()  

        set(OUT_TARGET ${TEST_TARGET_PREFIX}${GENERATE_TEST_NAME})

        message(STATUS "Build test application '${OUT_TARGET}' target")

        add_executable(${OUT_TARGET} ${GENERATE_TEST_SOURCES})
        
        foreach(lib ${GENERATE_TEST_LIBS})
            target_link_libraries(${OUT_TARGET} ${LIBRARY_TARGET_PREFIX}${lib})
        endforeach()
        foreach(lib ${GENERATE_TEST_SYSLIBS})
            target_link_libraries(${OUT_TARGET} ${lib})
        endforeach()
        
        ## depends on other targetss
        foreach(tgt ${GENERATE_TEST_DEPS})
            add_dependencies(${OUT_TARGET} ${tgt})
        endforeach()

        # set_target_properties(${OUT_TARGET} PROPERTIES 
            # OUTPUT_NAME ${OUT_TARGET}
        # )
        
        ## add to global tests build list
        add_dependencies(tests ${OUT_TARGET})
    endif()
endfunction()
