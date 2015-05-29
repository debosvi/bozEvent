
set(BOZ_EVENT_LIB bevt_client)

set(SOURCES
    bevt_client_p.c
    bevt_client_init.c
    bevt_client_finalise.c
    bevt_client_process.c
    )
    
add_library(${BOZ_EVENT_LIB} STATIC
    ${SOURCES}
    )

target_link_libraries(${BOZ_EVENT_LIB}
    bevt_debug
    skarnet
    )
    
