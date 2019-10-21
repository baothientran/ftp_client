#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include "Cmd.h"
#include "Utility.h"

/************************************************************
 * CommandService class definition
 ************************************************************/
struct CommandService::Impl {
    std::vector<std::string> parseCommandLine(const std::string &input) {
        std::vector<std::string> argvs;
        std::string argv;
        bool quote = false;
        for (auto c = input.begin(); c != input.end(); ++c) {
            if (*c == '\"') {
                quote = !quote;
                continue;
            }

            if (quote || *c != ' ')
                argv += *c;
            else if (*c == ' ') {
                argvs.push_back(argv);
                argv = "";
            }
        }

        argvs.push_back(argv);
        return argvs;
    }


    bool passiveMode;
    bool serviceAvailable;
    bool shouldTerminate;
    std::string hostname;
    uint16_t port;
    std::unique_ptr<FtpService> ftpService;
    std::map<std::string, std::unique_ptr<Command>> commands;
    std::ostream *output;
    std::istream *input;
    std::ostream *logger;
};


CommandService::CommandService(std::ostream *output, std::istream *input, std::ostream *logger,
                               const std::string &hostname, uint16_t port)
{
    _impl = std::make_unique<Impl>();
    _impl->passiveMode = false;
    _impl->serviceAvailable = false;
    _impl->shouldTerminate = false;
    _impl->hostname = hostname;
    _impl->port = port;
    _impl->output = output;
    _impl->input = input;
    _impl->logger = logger;
    _impl->ftpService  = std::make_unique<FtpService>(logger);

    // initialize commands
    _impl->commands.insert({      HelpCommand::PROG, std::make_unique<HelpCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({   ConnectCommand::PROG, std::make_unique<ConnectCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({DisconnectCommand::PROG, std::make_unique<DisconnectCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({      QuitCommand::PROG, std::make_unique<QuitCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({        CdCommand::PROG, std::make_unique<CdCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({       PwdCommand::PROG, std::make_unique<PwdCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({        LsCommand::PROG, std::make_unique<LsCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({       GetCommand::PROG, std::make_unique<GetCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({       PutCommand::PROG, std::make_unique<PutCommand>(_impl->ftpService.get(), this)});
    _impl->commands.insert({   PassiveCommand::PROG, std::make_unique<PassiveCommand>(_impl->ftpService.get(), this)});
}


CommandService::~CommandService() {}


void CommandService::setPassiveMode(bool passive) {
    _impl->passiveMode = passive;
}


bool CommandService::serviceShouldTerminate() const {
    return _impl->shouldTerminate;
}


void CommandService::setServiceShouldTerminate(bool terminate) {
    _impl->shouldTerminate = terminate;
}


std::ostream &CommandService::output() {
    return *_impl->output;
}


std::istream &CommandService::input() {
    return *_impl->input;
}


std::ostream &CommandService::logger() {
    return *_impl->logger;
}


const std::map<std::string, std::unique_ptr<Command>> &CommandService::commands() const {
    return _impl->commands;
}


bool CommandService::passiveMode() const {
    return _impl->passiveMode;
}


const std::string &CommandService::hostname() const {
    return _impl->hostname;
}


uint16_t CommandService::port() const {
    return _impl->port;
}


bool CommandService::serviceAvailable() const {
    return _impl->serviceAvailable;
}


void CommandService::setServiceAvailable(bool available) {
    _impl->serviceAvailable = available;
    if (!available)
        _impl->ftpService->closeCtrlConnect();
}


void CommandService::run() {
    std::string userInput = ConnectCommand::PROG;

    while (true) {
        if (!userInput.empty()) {
            // run command
            std::vector<std::string> argvs = _impl->parseCommandLine(userInput);
            auto cmd = _impl->commands.find(argvs[0]);
            if (cmd == _impl->commands.end()) {
                *_impl->output << "Unrecognized command.\n" <<
                                  "Type help for the list of supported commands.\n" <<
                                  "Type help <Space> <Command> <Enter> for a specific command usage\n";
            }
            else {
                try {
                    cmd->second->execute(argvs);
                } catch (SocketException e) {
                    *_impl->output << "Oops error occur: " << e.what() << "\n"
                                   << "Close ftp connection\n";
                    setServiceAvailable(false);
                }
            }
        }

        // should terminate serivce
        if (_impl->shouldTerminate) {
            *_impl->output << "Quit program\n";
            break;
        }

        // get input from user
        *_impl->output << "> ";
        getline(*_impl->input, userInput);
    }
}


/************************************************************
 * Command class definition
 ************************************************************/
struct Command::Impl {
    bool openPassiveDataConnection() {
        static const int RETRIES = 15;

        auto cmdService = cmd->cmdService;
        auto &output = cmdService->output();

        uint16_t passivePort;
        FtpCtrlReply reply;
        int retries = 1;
        while (retries <= RETRIES) {
            if (cmd->ftpService->netProtocol() == IPv6) {
                cmd->ftpService->sendEPSV(false, IPv6);
                cmd->getFtpReplyAndCheckTimeout(reply);
                if (reply.code != ENTERING_EXTENDED_PASSIVE_MODE)
                    break;

                std::string ipAddr;
                FtpService::parseEPSVReply(reply.msg, passivePort);
            }
            else {
                cmd->ftpService->sendPASV();
                cmd->getFtpReplyAndCheckTimeout(reply);
                if (reply.code != ENTERING_PASSIVE_MODE)
                    break;

                std::string ipAddr;
                FtpService::parsePASVReply(reply.msg, ipAddr, passivePort);
            }

            try {
                cmd->ftpService->openDataConnect(passivePort, false);

                break;
            } catch (SocketException e) {
                output << "Failed to open data connection on local. Retries: " << retries << "/" << RETRIES << "\n";
                ++retries;
            }
        }

        return retries <= RETRIES && (reply.code == ENTERING_PASSIVE_MODE || reply.code == ENTERING_EXTENDED_PASSIVE_MODE);
    }


    bool openActiveDataConnection() {
        auto cmdService = cmd->cmdService;
        auto &output = cmdService->output();

        FtpCtrlReply reply;
        uint16_t port = FtpService::USABLE_PORT_MAX;
        while (port >= FtpService::USABLE_PORT_MIN) {
            if (cmd->ftpService->netProtocol() == IPv6)
                cmd->ftpService->sendEPRT(IPv6, port);
            else
                cmd->ftpService->sendPORT(port);

            cmd->getFtpReplyAndCheckTimeout(reply);
            if (reply.code != COMMAND_OK)
                break;

            try {
                cmd->ftpService->openDataConnect(port, true);
                break;
            } catch (SocketException e) {
                --port;
                output << "Failed to open data connection on local. Retry another port number: " << port << "\n";
            }
        }

        return port >= FtpService::USABLE_PORT_MIN && reply.code == COMMAND_OK;
    }

    Command *cmd;
};


Command::Command(FtpService *ftp, CommandService *cmd)
    : ftpService{ftp}, cmdService{cmd}
{
    _impl = std::make_unique<Impl>();
    _impl->cmd = this;
}


Command::~Command()
{}


bool Command::openDataConnection() {
    if (cmdService->passiveMode())
        return _impl->openPassiveDataConnection();

    return _impl->openActiveDataConnection();
}


bool Command::checkCmdServiceAvailable() {
    auto &output = cmdService->output();
    bool available = cmdService->serviceAvailable();
    if (!available)
        output << "Service not available. Consider to use connect command\n";

    return available;
}


void Command::getFtpReplyAndCheckTimeout(FtpCtrlReply &reply) {
    getFtpReply(reply);
    cmdService->setServiceAvailable(reply.code != SERVICE_UNAVAILABLE);
}


void Command::getFtpReply(FtpCtrlReply &reply) {
    auto &output = cmdService->output();
    ftpService->readCtrlReply(reply);
    output << reply.msg;
}


/************************************************************
 * HelpCommand class definition
 ************************************************************/
const std::string HelpCommand::PROG = "help";


void HelpCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Display the help message for command\n";
    output << "Syntax: help [<Space> <Command>] <Enter>\n";
}


void HelpCommand::execute(const std::vector<std::string> &argvs) {
    auto &output = cmdService->output();
    const auto &commands = cmdService->commands();

    // print the list of command if command option not provided
    if (argvs.size() == 1) {
        for (const auto &cmd : commands)
            output << std::setw(7 + cmd.first.size()) << std::left << cmd.first;

        output << "\n";
        return;
    }

    // display the help for single command
    const auto &cmdProg = argvs[1];
    const auto &cmd     = commands.find(cmdProg);
    if (cmd == commands.end()) {
        output << "Command " << cmdProg << " not supported\n";
        return;
    }

    cmd->second->displayHelp();
}


/************************************************************
 * ConnectCommand class definition
 ************************************************************/
const std::string ConnectCommand::PROG = "connect";


void ConnectCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Connect to the ftp server\n";
    output << "Syntax: connect <Enter>\n";
}


void ConnectCommand::execute(const std::vector<std::string> &) {
    auto &output = cmdService->output();
    auto &input  = cmdService->input();
    FtpCtrlReply reply;
    if (!cmdService->serviceAvailable()) {
        const auto &hostname = cmdService->hostname();
        uint16_t port = cmdService->port();

        // connect service
        ftpService->openCtrlConnect(hostname, port);
        getFtpReply(reply);
        if (reply.code != SERVICE_READY) {
            cmdService->setServiceAvailable(false);
            return;
        }

        cmdService->setServiceAvailable(true);
    }

    // get username
    std::string user;
    output << "User: ";
    getline(input, user);
    ftpService->sendUSER(user);
    getFtpReplyAndCheckTimeout(reply);
    if (reply.code != USER_OK_PASSWORD_NEEDED) {
        return;
    }

    // get password
    std::string pass;
    output << "Password: ";
    getline(input, pass);
    ftpService->sendPASS(pass);
    getFtpReplyAndCheckTimeout(reply);
}


/************************************************************
 * DisconnectCommand class definition
 ************************************************************/
const std::string DisconnectCommand::PROG = "disconnect";


void DisconnectCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Log out of the ftp server. The program will not exit after the command\n";
    output << "Syntax: disconnect <Enter>\n";
}


void DisconnectCommand::execute(const std::vector<std::string> &) {
    if (!checkCmdServiceAvailable())
        return;

    FtpCtrlReply reply;
    ftpService->sendQUIT();
    getFtpReply(reply);
    cmdService->setServiceAvailable(false);
}


/************************************************************
 * QuitCommand class definition
 ************************************************************/
const std::string QuitCommand::PROG = "quit";


void QuitCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Log out of the ftp server. The program will exit after the command\n";
    output << "Syntax: quit <Enter>\n";
}


void QuitCommand::execute(const std::vector<std::string> &) {
    cmdService->setServiceShouldTerminate(true);

    if (cmdService->serviceAvailable()) {
        FtpCtrlReply reply;
        ftpService->sendQUIT();
        getFtpReply(reply);
        cmdService->setServiceAvailable(false);
    }
}


/************************************************************
 * CdCommand class definition
 ************************************************************/
const std::string CdCommand::PROG = "cd";


void CdCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Change to the remote directory\n";
    output << "Syntax: cd <Space> <Remote Directory> <Enter>\n";
}


void CdCommand::execute(const std::vector<std::string> &argvs) {
    if (!checkCmdServiceAvailable())
        return;

    if (argvs.size() < 2) {
        displayHelp();
        return;
    }
    const std::string &remotePath = argvs[1];

    FtpCtrlReply reply;
    ftpService->sendCWD(remotePath);
    getFtpReplyAndCheckTimeout(reply);
}


/************************************************************
 * PwdCommand class definition
 ************************************************************/
const std::string PwdCommand::PROG = "pwd";


void PwdCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Print working remote directory\n";
    output << "Syntax: pwd <Enter>\n";
}


void PwdCommand::execute(const std::vector<std::string> &) {
    if (!checkCmdServiceAvailable())
        return;

    FtpCtrlReply reply;
    ftpService->sendPWD();
    getFtpReplyAndCheckTimeout(reply);
}


/************************************************************
 * LsCommand class definition
 ************************************************************/
const std::string LsCommand::PROG = "ls";


void LsCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : List info about the remote file or all the files in the remote directory\n";
    output << "Syntax: ls [<Space> <Remote File or Directory>] <Enter>\n";
}


void LsCommand::execute(const std::vector<std::string> &argvs) {
    if (!checkCmdServiceAvailable())
        return;

    auto &output = cmdService->output();

    std::string remotePath = "";
    if (argvs.size() >= 2)
        remotePath = argvs[1];

    // open data connection
    FtpCtrlReply reply;
    if (!openDataConnection())
        return;

    // send LIST cmd
    ftpService->sendLIST(remotePath);
    getFtpReplyAndCheckTimeout(reply);
    if (reply.code != FILE_STATUS_OK_OPEN_DATA_CONNECTION) {
        ftpService->closeDataConnect();
        return;
    }

    // read data from data connection
    std::vector<unsigned char> buf;
    ftpService->readDataReply(buf);
    ftpService->closeDataConnect();

    // read server reply from ctrl connection
    getFtpReplyAndCheckTimeout(reply);
    if (reply.code == CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS)
        output.write(reinterpret_cast<const char *>(buf.data()), buf.size());
}


/************************************************************
 * GetCommand class definition
 ************************************************************/
const std::string GetCommand::PROG = "get";


void GetCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Download the remote file and save it into the local file. Local file is optional and default to be the name of remote file\n";
    output << "Syntax: get <Space> <Remote File> [<Space> <Local File>] <Enter>\n";
}


void GetCommand::execute(const std::vector<std::string> &argvs) {
    if (!checkCmdServiceAvailable())
        return;

    if (argvs.size() < 2) {
        displayHelp();
        return;
    }

    auto &output = cmdService->output();

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
    output << "Local path: "  << localPath  << "\n";
    output << "Remote path: " << remotePath << "\n";

    // open data connection
    FtpCtrlReply reply;
    if (!openDataConnection())
        return;

    // send RETR cmd
    ftpService->sendRETR(remotePath);
    getFtpReplyAndCheckTimeout(reply);
    if (reply.code != FILE_STATUS_OK_OPEN_DATA_CONNECTION) {
        ftpService->closeDataConnect();
        return;
    }

    // read data from data connection
    std::vector<Byte> buf;
    ftpService->readDataReply(buf);
    ftpService->closeDataConnect();

    // read server reply from ctrl connection
    getFtpReplyAndCheckTimeout(reply);
    if (reply.code != CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS)
        return;

    // write data to file
    std::ofstream file(localPath, std::ios::out | std::ios::binary);
    if (!file) {
        output << "Cannot open local path: " << localPath << "\n";
        return;
    }
    file.write(reinterpret_cast<const char *>(buf.data()), buf.size());
}


/************************************************************
 * PutCommand class definition
 ************************************************************/
const std::string PutCommand::PROG = "put";


void PutCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Upload the local file to the ftp server and save as the remote file name. Remote file is optional and default to be local file name\n";
    output << "Syntax: put <Space> <Local File> [<Space> <Remote File>] <Enter>\n";
}


void PutCommand::execute(const std::vector<std::string> &argvs) {
    if (!checkCmdServiceAvailable())
        return;

    if (argvs.size() < 2) {
        displayHelp();
        return;
    }

    auto &output = cmdService->output();

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
    output << "Local path: " << localPath << "\n";
    output << "Remote path: " << remotePath << "\n";

    // check if local file exists
    if (!isRegularFile(localPath)) {
        output << "Local path: " << localPath << " is not a regular file\n";
        return;
    }

    // open data connection
    FtpCtrlReply reply;
    if (!openDataConnection())
        return;

    // send STOR cmd
    ftpService->sendSTOR(remotePath);
    getFtpReplyAndCheckTimeout(reply);
    if (reply.code != FILE_STATUS_OK_OPEN_DATA_CONNECTION) {
        ftpService->closeDataConnect();
        return;
    }

    // read file here and send data to the server
    std::ifstream file(localPath);
    std::vector<Byte> buf;
    std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(buf));
    ftpService->sendDataConnect(buf);
    ftpService->closeDataConnect();

    // read server reply from ctrl connection
    getFtpReplyAndCheckTimeout(reply);
}


/************************************************************
 * PassiveCommand class definition
 ************************************************************/
const std::string PassiveCommand::PROG = "passive";


void PassiveCommand::displayHelp() {
    auto &output = cmdService->output();
    output << "Usage : Toggle passive mode for data connection\n";
    output << "Syntax: passive <Enter>\n";
}


void PassiveCommand::execute(const std::vector<std::string> &) {
    auto &output = cmdService->output();
    bool passive = cmdService->passiveMode();
    cmdService->setPassiveMode(!passive);
    if (cmdService->passiveMode())
        output << "Passive mode on\n";
    else
        output << "Passive mode off\n";
}

