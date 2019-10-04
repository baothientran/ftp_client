cmake_minimum_required(VERSION 3.5)

project(ftp_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
add_test(NAME test_ftp_client COMMAND test_ftp_client)
