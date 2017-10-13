add_executable(phone_server phone_server.c phone_book.c)
target_link_libraries(phone_server margo)

add_executable(phone_client phone_client.c)
target_link_libraries(phone_client margo)
