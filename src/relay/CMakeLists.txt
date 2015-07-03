
# install scripts and specific stuffs
install(FILES bevt_relayd_dbcreate.sql 
    DESTINATION ${SCRIPT_INSTALL_DIR}
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

# build targets
set(BOZ_RELAY_LIB bevt_relay)

set(SOURCES
    bevt_relay_p.c
    bevt_relay_db_check_reg.c
    bevt_relay_db_check_sub.c
    bevt_relay_db_get_elem.c
    bevt_relay_db_init.c
    bevt_relay_db_close.c
    )
    
add_library(${BOZ_RELAY_LIB} STATIC
    ${SOURCES}
    )

set(BOZ_RELAY_APP bevt_relayd)

set(SOURCES
    bevt_relay_main_socket.c
    bevt_relay_parse_prot_cmd.c
    bevt_relay_parse_prot_event.c
    bevt_relayd.c
    )
    
add_executable(${BOZ_RELAY_APP}
    ${SOURCES}
    )

target_link_libraries(${BOZ_RELAY_APP}
    ${BOZ_RELAY_LIB}
    sqlite3
    bevt_debug
    bozmessage
    skarnet
    )
    
BOZ_INSTALL_ITEM(${BOZ_RELAY_APP})
