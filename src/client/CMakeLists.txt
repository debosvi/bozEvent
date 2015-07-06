
set(BOZ_EVENT_LIB bevt_client)

set(SOURCES
    bevt_client_p.c
    bevt_client_init.c
    bevt_client_finalise.c
    bevt_client_process.c
    bevt_client_parse_prot_cmd.c
    bevt_client_parse_prot_event.c
    bevt_client_register.c
    bevt_client_subscribe.c
    bevt_client_unregister.c
    bevt_client_unsubscribe.c
    )
    
add_library(${BOZ_EVENT_LIB} STATIC
    ${SOURCES}
    )

target_link_libraries(${BOZ_EVENT_LIB}
    bevt_debug
    skarnet
    )
    
BOZ_INSTALL_ITEM(${BOZ_EVENT_LIB})
