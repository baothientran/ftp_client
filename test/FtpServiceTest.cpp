#include <iostream>
#include "catch.hpp"
#include "FtpService.h"


TEST_CASE("FtpService connect to remote host", "[FtpService]") {
    SECTION("connect to random non-existence server") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("adasdadqlwqwndqbwduiqbwdqwd", "abasbasdaba");
        REQUIRE(stat.code == CONNECTION_ERROR);
        REQUIRE(stat.msg == "cannot open socket to connect to host adasdadqlwqwndqbwduiqbwdqwd port abasbasdaba");
    }

    SECTION("connect ftp server with wrong port") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "2121");
        REQUIRE(stat.code == CONNECTION_ERROR);
        REQUIRE(stat.msg == "cannot open socket to connect to host 10.246.251.93 port 2121");
    }

    SECTION("legit ftp server") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");
    }
}


TEST_CASE("FtpService authenticate user to remote host", "[FtpService]") {
    SECTION("good username and password") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftp");
        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
        REQUIRE(stat.msg  == "230 Login successful.\r\n");
    }

    SECTION("good username and bad password") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftpblas");
        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
    }

    SECTION("bad username and bad password") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("badCs47");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftpblas");
        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
    }

    SECTION("empty username and bad password") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftpblas");
        REQUIRE(stat.code == BAD_SEQUENCE_COMMAND);
        REQUIRE(stat.msg == "503 Login with USER first.\r\n");
    }

    SECTION("good username and empty password") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("");
        REQUIRE(stat.code == USER_NOT_LOGGED_IN);
        REQUIRE(stat.msg == "530 Login incorrect.\r\n");
    }
}


TEST_CASE("FtpService send CWD command", "[FtpService]") {
    SECTION("change to good directory") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftp");
        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
        REQUIRE(stat.msg == "230 Login successful.\r\n");

        stat = ftpService->cwd("..");
        REQUIRE(stat.code == REQUESTED_FILE_ACTION_COMPLETED);
        REQUIRE(stat.msg == "250 Directory successfully changed.\r\n");
    }

    SECTION("change to bad directory") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftp");
        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
        REQUIRE(stat.msg == "230 Login successful.\r\n");

        stat = ftpService->cwd("asas");
        REQUIRE(stat.code == REQUESTED_FILE_ACTION_NOT_TAKEN_FILE_UNAVAILABLE);
        REQUIRE(stat.msg == "550 Failed to change directory.\r\n");
    }
}


TEST_CASE("FtpService send PWD command", "[FtpService]") {
    SECTION("current directory") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftp");
        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
        REQUIRE(stat.msg == "230 Login successful.\r\n");

        stat = ftpService->pwd();
        REQUIRE(stat.code == PATHNAME_CREATED);
        REQUIRE(stat.msg == "257 \"/home/cs472\" is the current directory\r\n");
    }
}


TEST_CASE("FtpService send QUIT command", "[FtpService]") {
    SECTION("quit") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftp");
        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
        REQUIRE(stat.msg == "230 Login successful.\r\n");

        stat = ftpService->quit();
        REQUIRE(stat.code == SERVICE_CLOSE_CTRL_CONNECTION);
        REQUIRE(stat.msg == "221 Goodbye.\r\n");
    }
}


TEST_CASE("FtpService send LIST command", "[FtpService]") {
    SECTION("list") {
        // connect to legit ftp client
        auto ftpService = std::make_unique<FtpService>();
        auto stat = ftpService->connectHost("10.246.251.93", "21");
        REQUIRE(stat.code == SERVICE_READY);
        REQUIRE(stat.msg == "220 Welcome to CS472 FTP Server\r\n");

        stat = ftpService->user("cs472");
        REQUIRE(stat.code == USER_OK_PASSWORD_NEEDED);
        REQUIRE(stat.msg == "331 Please specify the password.\r\n");

        stat = ftpService->pass("hw2ftp");
        REQUIRE(stat.code == USER_LOGGED_IN_PROCCEED);
        REQUIRE(stat.msg == "230 Login successful.\r\n");

        stat = ftpService->list("");
        REQUIRE(stat.code == SERVICE_CLOSE_CTRL_CONNECTION);
        REQUIRE(stat.msg == "221 Goodbye.\r\n");
    }
}
