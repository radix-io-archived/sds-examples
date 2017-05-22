if(${MARGO_FOUND})
	add_executable(backup_server backup_server.c)
	target_link_libraries(backup_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARY})

	add_executable(backup_client back_client.c)
	target_link_libraries(backup_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES} ${ABT_SNOOZER_LIBRARIES} ${ABT_LIBRARIES} ${MARGO_LIBRARY})
endif()
