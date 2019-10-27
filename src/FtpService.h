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


/*
 * SocketException
 * The exception will be thrown if the ftp service cannot open socket, or error when read from
 * and write to the socket
 */
class SocketException : public std::exception {
public:
    SocketException() = default;

    ~SocketException() override;

    const char *what() const noexcept override;
};


/*
 * FtpCtrlReply struct
 * Store the control reply and the reply code from ftp server
 */
struct FtpCtrlReply {
    FtpCode code;
    std::string msg;
};


/*
 * FtpService class
 * The class provide low level interface to interact with ftp server, such as
 * open control connection with ftp server, open data connection with ftp server,
 * send control command to ftp server, send data to the ftp server through data connection,
 * and retrieve reply back from the server. FtpService is not responsible for the order of
 * Ftp commands sent to the ftp server defined in RFC
 */
class FtpService
{
public:
    FtpService(std::ostream *log);

    FtpService(const FtpService &) = delete;

    FtpService &operator=(const FtpService &) = delete;

    ~FtpService();

    /*
     * Get the protocol used in the ftp connection: IPv4 or IPv6
     */
    NetProtocol netProtocol() const;

    /*
     * Open data connection in active or passive mode. If passive mode is chosen,
     * the port parameter will be ignored
     */
    void openDataConnect(uint16_t port, bool active);

    /*
     * Send the buffer of bytes to the server through data connection
     */
    void sendDataConnect(const std::vector<Byte> &buf);

    /*
     * Read the data back from the ftp server through data connection
     */
    void readDataReply(std::vector<Byte> &buf);

    /*
     * Close the data connection with the fpt server
     */
    void closeDataConnect();

    /*
     * Open control connection with the server
     */
    void openCtrlConnect(const std::string &hostname, uint16_t port);

    /*
     * Read the control reply from server
     */
    void readCtrlReply(FtpCtrlReply &reply);

    /*
     * Close the control connection with the server
     */
    void closeCtrlConnect();

    /*
     * Send USER command to the ftp server
     */
    void sendUSER(const std::string &user);

    /*
     * Send PASS command to the ftp server
     */
    void sendPASS(const std::string &password);

    /*
     * Send CWD command to the ftp server
     */
    void sendCWD(const std::string &path);

    /*
     * Send PWD command to the ftp server
     */
    void sendPWD();

    /*
     * Send LIST command to the ftp server
     */
    void sendLIST(const std::string &path);

    /*
     * Send QUIT command to the ftp server
     */
    void sendQUIT();

    /*
     * Send PASV command to the ftp server
     */
    void sendPASV();

    /*
     * Send EPSV command to the ftp server
     */
    void sendEPSV(bool argALL, NetProtocol netProtocol);

    /*
     * Send PORT command to the ftp server
     */
    void sendPORT(uint16_t port);

    /*
     * Send EPRT command to the ftp server
     */
    void sendEPRT(NetProtocol netProtocol, uint16_t port);

    /*
     * Send RETR command to the ftp server
     */
    void sendRETR(const std::string &filePath);

    /*
     * Send STOR command to the ftp server
     */
    void sendSTOR(const std::string &filePath);

    /*
     * Parse the PASV reply to get the port number and ip address for data connection
     */
    static void parsePASVReply(const std::string &pasvReply, std::string &ipAddr, uint16_t &port);

    /*
     * Parse the EPSV reply to get the port number for data connection
     */
    static void parseEPSVReply(const std::string &epsvReply, uint16_t &port);

    static const uint16_t USABLE_PORT_MIN  = 1024;

    static const uint16_t USABLE_PORT_MAX  = std::numeric_limits<uint16_t>::max();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

#endif // FTPSERVICE_H
