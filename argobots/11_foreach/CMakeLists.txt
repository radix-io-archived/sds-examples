if(${SDS_UTILS_FOUND})
  add_executable(11_foreach 11_foreach.cpp)
  target_link_libraries(11_foreach argobots)
endif()
