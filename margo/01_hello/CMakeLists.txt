if(${MARGO_FOUND})
	add_executable(01_margo_hello_server hello_server.c)
	target_link_libraries(01_margo_hello_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARIES})

	add_executable(01_margo_hello_client hello_client.c)
	target_link_libraries(01_margo_hello_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARIES})
endif()
