
# install scripts and specific stuffs
install(FILES bevt_relayd_dbcreate.sql 
    DESTINATION ${SCRIPT_INSTALL_DIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

# build target
set(BOZ_RELAY_APP bevt_relayd)

set(SOURCES
    bevt_relay_db_close.c
    bevt_relay_db_init.c
    bevt_relay_main_socket.c
    bevt_relay_parse_prot_cmd.c
    bevt_relay_parse_prot_event.c
    bevt_relayd.c
    )
    
add_executable(${BOZ_RELAY_APP}
    ${SOURCES}
    )

target_link_libraries(${BOZ_RELAY_APP}
    sqlite3
    bevt_debug
    bozmessage
    skarnet
    )
    
BOZ_INSTALL_ITEM(${BOZ_RELAY_APP})
