if(${MERCURY_FOUND})
	add_executable(05_hg_save_server save_server.c)
	target_link_libraries(05_hg_save_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} -lrt)

	add_executable(05_hg_save_client save_client.c)
	target_link_libraries(05_hg_save_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES})
endif()
