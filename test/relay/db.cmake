
include_directories(${CMAKE_SOURCE_DIR}/src/debug)
include_directories(${CMAKE_SOURCE_DIR}/src/relay)
include_directories(${CMAKE_SOURCE_DIR}/src/central)

set(TEST_BIN test_relay_db)
add_executable(${TEST_BIN}
    db.c
    )

target_link_libraries(${TEST_BIN}
    bevt_relay
    sqlite3
    bevt_debug
    skarnet
    )
