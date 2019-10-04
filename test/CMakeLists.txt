project(test_ftp_client LANGUAGES CXX)

add_executable(test_ftp_client
    "main.cpp"
    "FtpServiceTest.cpp")

target_link_libraries(test_ftp_client PRIVATE ftp_client_lib)
target_include_directories(test_ftp_client PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME test_ftp_client COMMAND test_ftp_client)
