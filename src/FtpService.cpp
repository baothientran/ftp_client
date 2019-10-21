#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <exception>
#include <vector>
#include <bitset>
#include <algorithm>
#include <iomanip>
#include "Utility.h"
#include "FtpService.h"


static const int BUFFER_SIZE_MIN  = 2048;
static const int LISTEN_QUEUE_MAX = 100;

struct FtpService::Impl {
    std::ostream &getLogger() {
        std::time_t now = std::time(nullptr);
        std::tm tm = *std::localtime(&now);
        return *logger << std::put_time(&tm, "%c %Z") << ": ";
    }


    void acceptHost(int listenSockfd, int &sockfd) {
        sockaddr_storage peerAddr;
        socklen_t len = sizeof(peerAddr);
        sockfd = accept(listenSockfd, reinterpret_cast<sockaddr *>(&peerAddr), &len);
        if (sockfd == -1)
            throw SocketException();
    }


    void listenHost(const std::string &port, int &outSockfd) {
        // get ip address
        int stat;
        addrinfo hint, *ipAddrHdr = nullptr;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = netProtocol == IPv4 ? AF_INET : AF_INET6;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_flags = AI_PASSIVE;
        if ((stat = getaddrinfo(nullptr, port.c_str(), &hint, &ipAddrHdr)) == -1)
            throw SocketException();

        // loop through all possible ip address to open socket
        addrinfo *ipAddr;
        for (ipAddr = ipAddrHdr; ipAddr; ipAddr = ipAddr->ai_next) {
            int sockfd = socket(ipAddr->ai_family, ipAddr->ai_socktype, ipAddr->ai_protocol);
            if (sockfd == -1)
                continue;

            int reuse = 1;
            bool socketUnusable = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1 ||
                                  bind(sockfd, ipAddr->ai_addr, ipAddr->ai_addrlen)                 == -1 ||
                                  listen(sockfd, LISTEN_QUEUE_MAX)                                  == -1;

            if (socketUnusable) {
                closeSocket(sockfd);
                continue;
            }

            outSockfd = sockfd;

            break;
        }

        if (ipAddr == nullptr) {
            freeaddrinfo(ipAddrHdr);
            throw SocketException();
        }

        freeaddrinfo(ipAddrHdr);
    }


    void connectHost(const std::string &host, const std::string &port, int &sockfd, NetProtocol &protocol) {
        // get ip address
        int stat;
        addrinfo hint, *ipAddrHdr = nullptr;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = SOCK_STREAM;
        if ((stat = getaddrinfo(host.c_str(), port.c_str(), &hint, &ipAddrHdr)) == -1)
            throw SocketException();

        // loop through all possible ip address to open socket
        addrinfo *ipAddr;
        for (ipAddr = ipAddrHdr; ipAddr; ipAddr = ipAddr->ai_next) {
            int fd = socket(ipAddr->ai_family, ipAddr->ai_socktype, ipAddr->ai_protocol);
            if (fd == -1)
                continue;

            if (connect(fd, ipAddr->ai_addr, ipAddr->ai_addrlen) == -1) {
                closeSocket(fd);
                continue;
            }

            sockfd = fd;
            protocol = ipAddr->ai_family == AF_INET ? IPv4 : IPv6;

            break;
        }

        if (ipAddr == nullptr) {
            freeaddrinfo(ipAddrHdr);
            throw SocketException();
        }

        freeaddrinfo(ipAddrHdr);
    }


    void writeAndLogCtrlCmd(const std::string &cmd) {
        writeSockEnsure(ctrlSockfd, reinterpret_cast<const Byte *>(cmd.c_str()), cmd.size());
        getLogger() << "Sent " << cmd;
    }


    void readDataReply(int sockfd, std::vector<Byte> &buf) {
        Byte rb[BUFFER_SIZE_MIN];
        buf.reserve(BUFFER_SIZE_MIN);

        int rn;
        while ((rn = readSockEnsure(sockfd, rb, BUFFER_SIZE_MIN)) > 0) {
            std::copy(rb, rb + rn, std::back_inserter(buf));
        }
    }


    void writeSockEnsure(int sockfd, const Byte *buf, size_t size) {
        size_t writeSofar = 0;
        while (writeSofar < size) {
            auto wn = send(sockfd, buf + writeSofar, size - writeSofar, MSG_NOSIGNAL);
            if (wn < 0)
                throw SocketException();

            writeSofar += static_cast<size_t>(wn);
        }
    }


    void readLineSockEnsure(int sockfd, std::string &line) {
        char ch;
        ssize_t rn;
        line = "";
        do {
            rn = read(sockfd, &ch, 1);
            if (rn == -1)
                throw SocketException();

            if (rn == 0)
                break;

            line += ch;
        } while (ch != '\n');
    }


    int readSockEnsure(int sockfd, Byte *buf, int size) {
        int readSoFar = 0;
        while (readSoFar < size) {
            auto rn = read(sockfd, buf + readSoFar, static_cast<unsigned long>(size - readSoFar));
            if (rn == -1)
                throw SocketException();

            if (rn == 0)
                break;

            readSoFar += rn;
        }

        return readSoFar;
    }


    void closeSocket(int sockfd) {
        if (close(sockfd) != 0)
            throw SocketException();
    }


    void getIpAddress(NetProtocol protocol, int sockfd, std::string &ipAddress) {
        // retrieve local ip address will be used for PORT and EPRT cmd
        char localIp[INET6_ADDRSTRLEN];
        if (protocol == IPv4) {
            sockaddr_in addr;
            socklen_t len = sizeof(sockaddr_in);
            getsockname(sockfd, reinterpret_cast<sockaddr*>(&addr), &len);
            inet_ntop(AF_INET, &(addr.sin_addr), localIp, sizeof(localIp));
        }
        else {
            sockaddr_in6 addr;
            socklen_t len = sizeof(sockaddr_in6);
            getsockname(ctrlSockfd, reinterpret_cast<sockaddr*>(&addr), &len);
            inet_ntop(AF_INET6, &(addr.sin6_addr), localIp, sizeof(localIp));
        }

        ipAddress = std::string(localIp);
    }


    void parseCtrlReplyCode(const std::string &reply, FtpCode &replyCode) {
        unsigned code = 0;
        for (size_t i = 0; i < reply.size(); ++i) {
            if (reply[i] >= '0' && reply[i] <= '9')
                code = code * 10 + static_cast<unsigned>(reply[i]-'0');
            else
                break;
        }

        replyCode = static_cast<FtpCode>(code);
    }


    int ctrlSockfd;
    int dataSockfd;
    bool activeDataMode;
    NetProtocol netProtocol;
    std::string hostname;
    std::string localIpAddr;
    std::ostream *logger;
};


FtpService::FtpService(std::ostream *logger) {
    _impl = std::make_unique<Impl>();
    _impl->ctrlSockfd = -1;
    _impl->dataSockfd = -1;
    _impl->activeDataMode = true;
    _impl->netProtocol = UNSPECIFIED;
    _impl->hostname = "";
    _impl->localIpAddr = "";
    _impl->logger = logger;
}


FtpService::~FtpService() {
    if (_impl->ctrlSockfd != -1)
        close(_impl->ctrlSockfd);
    if (_impl->dataSockfd != -1)
        close(_impl->dataSockfd);
}


NetProtocol FtpService::netProtocol() const {
    return _impl->netProtocol;
}


void FtpService::openCtrlConnect(const std::string &hostname, uint16_t port) {
    int sockfd;
    NetProtocol protocol;
    _impl->connectHost(hostname, std::to_string(port), sockfd, protocol);

    _impl->ctrlSockfd    = sockfd;
    _impl->hostname      = hostname;
    _impl->netProtocol   = protocol;
    _impl->getIpAddress(protocol, sockfd, _impl->localIpAddr);

    // log open connection
    _impl->getLogger() << "Opened control connection with host " << _impl->hostname << " port " << port << std::endl;
}


void FtpService::readCtrlReply(FtpCtrlReply &reply) {
    _impl->readLineSockEnsure(_impl->ctrlSockfd, reply.msg);
    _impl->parseCtrlReplyCode(reply.msg, reply.code);

    // log ctrl reply from server
    _impl->getLogger() << "Received " << reply.msg << std::flush;
}


void FtpService::closeCtrlConnect() {
    if (_impl->ctrlSockfd == -1)
        return;

    _impl->closeSocket(_impl->ctrlSockfd);
    _impl->ctrlSockfd  = -1;
    _impl->netProtocol = UNSPECIFIED;
    _impl->localIpAddr = "";

    // log close connection
    _impl->getLogger() << "Closed control connection with host " << _impl->hostname << std::endl;
}


void FtpService::openDataConnect(uint16_t port, bool active) {
    int dataSockfd;
    NetProtocol protocol;
    if (!active) {
        _impl->connectHost(_impl->hostname, std::to_string(port), dataSockfd, protocol);

        // log open passive data connection
        _impl->getLogger() << "Opened passive data connection with host " << _impl->hostname << " port " << port << std::endl;
    }
    else {
        _impl->listenHost(std::to_string(port), dataSockfd);

        // log open active data connection
        _impl->getLogger() << "Opened active data connection with host " << _impl->hostname << " port " << port << std::endl;
    }

    _impl->dataSockfd = dataSockfd;
    _impl->activeDataMode = active;
}


void FtpService::sendDataConnect(const std::vector<Byte> &buf) {
    if (_impl->activeDataMode) {
        int sockfd;
        _impl->acceptHost(_impl->dataSockfd, sockfd);
        _impl->writeSockEnsure(sockfd, buf.data(), buf.size());
        _impl->closeSocket(sockfd);
    }
    else
        _impl->writeSockEnsure(_impl->dataSockfd, buf.data(), buf.size());

    // log data sent through data connection
    _impl->getLogger() << "Sent " << buf.size() << " bytes to host " << _impl->hostname << " through data connection" << std::endl;
}


void FtpService::readDataReply(std::vector<Byte> &buf) {
    if (_impl->activeDataMode) {
        int sockfd;
        _impl->acceptHost(_impl->dataSockfd, sockfd);
        _impl->readDataReply(sockfd, buf);
        _impl->closeSocket(sockfd);
    }
    else
        _impl->readDataReply(_impl->dataSockfd, buf);

    // log data received through data connection
    _impl->getLogger() << "Received " << buf.size() << " bytes from host " << _impl->hostname << " through data connection" << std::endl;
}


void FtpService::closeDataConnect() {
    if (_impl->dataSockfd == -1)
        return;

    _impl->closeSocket(_impl->dataSockfd);
    _impl->dataSockfd     = -1;
    _impl->activeDataMode = false;

    // log data received through data connection
    _impl->getLogger() << "Closed data connection with host " << _impl->hostname << std::endl;
}


void FtpService::sendUSER(const std::string &username) {
    std::string cmd = "USER " + username + "\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendPASS(const std::string &password) {
    std::string cmd = "PASS " + password + "\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendCWD(const std::string &path) {
    std::string cmd = "CWD " + path + "\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendPWD() {
    std::string cmd = "PWD\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendQUIT() {
    std::string cmd = "QUIT\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendLIST(const std::string &path) {
    std::string space = path.empty() ? "" : " ";
    std::string cmd = "LIST" + space + path + "\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendPASV() {
    std::string cmd = "PASV\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendEPSV(bool argALL, NetProtocol netProtocol) {
    std::string args;
    if (argALL)
        args = " ALL";
    else if (netProtocol == IPv4)
        args = " 1";
    else if (netProtocol == IPv6)
        args = " 2";
    else
        args = "";

    std::string cmd = "EPSV" + args + "\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendPORT(uint16_t port) {
    std::string cmd = "PORT ";

    // add local ip address to cmd
    auto ipn = splitString(_impl->localIpAddr, ".");
    for (const auto &n : ipn)
        cmd += n + ",";

    // add first 8 bits and second 8 bits of port number to cmd
    std::string p1 = std::to_string(port >> 8);
    std::string p2 = std::to_string(port & 0x00FF);
    cmd += p1 + "," + p2 + "\r\n";

    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendEPRT(NetProtocol netProtocol, uint16_t port) {
    std::string cmd = "EPRT ";
    if (netProtocol == IPv4)
        cmd += "|1|" + _impl->localIpAddr + "|" + std::to_string(port) + "|\r\n";
    else if (netProtocol == IPv6)
        cmd += "|2|" + _impl->localIpAddr + "|" + std::to_string(port) + "|\r\n";
    else
        return;

    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendRETR(const std::string &filePath) {
    std::string cmd = "RETR " + filePath + "\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::sendSTOR(const std::string &filePath) {
    std::string cmd = "STOR " + filePath + "\r\n";
    _impl->writeAndLogCtrlCmd(cmd);
}


void FtpService::parsePASVReply(const std::string &pasvReply, std::string &ipAddr, uint16_t &port) {
    std::string ipAddrPort;
    bool beginParse = false;
    for (auto c = pasvReply.rbegin(); c != pasvReply.rend(); ++c) {
        if (*c == '(')
            break;

        if (beginParse)
            ipAddrPort += *c;

        if (*c == ')')
            beginParse = true;
    }

    std::reverse(ipAddrPort.begin(), ipAddrPort.end());
    std::vector<std::string> nums = splitString(ipAddrPort, ",");

    // parse ip
    ipAddr = joinString(nums.begin(), nums.begin() + 4, ".");

    // parse port
    uint16_t p1 = static_cast<uint16_t>(atoi(nums[4].c_str()));
    uint16_t p2 = static_cast<uint16_t>(atoi(nums[5].c_str()));
    port = (p1 << 8 & 0xFFFF) | p2;
}


void FtpService::parseEPSVReply(const std::string &epsvReply, uint16_t &port) {
    std::string portStr;
    bool beginParse = false;
    for (auto c = epsvReply.rbegin(); c != epsvReply.rend(); ++c) {
        if (*c == '(')
            break;

        if (beginParse && (*c >= '0' && *c <= '9'))
            portStr += *c;

        if (*c == ')')
            beginParse = true;
    }

    std::reverse(portStr.begin(), portStr.end());
    port = static_cast<uint16_t>(atoi(portStr.c_str()));
}


SocketException::~SocketException() {}


const char *SocketException::what() const noexcept { return strerror(errno); }

