if(${MARGO_FOUND})
	add_executable(02_margo_sum_server sum_server.c)
	target_link_libraries(02_margo_sum_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARIES})

	add_executable(02_margo_sum_client sum_client.c)
	target_link_libraries(02_margo_sum_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARIES})
endif()
