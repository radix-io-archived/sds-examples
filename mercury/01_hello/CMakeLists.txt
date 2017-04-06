if(${MERCURY_FOUND})
	add_executable(01_hg_hello_server hello_server.c)
	target_link_libraries(01_hg_hello_server ${MERCURY_LIBRARIES} ${BMI_LIBRARIES})

	add_executable(01_hg_hello_client hello_client.c)
	target_link_libraries(01_hg_hello_client ${MERCURY_LIBRARIES} ${BMI_LIBRARIES})
endif()
