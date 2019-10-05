#ifndef FTPSERVICE_H
#define FTPSERVICE_H

#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <limits>


enum FtpCode {
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
    REQUESTED_ACTION_NOT_TAKEN_FILENAME_NOT_ALLOWED = 553
};


struct FtpCtrlReply {
    FtpCode code;
    std::string msg;
};


class FtpService
{
public:
    FtpService();

    FtpService(const FtpService &) = delete;

    FtpService &operator=(const FtpService &) = delete;

    ~FtpService();

    bool openDataConnect(uint16_t port);

    bool readDataReply(std::vector<char> &buf);

    bool closeDataConnect();

    bool openCtrlConnect(const std::string &hostname, uint16_t port);

    bool readCtrlReply(FtpCtrlReply &status);

    bool closeCtrlConnect();

    bool sendUSER(const std::string &sendUSER);

    bool sendPASS(const std::string &sendPASS);

    bool sendCWD(const std::string &path);

    bool sendPWD();

    bool sendLIST(const std::string &path);

    bool sendQUIT();

    bool sendPASV();

    bool sendEPSV(const std::string &networkProtocol);

    bool sendPORT(uint16_t port);

    bool sendEPRT(uint16_t port);

    static void parsePASVReply(const std::string &pasvReply, uint16_t &port);

    static void parseEPSVReply(const std::string &epsvReply, uint16_t &port);

    static const uint32_t USABLE_PORT_MIN  = 1024;

    static const uint32_t USABLE_PORT_MAX  = std::numeric_limits<uint16_t>::max();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

#endif // FTPSERVICE_H
