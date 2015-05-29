
set(BOZ_EVENT_APP bevt_relayd)

set(SOURCES
    bevt_relay_db_init.c
    bevt_relayd.c
    )
    
add_executable(${BOZ_EVENT_APP}
    ${SOURCES}
    )

target_link_libraries(${BOZ_EVENT_APP}
    bevt_debug
    skarnet
    )
    
