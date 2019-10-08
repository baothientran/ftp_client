#ifndef CMD_H
#define CMD_H


#include <vector>
#include <string>
#include "FtpService.h"


class CommandService {
public:
    CommandService();

    bool passiveMode() const;

    bool serviceTerminate() const;

    void run(const std::string &cmd);

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};


class Command {
public:
    Command(FtpService *ftp, CommandService *cmd)
        : ftpService{ftp}, cmdService{cmd}
    {}

    virtual ~Command();

    virtual void execute(const std::vector<std::string> &argvs) = 0;

protected:
    FtpService *ftpService;
    CommandService *cmdService;
};


class AuthCommand : public Command {
public:
    AuthCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void execute(const std::vector<std::string> &argvs) override;
};


class QuitCommand : public Command {
public:
    QuitCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void execute(const std::vector<std::string> &argvs) override;
};


class CdCommand : public Command {
public:
    CdCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void execute(const std::vector<std::string> &argvs) override;
};


class PwdCommand : public Command {
public:
    PwdCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void execute(const std::vector<std::string> &argvs) override;
};


class LsCommand : public Command {
public:
    LsCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void execute(const std::vector<std::string> &argvs) override;
};


class GetCommand : public Command {
public:
    GetCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void execute(const std::vector<std::string> &argvs) override;
};


class PutCommand : public Command {
public:
    PutCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void execute(const std::vector<std::string> &argvs) override;
};


#endif // CMD_H
