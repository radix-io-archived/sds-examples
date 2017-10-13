add_executable(05_hg_save_server save_server.c)
target_link_libraries(05_hg_save_server mercury rt)

add_executable(05_hg_save_client save_client.c)
target_link_libraries(05_hg_save_client mercury)
