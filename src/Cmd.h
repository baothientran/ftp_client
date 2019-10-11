#ifndef CMD_H
#define CMD_H


#include <vector>
#include <string>
#include <map>
#include "FtpService.h"


class Command;


class CommandService {
public:
    CommandService(const std::string &hostname, uint16_t port);

    ~CommandService();

    const std::string &hostname() const;

    uint16_t port() const;

    bool serviceAvailable() const;

    void setServiceAvailable(bool available);

    bool passiveMode() const;

    void setPassiveMode(bool passive);

    bool serviceShouldTerminate() const;

    void setServiceShouldTerminate(bool terminate);

    const std::map<std::string, std::unique_ptr<Command>> &commands() const;

    void run();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};


class Command {
public:
    Command(FtpService *ftp, CommandService *cmd);

    virtual ~Command();

    virtual void displayHelp() = 0;

    virtual void execute(const std::vector<std::string> &argvs) = 0;

protected:
    bool openDataConnection();

    bool checkCmdServiceAvailable();

    void getFtpReplyAndCheckTimeout(FtpCtrlReply &reply);

    void getFtpReply(FtpCtrlReply &reply);

    FtpService *ftpService;
    CommandService *cmdService;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};


class HelpCommand : public Command {
public:
    HelpCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class ConnectCommand : public Command {
public:
    ConnectCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class DisconnectCommand : public Command {
public:
    DisconnectCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class QuitCommand : public Command {
public:
    QuitCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class CdCommand : public Command {
public:
    CdCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class PwdCommand : public Command {
public:
    PwdCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class LsCommand : public Command {
public:
    LsCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class GetCommand : public Command {
public:
    GetCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class PutCommand : public Command {
public:
    PutCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


class PassiveCommand : public Command {
public:
    PassiveCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


#endif // CMD_H
