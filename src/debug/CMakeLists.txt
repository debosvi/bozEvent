
set(BOZ_DEBUG_LIB bevt_debug)

set(SOURCES
    bevt_debug_p.c
    )
    
add_library(${BOZ_DEBUG_LIB} STATIC
    ${SOURCES}
    )

#target_link_libraries(${BOZ_DEBUG_LIB}
#    skarnet
#    )
    
