add_executable(client client.cpp)
target_link_libraries(client cipher ${Boost_LIBRARIES} pthread)
