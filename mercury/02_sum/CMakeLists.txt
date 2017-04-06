if(${MERCURY_FOUND})
	add_executable(02_hg_sum_server sum_server.c)
	target_link_libraries(02_hg_sum_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES})

	add_executable(02_hg_sum_client sum_client.c)
	target_link_libraries(02_hg_sum_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES})
endif()
