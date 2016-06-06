
set(BOZ_CENTRAL_APP bevt_centrald)

set(SOURCES
    # bevt_central_parse_prot_cmd.c
#    bevt_central_parse_prot_event.c
    # bevt_central_main_socket.c
    bevt_centrald.c
    )
    
add_executable(${BOZ_CENTRAL_APP}
    ${SOURCES}
    )

target_link_libraries(${BOZ_CENTRAL_APP}
    # bevt_debug
    # bevt_relay
    # bozclient
    # bozmessage
    # boztree
    # skarnet
	event
    )
    
BOZ_INSTALL_ITEM(${BOZ_CENTRAL_APP})
