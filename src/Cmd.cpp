#include <iostream>
#include <fstream>
#include "Cmd.h"


static bool openPassiveDataConnection(FtpService &ftpService) {
    FtpCtrlReply reply;
    ftpService.sendPASV();
    ftpService.readCtrlReply(reply);
    if (reply.code != ENTERING_PASSIVE_MODE) {
        std::cout << reply.msg << "\n";
        return false;
    }
    std::cout << reply.msg << "\n";

    uint16_t passivePort;
    std::string ipAddr;
    FtpService::parsePASVReply(reply.msg, ipAddr, passivePort);
    return ftpService.openDataConnect(passivePort, false);
}


static bool openActiveDataConnection(FtpService &ftpService) {
    FtpCtrlReply reply;
    uint16_t p;
    for (p = FtpService::USABLE_PORT_MAX; p >= FtpService::USABLE_PORT_MIN; --p) {
        ftpService.sendPORT(p);
        ftpService.readCtrlReply(reply);
        if (reply.code != COMMAND_OK)
            continue;

        if (!ftpService.openDataConnect(p, true))
            continue;

        std::cout << reply.msg << "\n";
        return true;
    }

    std::cout << "Cannot open data connection\n";
    return false;
}


static bool openDataConnection(FtpService &ftpService, CommandService &cmdService) {
    if (cmdService.passiveMode())
        return openPassiveDataConnection(ftpService);

    return openActiveDataConnection(ftpService);
}


Command::~Command()
{}


void AuthCommand::execute(const std::vector<std::string> &) {
    // hardcode authentication for now

}


void QuitCommand::execute(const std::vector<std::string> &) {
    FtpCtrlReply reply;
    ftpService->sendQUIT();
    ftpService->readCtrlReply(reply);

    std::cout << reply.msg << "\n";
}


void CdCommand::execute(const std::vector<std::string> &argvs) {
    if (argvs.size() < 2) {
        std::cout << "Syntax error. Required path name\n";
        return;
    }
    const std::string &path = argvs[1];

    FtpCtrlReply reply;
    ftpService->sendCWD(path);
    ftpService->readCtrlReply(reply);

    std::cout << reply.msg << "\n";
}


void PwdCommand::execute(const std::vector<std::string> &) {
    FtpCtrlReply reply;
    ftpService->sendPWD();
    ftpService->readCtrlReply(reply);

    std::cout << reply.msg << "\n";
}


void LsCommand::execute(const std::vector<std::string> &argvs) {
    std::string path = "";
    if (argvs.size() >= 2)
        path = argvs[1];

    // open data connection
    FtpCtrlReply reply;
    openDataConnection(*ftpService, *cmdService);

    // send LIST cmd
    ftpService->sendLIST(path);
    ftpService->readCtrlReply(reply);
    if (reply.code != FILE_STATUS_OK_OPEN_DATA_CONNECTION) {
        std::cout << reply.msg << "\n";
        return;
    }

    // read data from data connection
    std::vector<unsigned char> buf;
    ftpService->readDataReply(buf);
    ftpService->closeDataConnect();

    // read server reply from ctrl connection
    ftpService->readCtrlReply(reply);
    if (reply.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS)
        std::cout << buf.data() << "\n";

    std::cout << reply.msg << "\n";
}


void GetCommand::execute(const std::vector<std::string> &argvs) {
    if (argvs.size() < 2) {
        std::cout << "Syntax error. Required path name\n";
        return;
    }

    // get local and remote path
    std::string localPath;
    std::string remotePath;
    if (argvs.size() == 2) {
        remotePath = argvs[1];
        localPath  = remotePath;
    }
    else {
        remotePath  = argvs[1];
        localPath   = argvs[2];
    }
    std::cout << "local path: " << localPath << "\n";
    std::cout << "remote path: " << remotePath << "\n";

    // try to open local file
    std::ofstream file(localPath);
    if (!file) {
        std::cout << "cannot open local path: " << localPath << "\n";
        return;
    }

    // open data connection
    FtpCtrlReply reply;
    openDataConnection(*ftpService, *cmdService);

    // send RETR cmd
    ftpService->sendRETR(remotePath);
    ftpService->readCtrlReply(reply);
    if (reply.code != FILE_STATUS_OK_OPEN_DATA_CONNECTION) {
        std::cout << reply.msg << "\n";
        return;
    }

    // read data from data connection
    std::vector<unsigned char> buf;
    ftpService->readDataReply(buf);
    ftpService->closeDataConnect();

    // read server reply from ctrl connection
    ftpService->readCtrlReply(reply);
    if (reply.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS)
        file << buf.data();

    std::cout << reply.msg << "\n";
}


void PutCommand::execute(const std::vector<std::string> &argvs) {
    if (argvs.size() < 2) {
        std::cout << "Syntax error. Required path name\n";
        return;
    }

    // get local path and remote path
    std::string localPath  = "";
    std::string remotePath = "";
    if (argvs.size() == 2) {
        localPath  = argvs[1];
        remotePath = localPath;
    }
    else {
        localPath  = argvs[1];
        remotePath = argvs[2];
    }
    std::cout << "local path: " << localPath << "\n";
    std::cout << "remote path: " << remotePath << "\n";

    // try to open local file
    std::ifstream file(localPath);
    if (!file) {
        std::cout << "cannot open local path: " << localPath << "\n";
        return;
    }

    // open data connection
    FtpCtrlReply reply;
    openDataConnection(*ftpService, *cmdService);

    // send STOR cmd
    ftpService->sendSTOR(remotePath);
    ftpService->readCtrlReply(reply);
    if (reply.code != FILE_STATUS_OK_OPEN_DATA_CONNECTION) {
        std::cout << reply.msg << "\n";
        return;
    }

    // read file here and send data to the server
    std::vector<unsigned char> buf;
    std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(buf));
    ftpService->sendDataConnect(buf);
    ftpService->closeDataConnect();

    // read server reply from ctrl connection
    ftpService->readCtrlReply(reply);

    std::cout << reply.msg << "\n";
}
