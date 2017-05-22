if(${MARGO_FOUND})
	add_executable(phone_server phone_server.c phone_book.c)
	target_link_libraries(phone_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARIES})

	add_executable(phone_client phone_client.c)
	target_link_libraries(phone_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARIES})
endif()
