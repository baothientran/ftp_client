#ifndef FTPSERVICE_H
#define FTPSERVICE_H

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <limits>
#include <exception>


using Byte = unsigned char;


enum FtpCode {
    // RFC 959 reply code
    COMMAND_OK = 200,
    COMMAND_NOT_RECOGNIZED = 500,
    COMMAND_ARGS_NOT_RECOGNIZED = 501,
    COMMAND_NOT_IMPLEMENTED_SUPERFLOUS = 202,
    COMMAND_NOT_IMPLEMENTED = 502,
    BAD_SEQUENCE_COMMAND = 503,
    COMMAND_NOT_IMPLEMENTED_FOR_ARGS = 504,
    RESTARTER_MARKER_REPLY = 110,
    SYSTEM_STATUS = 211,
    DIRECTORY_STATUS = 212,
    FILE_STATUS = 213,
    HELP_MESSAGE = 214,
    NAME_SYSTEM_TYPE = 215,

    SERVICE_DELAY = 120,
    SERVICE_READY = 220,
    SERVICE_CLOSE_CTRL_CONNECTION = 221,
    SERVICE_UNAVAILABLE = 421,
    DATA_CONNECTION_OPEN_TRANSFER_STARTING = 125,
    DATA_CONNECTION_OPEN_NO_TRANSFER_IN_PROGRESS = 225,
    CANNOT_OPEN_DATA_CONNECTION = 425,
    CLOSE_DATA_CONNECTION_REQUEST_FILE_ACTION_SUCCESS = 226,
    CONNECTION_CLOSE_TRANSFER_ABORT = 426,
    ENTERING_PASSIVE_MODE = 227,

    USER_LOGGED_IN_PROCCEED = 230,
    USER_NOT_LOGGED_IN = 530,
    USER_OK_PASSWORD_NEEDED = 331,
    ACCT_NEEDED_FOR_LOGGED_IN = 332,
    ACCT_NEEDED_FOR_STORING_FILE = 532,
    FILE_STATUS_OK_OPEN_DATA_CONNECTION = 150,
    REQUESTED_FILE_ACTION_COMPLETED = 250,
    PATHNAME_CREATED = 257,
    REQUESTED_FILE_ACTION_PENDING_FOR_FURTHER_INFO = 350,
    REQUESTED_FILE_ACTION_NOT_TAKEN_FILE_TEMP_UNAVAILABLE = 450,
    REQUESTED_FILE_ACTION_NOT_TAKEN_FILE_UNAVAILABLE = 550,
    REQUESTED_ACTION_ABORTED_LOCAL_ERROR_PROCESSING = 451,
    REQUESTED_ACTION_ABORTED_PAGE_TYPE_UNKNOWN = 551,
    REQUESTED_ACTION_NOT_TAKEN_INSUFFICIENT_STORAGE = 452,
    REQUESTED_ACTION_ABORTED_EXCEEDED_STORAGE_ALLOCATION = 552,
    REQUESTED_ACTION_NOT_TAKEN_FILENAME_NOT_ALLOWED = 553,

    // RFC 2428 reply code
    NETWORK_PROTOCOL_NOT_SUPPORTED = 522,
    ENTERING_EXTENDED_PASSIVE_MODE = 229,
};


enum NetProtocol {
    UNSPECIFIED = 0,
    IPv4 = 1,
    IPv6 = 2,
};


class SocketException : public std::exception {
public:
    SocketException() = default;

    ~SocketException() override;

    const char *what() const noexcept override;
};


struct FtpCtrlReply {
    FtpCode code;
    std::string msg;
};


class FtpService
{
public:
    FtpService(std::ostream *log);

    FtpService(const FtpService &) = delete;

    FtpService &operator=(const FtpService &) = delete;

    ~FtpService();

    NetProtocol netProtocol() const;

    void openDataConnect(uint16_t port, bool active);

    void sendDataConnect(const std::vector<Byte> &buf);

    void readDataReply(std::vector<Byte> &buf);

    void closeDataConnect();

    void openCtrlConnect(const std::string &hostname, uint16_t port);

    void readCtrlReply(FtpCtrlReply &reply);

    void closeCtrlConnect();

    void sendUSER(const std::string &user);

    void sendPASS(const std::string &password);

    void sendCWD(const std::string &path);

    void sendPWD();

    void sendLIST(const std::string &path);

    void sendQUIT();

    void sendPASV();

    void sendEPSV(bool argALL, NetProtocol netProtocol);

    void sendPORT(uint16_t port);

    void sendEPRT(NetProtocol netProtocol, uint16_t port);

    void sendRETR(const std::string &filePath);

    void sendSTOR(const std::string &filePath);

    static void parsePASVReply(const std::string &pasvReply, std::string &ipAddr, uint16_t &port);

    static void parseEPSVReply(const std::string &epsvReply, uint16_t &port);

    static const uint16_t USABLE_PORT_MIN  = 1024;

    static const uint16_t USABLE_PORT_MAX  = std::numeric_limits<uint16_t>::max();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

#endif // FTPSERVICE_H
