#include <iostream>
#include "catch.hpp"
#include "FtpService.h"


//static void connectLegitServer(FtpService &ftpService) {
//    FtpCtrlReply stat;
//    ftpService.openCtrlConnect("10.246.251.93", 21);
//    ftpService.readCtrlReply(stat);
//    REQUIRE(stat.code == SERVICE_READY);
//    REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

//    ftpService.sendUSER("cs472");
//    ftpService.readCtrlReply(stat);
//    REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
//    REQUIRE(stat.msg == "331 Please specify the password.\r\n");

//    ftpService.sendPASS("hw2ftp");
//    ftpService.readCtrlReply(stat);
//    REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
//    REQUIRE(stat.msg == "230 Login successful.\r\n");
//}


//TEST_CASE("FtpService connect to remote host", "[FtpService]") {
//    SECTION("connect to random non-existence server") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>();
//        REQUIRE(ftpService->openCtrlConnect("adasdadqlwqwndqbwduiqbwdqwd", 123) == false);
//    }

//    SECTION("connect ftp server with wrong port") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>();
//        REQUIRE(ftpService->openCtrlConnect("10.246.251.93", 2121) == false);
//    }

//    SECTION("legit ftp server") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        ftpService->openCtrlConnect("10.246.251.93", 21);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == SERVICE_READY);
//        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");
//    }
//}


//TEST_CASE("FtpService authenticate user to remote host", "[FtpService]") {
//    SECTION("good username and password") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        ftpService->openCtrlConnect("10.246.251.93", 21);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == SERVICE_READY);
//        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

//        ftpService->sendUSER("cs472");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
//        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

//        ftpService->sendPASS("hw2ftp");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
//        REQUIRE(stat.msg  == "230 Login successful.\r\n");
//    }

//    SECTION("good username and bad password") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        ftpService->openCtrlConnect("10.246.251.93", 21);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == SERVICE_READY);
//        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

//        ftpService->sendUSER("cs472");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
//        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

//        ftpService->sendPASS("hw2ftpblas");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
//        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
//    }

//    SECTION("bad username and bad password") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        ftpService->openCtrlConnect("10.246.251.93", 21);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == SERVICE_READY);
//        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

//        ftpService->sendUSER("badCs47");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
//        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

//        ftpService->sendPASS("hw2ftpblas");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
//        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
//    }

//    SECTION("empty username and bad password") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        ftpService->openCtrlConnect("10.246.251.93", 21);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == SERVICE_READY);
//        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

//        ftpService->sendUSER("");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
//        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

//        ftpService->sendPASS("hw2ftpblas");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == BAD_SEQUENCE_COMMAND);
//        REQUIRE(stat.msg == "503 Login with USER first.\r\n");
//    }

//    SECTION("good username and empty password") {
//        // connect to legit ftp client
//        FtpCtrlReply stat;
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        ftpService->openCtrlConnect("10.246.251.93", 21);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == SERVICE_READY);
//        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

//        ftpService->sendUSER("cs472");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
//        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

//        ftpService->sendPASS("");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
//        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
//    }
//}


//TEST_CASE("FtpService send CWD command", "[FtpService]") {
//    SECTION("change to good directory") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendCWD("..");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == REQUESTED_FILE_ACTION_COMPLETED);
//        REQUIRE(stat.msg == "250 Directory successfully changed.\r\n");
//    }

//    SECTION("change to bad directory") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendCWD("asas");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == REQUESTED_FILE_ACTION_NOT_TAKEN_FILE_UNAVAILABLE);
//        REQUIRE(stat.msg == "550 Failed to change directory.\r\n");
//    }
//}


//TEST_CASE("FtpService send PWD command", "[FtpService]") {
//    SECTION("current directory") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendPWD();
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == PATHNAME_CREATED);
//        REQUIRE(stat.msg == "257 \"/home/cs472\" is the current directory\r\n");
//    }
//}


//TEST_CASE("FtpService send QUIT command", "[FtpService]") {
//    SECTION("quit") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendQUIT();
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == SERVICE_CLOSE_CTRL_CONNECTION);
//        REQUIRE(stat.msg == "221 Goodbye.\r\n");
//    }
//}


//TEST_CASE("FtpService send PORT command", "[FtpService]") {
//    SECTION("port") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendPORT(8000);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == COMMAND_OK);
//        REQUIRE(stat.msg == "200 PORT command successful. Consider using PASV.\r\n");
//    }
//}


//TEST_CASE("FtpService send LIST command", "[FtpService]") {
//    SECTION("list active mode") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;

//        ftpService->openDataConnect(30000, true);

//        ftpService->sendPORT(30000);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == COMMAND_OK);
//        REQUIRE(stat.msg == "200 PORT command successful. Consider using PASV.\r\n");

//        ftpService->sendLIST("");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
//        REQUIRE(stat.msg  == "150 Here comes the directory listing.\r\n");

//        std::vector<unsigned char> buf;
//        ftpService->readDataReply(buf);
//        REQUIRE(buf.size() > 0);
//        ftpService->closeDataConnect();

//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
//        REQUIRE(stat.msg  == "226 Directory send OK.\r\n");
//    }

//    SECTION("list passive mode") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendPASV();
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == ENTERING_PASSIVE_MODE);

//        uint16_t passivePort;
//        std::string ipAddr;
//        FtpService::parsePASVReply(stat.msg, ipAddr, passivePort);
//        ftpService->openDataConnect(passivePort, false);

//        ftpService->sendLIST("");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
//        REQUIRE(stat.msg  == "150 Here comes the directory listing.\r\n");

//        std::vector<unsigned char> buf;
//        ftpService->readDataReply(buf);
//        REQUIRE(buf.size() > 0);
//        ftpService->closeDataConnect();

//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
//        REQUIRE(stat.msg  == "226 Directory send OK.\r\n");
//    }
//}


//TEST_CASE("FtpService send STOR command", "[ftpService]") {
//    SECTION("stor active mode") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;

//        ftpService->openDataConnect(30000, true);

//        ftpService->sendPORT(30000);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == COMMAND_OK);
//        REQUIRE(stat.msg == "200 PORT command successful. Consider using PASV.\r\n");

//        ftpService->sendSTOR("test.txt");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
//        REQUIRE(stat.msg  == "150 Ok to send data.\r\n");

//        std::string content = "this is a test content.";
//        std::vector<unsigned char> buf;
//        std::copy(content.begin(), content.end(), std::back_inserter(buf));
//        ftpService->sendDataConnect(buf);
//        ftpService->closeDataConnect();

//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
//        REQUIRE(stat.msg  == "226 Transfer complete.\r\n");
//    }

//    SECTION("stor passive mode") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendPASV();
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == ENTERING_PASSIVE_MODE);

//        uint16_t passivePort;
//        std::string ipAddr;
//        FtpService::parsePASVReply(stat.msg, ipAddr, passivePort);
//        ftpService->openDataConnect(passivePort, false);

//        ftpService->sendSTOR("test.txt");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
//        REQUIRE(stat.msg  == "150 Ok to send data.\r\n");

//        std::string content = "this is a test content.";
//        std::vector<unsigned char> buf;
//        std::copy(content.begin(), content.end(), std::back_inserter(buf));
//        ftpService->sendDataConnect(buf);
//        ftpService->closeDataConnect();

//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
//        REQUIRE(stat.msg  == "226 Transfer complete.\r\n");
//    }
//}


//TEST_CASE("FtpService send RETR command", "[ftpService]") {
//    SECTION("retr active mode") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;

//        ftpService->sendPORT(30000);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == COMMAND_OK);
//        REQUIRE(stat.msg == "200 PORT command successful. Consider using PASV.\r\n");

//        ftpService->openDataConnect(30000, true);

//        ftpService->sendRETR("ftp-rfcs.txt");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
//        REQUIRE(stat.msg  == "150 Opening BINARY mode data connection for ftp-rfcs.txt (321080 bytes).\r\n");

//        std::vector<unsigned char> buf;
//        ftpService->readDataReply(buf);
//        REQUIRE(buf.size() > 0);
//        ftpService->closeDataConnect();

//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
//        REQUIRE(stat.msg  == "226 Transfer complete.\r\n");
//    }

//    SECTION("retr passive mode") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendPASV();
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == ENTERING_PASSIVE_MODE);

//        uint16_t passivePort;
//        std::string ipAddr;
//        FtpService::parsePASVReply(stat.msg, ipAddr, passivePort);
//        ftpService->openDataConnect(passivePort, false);

//        ftpService->sendRETR("ftp-rfcs.txt");
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == FILE_STATUS_OK_OPEN_DATA_CONNECTION);
//        REQUIRE(stat.msg  == "150 Opening BINARY mode data connection for ftp-rfcs.txt (321080 bytes).\r\n");

//        std::vector<unsigned char> buf;
//        ftpService->readDataReply(buf);
//        REQUIRE(buf.size() > 0);
//        ftpService->closeDataConnect();

//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS);
//        REQUIRE(stat.msg  == "226 Transfer complete.\r\n");
//    }
//}


//TEST_CASE("EPRT") {
//    SECTION("retr passive mode") {
//        auto ftpService = std::make_unique<FtpService>(&std::cout);
//        connectLegitServer(*ftpService);

//        FtpCtrlReply stat;
//        ftpService->sendEPRT(IPv4, 30000);
//        ftpService->readCtrlReply(stat);
//        REQUIRE(stat.code == ENTERING_PASSIVE_MODE);
//    }
//}
