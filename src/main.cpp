#include <iostream>
#include <string>
#include <fstream>
#include "Cmd.h"
#include "Utility.h"


const std::string DEFAULT_FTP_PORT = "21";


/*
 * displayUsage()
 * Display the help message when user enter wrong command line arguments to the main program
 */
void displayUsage() {
    std::cout << "Usage: ftp_client_exe [IP addr or hostname] [log file] [port number]\n";
    std::cout << "[IP addr or hostname]: REQUIRED. The IP address or hostname of ftp server to connect to\n";
    std::cout << "[log file           ]: REQUIRED. The log file to log the client actions\n";
    std::cout << "[port number        ]: OPTIONAL. The port number used to connect to ftp server. Default is port 21\n";
}


/*
 * main()
 * Main function of ftp client. Responsible for parsing command line arguments,
 * Openning log file, and Running Command Service to get input from the user
 */
int main(int argc, const char **argv) {
    // parsing command line
    std::string hostname, logFile;
    uint16_t port;
    if (argc == 3 || argc == 4) {
        hostname = argv[1];
        logFile  = argv[2];

        std::string portStr  = argc == 4 ? argv[3] : DEFAULT_FTP_PORT;
        int res = toUnsignedInt<uint16_t>(portStr, port);
        if (res == -1) {
            std::cout << "Port not a number.\n";
            exit(0);
        }
        else if (res == 1) {
            std::cout << "Port number overflow.\n";
            exit(0);
        }
    }
    else {
        displayUsage();
        exit(0);
    }

    // open log file
    std::ofstream logger(logFile, std::ios::app);
    if (!logger) {
        std::cout << "Cannot open file " << logFile << "\n";
        exit(0);
    }

    // run command service
    CommandService cmdService(&std::cout, &std::cin, &logger, hostname, port);
    cmdService.run();

    exit(0);
}
