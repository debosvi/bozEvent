
macro(BUILD_AND_SET_INCLUDE )
    foreach(arg ${ARGN})
        include_directories(${arg})
    endforeach()
    foreach(arg ${ARGN})
        add_subdirectory(${arg})
    endforeach()
endmacro()

BUILD_AND_SET_INCLUDE(debug client relay central) 
