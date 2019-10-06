#include <iostream>
#include "catch.hpp"
#include "FtpService.h"


static void connectLegitServer(FtpService &ftpService) {
    FtpCtrlReply stat;
    REQUIRE(ftpService.openCtrlConnect("10.246.251.93", 21) == true);
    REQUIRE(ftpService.readCtrlReply(stat) == true);
    REQUIRE(stat.code == SERVICE_READY);
    REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

    REQUIRE(ftpService.sendUSER("cs472") == true);
    REQUIRE(ftpService.readCtrlReply(stat) == true);
    REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
    REQUIRE(stat.msg == "331 Please specify the password.\r\n");

    REQUIRE(ftpService.sendPASS("hw2ftp") == true);
    REQUIRE(ftpService.readCtrlReply(stat) == true);
    REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
    REQUIRE(stat.msg == "230 Login successful.\r\n");
}


TEST_CASE("FtpService connect to remote host", "[FtpService]") {
    SECTION("connect to random non-existence server") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("adasdadqlwqwndqbwduiqbwdqwd", 123) == false);
    }

    SECTION("connect ftp server with wrong port") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 2121) == false);
    }

    SECTION("legit ftp server") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 21) == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");
    }
}


TEST_CASE("FtpService authenticate user to remote host", "[FtpService]") {
    SECTION("good username and password") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 21) == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        REQUIRE(ftpService->sendUSER("cs472") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        REQUIRE(ftpService->sendPASS("hw2ftp") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
        REQUIRE(stat.msg  == "230 Login successful.\r\n");
    }

    SECTION("good username and bad password") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 21) == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        REQUIRE(ftpService->sendUSER("cs472") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        REQUIRE(ftpService->sendPASS("hw2ftpblas") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
    }

    SECTION("bad username and bad password") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 21) == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        REQUIRE(ftpService->sendUSER("badCs47") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        REQUIRE(ftpService->sendPASS("hw2ftpblas") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
    }

    SECTION("empty username and bad password") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 21) == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        REQUIRE(ftpService->sendUSER("") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        REQUIRE(ftpService->sendPASS("hw2ftpblas") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == BAD_SEQUENCE_COMMAND);
        REQUIRE(stat.msg == "503 Login with USER first.\r\n");
    }

    SECTION("good username and empty password") {
        // connect to legit ftp client
        FtpCtrlReply stat;
        auto ftpService = std::make_unique<FtpService>();
        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 21) == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        REQUIRE(ftpService->sendUSER("cs472") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        REQUIRE(ftpService->sendPASS("") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
    }
}


TEST_CASE("FtpService send CWD command", "[FtpService]") {
    SECTION("change to good directory") {
        auto ftpService = std::make_unique<FtpService>();
        connectLegitServer(*ftpService);

        FtpCtrlReply stat;
        REQUIRE(ftpService->sendCWD("..") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == REQUESTED_FILE_ACTION_COMPLETED);
        REQUIRE(stat.msg == "250 Directory successfully changed.\r\n");
    }

    SECTION("change to bad directory") {
        auto ftpService = std::make_unique<FtpService>();
        connectLegitServer(*ftpService);

        FtpCtrlReply stat;
        REQUIRE(ftpService->sendCWD("asas") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == REQUESTED_FILE_ACTION_NOT_TAKEN_FILE_UNAVAILABLE);
        REQUIRE(stat.msg == "550 Failed to change directory.\r\n");
    }
}


TEST_CASE("FtpService send PWD command", "[FtpService]") {
    SECTION("current directory") {
        auto ftpService = std::make_unique<FtpService>();
        connectLegitServer(*ftpService);

        FtpCtrlReply stat;
        REQUIRE(ftpService->sendPWD() == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == PATHNAME_CREATED);
        REQUIRE(stat.msg == "257 \"/home/cs472\" is the current directory\r\n");
    }
}


TEST_CASE("FtpService send QUIT command", "[FtpService]") {
    SECTION("quit") {
        auto ftpService = std::make_unique<FtpService>();
        connectLegitServer(*ftpService);

        FtpCtrlReply stat;
        REQUIRE(ftpService->sendQUIT() == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == SERVICE_CLOSE_CTRL_CONNECTION);
        REQUIRE(stat.msg == "221 Goodbye.\r\n");
    }
}


TEST_CASE("FtpService send PORT command", "[FtpService]") {
    SECTION("port") {
        auto ftpService = std::make_unique<FtpService>();
        connectLegitServer(*ftpService);

        FtpCtrlReply stat;
        REQUIRE(ftpService->sendPORT(8000) == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == COMMAND_OK);
        REQUIRE(stat.msg == "200 PORT command successful. Consider using PASV.\r\n");
    }
}


TEST_CASE("FtpService send LIST command", "[FtpService]") {
    SECTION("list active mode") {
        auto ftpService = std::make_unique<FtpService>();
        connectLegitServer(*ftpService);

        FtpCtrlReply stat;
        REQUIRE(ftpService->sendPORT(30000));
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == COMMAND_OK);
        REQUIRE(stat.msg == "200 PORT command successful. Consider using PASV.\r\n");

        REQUIRE(ftpService->openDataConnect(30000, true));

        REQUIRE(ftpService->sendLIST("") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
        REQUIRE(stat.msg  == "150 Here comes the directory listing.\r\n");

        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
        REQUIRE(stat.msg  == "226 Directory send OK.\r\n");

        std::vector<char> buf;
        REQUIRE(ftpService->readDataReply(buf) == true);
        REQUIRE(buf.size() > 0);
        REQUIRE(ftpService->closeDataConnect() == true);
    }

    SECTION("list passive mode") {
        auto ftpService = std::make_unique<FtpService>();
        connectLegitServer(*ftpService);

        FtpCtrlReply stat;
        REQUIRE(ftpService->sendPASV());
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == ENTERING_PASSIVE_MODE);

        uint16_t passivePort;
        std::string ipAddr;
        FtpService::parsePASVReply(stat.msg, ipAddr, passivePort);
        REQUIRE(ftpService->openDataConnect(passivePort, false));

        REQUIRE(ftpService->sendLIST("") == true);
        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
        REQUIRE(stat.msg  == "150 Here comes the directory listing.\r\n");

        REQUIRE(ftpService->readCtrlReply(stat) == true);
        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
        REQUIRE(stat.msg  == "226 Directory send OK.\r\n");

        std::vector<char> buf;
        REQUIRE(ftpService->readDataReply(buf) == true);
        REQUIRE(buf.size() > 0);
        REQUIRE(ftpService->closeDataConnect() == true);
    }
}
