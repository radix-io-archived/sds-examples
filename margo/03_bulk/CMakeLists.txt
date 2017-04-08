if(${MARGO_FOUND})
	add_executable(03_hg_save_server save_server.c)
	target_link_libraries(03_hg_save_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARY})

	add_executable(03_hg_save_client save_client.c)
	target_link_libraries(03_hg_save_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARY})
endif()
