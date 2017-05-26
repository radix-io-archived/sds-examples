if(${MARGO_FOUND})
	add_executable(save_server save_server.c)
	target_link_libraries(save_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARY})

	add_executable(save_client save_client.c)
	target_link_libraries(save_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARY})

	add_executable(fwd_server forwarder.c)
	target_link_libraries(fwd_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARY})
endif()
