
set(TEST_BIN test_client_util)
add_definitions(-DPROGNAME="${TEST_BIN}")

add_executable(${TEST_BIN}
    util.c
    )

target_link_libraries(${TEST_BIN}
    bevt_client
    bevt_relay
    )
