add_executable(save_server save_server.c)
target_link_libraries(save_server margo)

add_executable(save_client save_client.c)
target_link_libraries(save_client margo)

add_executable(fwd_server forwarder.c)
target_link_libraries(fwd_server margo)
