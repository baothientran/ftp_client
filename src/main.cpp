#include <iostream>
#include <string>
#include "FtpService.h"


const std::string DEFAULT_FTP_PORT = "21";


void displayUsage() {
    std::cout << "Usage: ftp_client_exe [IP addr or hostname] [log file] [port number]\n";
    std::cout << "[IP addr or hostname]: REQUIRED. The IP address or hostname of ftp server to connect to\n";
    std::cout << "[log file           ]: REQUIRED. The log file to log the client actions\n";
    std::cout << "[port number        ]: OPTIONAL. The port number used to connect to ftp server. Default is port 21\n";
}


int main(int argc, const char **argv) {
    // parsing command line
    std::string hostAddr, logFile, port;
    if (argc == 3 || argc == 4) {
        hostAddr = argv[1];
        logFile  = argv[2];
        port     = argc == 4 ? argv[3] : DEFAULT_FTP_PORT;
    }
    else {
        displayUsage();
        exit(0);
    }

    // connect to ftp server
    auto ftpService = std::make_unique<FtpService>();
    auto ftpStat    = ftpService->connectHost(hostAddr, port);
    if (ftpStat.code == CONNECTION_ERROR) {
        std::cout << ftpStat.msg << "\n";
        exit(0);
    }

    // ready to receive command from user
    while (true) {
        std::string cmd;
        std::cout << "> ";
        std::getline(std::cin, cmd);
    }

    return 0;
}
