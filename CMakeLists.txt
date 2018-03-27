cmake_minimum_required (VERSION 2.6)
project (SanSocks CXX)

# ---[ CTest
include(CTest)

# ---[ Dependencies
find_package(Threads REQUIRED)

# ---[ C++1y Flags
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++1y" COMPILER_SUPPORTS_CXX1y)

if(COMPILER_SUPPORTS_CXX1y)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++1y support. Please use a different C++ compiler.")
endif()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

# Create a new pre-processor macro __VTableFILE__ that has a truncated
# path to reduce the size of the debug log messages.
# Source: http://stackoverflow.com/a/16658858
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__VTableFILE__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

# --[ Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Wno-unused-private-field")

# --[ Debug Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUGS} -O0 -ggdb -fno-omit-frame-pointer -fno-optimize-sibling-calls")

# --[ Output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --[ Includes
set(SANSOCK_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(SANSOCK_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
set(SANSOCK_THIRD_PARITY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party)
include_directories(${SANSOCK_SRC_INCLUDE_DIR}
  ${SANSOCK_TEST_INCLUDE_DIR}
  ${SANSOCK_THIRD_PARITY_INCLUDE_DIR})
include_directories(BEFORE src)

# --[ Subdirectory
add_subdirectory(src)
# add_subdirectory(test)
