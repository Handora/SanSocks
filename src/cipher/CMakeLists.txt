add_library(cipher cipher.cpp base64.cpp)

add_executable(generate main.cpp cipher base64.cpp)
