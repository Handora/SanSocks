add_executable(server server.cpp)
target_link_libraries(server cipher ${Boost_LIBRARIES} pthread) 

add_executable(aserver aserver.cpp)
target_link_libraries(aserver cipher ${Boost_LIBRARIES} pthread) 
