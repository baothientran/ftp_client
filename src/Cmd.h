#ifndef CMD_H
#define CMD_H


#include <vector>
#include <string>
#include <map>
#include "FtpService.h"


class Command;


/*
 * CommandService class
 * Provide shared state for all user command interfaces, Get user command input,
 * and Run the respective command on behalf of the user
 */
class CommandService {
public:
    CommandService(std::ostream *output, std::istream *input, std::ostream *logger,
                   const std::string &hostname, uint16_t port);

    ~CommandService();

    /*
     * Return the ftp server hostname the client is connected to
     */
    const std::string &hostname() const;

    /*
     * Return the port number of the control connection between this client and the ftp server
     */
    uint16_t port() const;

    /*
     * Check if the ftp server is available to accept ftp command
     */
    bool serviceAvailable() const;

    /*
     * Some commands will manually connect and disconnect ftp server. In those cases, they need to
     * set the ftp server to be unavailable so that other commands that depends on the ftp server will not run
     */
    void setServiceAvailable(bool available);

    /*
     * Check if passive mode for data connection is turned on
     */
    bool passiveMode() const;

    /*
     * Some commands will receive input from user to turn on passive mode. In those cases, they need to
     * set the passsive mode to be true so that other commands that use data connection will know to use the
     * passive mode instead of active mode
     */
    void setPassiveMode(bool passive);

    /*
     * Check if the command service should terminate or not
     */
    bool serviceShouldTerminate() const;

    /*
     * Some commands will receive input from user to quit command service. In those cases, they need to
     * set the terminate to be true to escape the main loop
     */
    void setServiceShouldTerminate(bool terminate);

    /*
     * Get the output stream. It is not neccessary STDOUT. Command interface should
     * write user message to this stream
     */
    std::ostream &output();

    /*
     * Get the input stream. It is not neccessary STDIN. Command interface should
     * get input from this stream
     */
    std::istream &input();

    /*
     * Get the logger stream. It is not neccessary file logger. Command interface should
     * use this stream to log any messages
     */
    std::ostream &logger();

    /*
     * Get all the commands supported by CommandService
     */
    const std::map<std::string, std::unique_ptr<Command>> &commands() const;

    /*
     * The main loop of Command service. Responsible for retrieve user input and
     * run command on behalf of the user
     */
    void run();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};


/*
 * Command interface
 * Define abstract interface for user command
 */
class Command {
public:
    Command(FtpService *ftp, CommandService *cmd);

    virtual ~Command();

    /*
     * Display the usage and syntax of the command
     */
    virtual void displayHelp() = 0;

    /*
     * Execute the command based on user input arguments
     */
    virtual void execute(const std::vector<std::string> &argvs) = 0;

protected:
    /*
     * Helper function to open data connection to the ftp server in active or passive mode.
     * Function returns false if it cannot open data connection
     */
    bool openDataConnection();

    /*
     * Helper function to check if command service is available or not. If not available, it will
     * output the message to the output stream
     */
    bool checkCmdServiceAvailable();

    /*
     * Get control reply from ftp server. If reply code indicates time out, the function
     * will disconnect ftp server
     */
    void getFtpReplyAndCheckTimeout(FtpCtrlReply &reply);

    /*
     * Get control reply without checking time out
     */
    void getFtpReply(FtpCtrlReply &reply);

    FtpService *ftpService;
    CommandService *cmdService;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};


/*
 * HelpCommand class
 * Display the help message for every available user commands
 */
class HelpCommand : public Command {
public:
    HelpCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * ConnectCommand
 * Connect to the ftp server and log in to the server
 */
class ConnectCommand : public Command {
public:
    ConnectCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * DisconnectCommand
 * Disconnect from ftp server. The command will not quit the main program
 */
class DisconnectCommand : public Command {
public:
    DisconnectCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * QuitCommand
 * Disconnect from ftp server and quit the main program
 */
class QuitCommand : public Command {
public:
    QuitCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * CdCommand
 * Change remote directory
 */
class CdCommand : public Command {
public:
    CdCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * PwdCommand
 * Print the current remote directory
 */
class PwdCommand : public Command {
public:
    PwdCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * LsCommand
 * List the info about a file or directory or all the files and directories in the current remote directory
 */
class LsCommand : public Command {
public:
    LsCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * GetCommand
 * Retrieve the remote file and save it to a local file
 */
class GetCommand : public Command {
public:
    GetCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * PutCommand
 * Upload a local file to the ftp server
 */
class PutCommand : public Command {
public:
    PutCommand(FtpService *ftp, CommandService *cmd)
        : Command{ftp, cmd}
    {}

    void displayHelp() override;

    void execute(const std::vector<std::string> &argvs) override;

    static const std::string PROG;
};


/*
 * PassiveCommand
 * Toggle the passive mode for data connection
 */
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
