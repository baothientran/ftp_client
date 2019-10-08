project(ftp_client_lib LANGUAGES CXX)

set(src
    "Cmd.cpp"
    "Utility.cpp"
    "FtpService.cpp")

set(header
    "Cmd.h"
    "Utility.h"
    "FtpService.h")

add_library(ftp_client_lib
    ${src}
    ${header}
)
target_compile_features(ftp_client_lib PUBLIC cxx_std_14)
target_include_directories(ftp_client_lib PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(ftp_client_exe
    "main.cpp"
)
target_link_libraries(ftp_client_exe PRIVATE ftp_client_lib)
