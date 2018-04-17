add_executable(client client.cpp)
target_link_libraries(client cipher)
target_link_libraries(client ${USED_LIBS} pthread)
