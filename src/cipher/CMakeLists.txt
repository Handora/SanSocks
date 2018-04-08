add_library(cipher cipher.cpp base64.cpp)
add_executable(generate cipher.cpp base64.cpp)
