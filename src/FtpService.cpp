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
#include "Ultility.h"
#include "FtpService.h"


static const int      BUFFER_SIZE_MIN  = 2048;
static const int      LISTEN_QUEUE_MAX = 100;

struct FtpService::Impl {
    bool listenHost(const std::string &port, int &outSockfd) {
        // get ip address
        int stat;
        addrinfo hint, *ipAddrHdr = nullptr;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_flags = AI_PASSIVE;
        if ((stat = getaddrinfo(nullptr, port.c_str(), &hint, &ipAddrHdr)) == -1)
            return false;

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
                close(sockfd);
                continue;
            }

            outSockfd = sockfd;
            break;
        }

        freeaddrinfo(ipAddrHdr);

        return ipAddr != nullptr;
    }


    bool connectHost(const std::string &host, const std::string &port) {
        // get ip address
        int stat;
        addrinfo hint, *ipAddrHdr = nullptr;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = SOCK_STREAM;
        if ((stat = getaddrinfo(host.c_str(), port.c_str(), &hint, &ipAddrHdr)) == -1)
            return false;

        // loop through all possible ip address to open socket
        addrinfo *ipAddr;
        for (ipAddr = ipAddrHdr; ipAddr; ipAddr = ipAddr->ai_next) {
            int sockfd = socket(ipAddr->ai_family, ipAddr->ai_socktype, ipAddr->ai_protocol);
            if (sockfd == -1)
                continue;

            if (connect(sockfd, ipAddr->ai_addr, ipAddr->ai_addrlen) == -1) {
                close(sockfd);
                continue;
            }

            // retrieve local ip address will be used for PORT and EPRT cmd
            char localIp[INET6_ADDRSTRLEN];
            int ipver = ipAddr->ai_family;
            if (ipver == AF_INET) {
                sockaddr_in addr;
                socklen_t len = sizeof(sockaddr_in);
                getsockname(ctrlSockfd, reinterpret_cast<sockaddr*>(&addr), &len);
                inet_ntop(ipver, &(addr.sin_addr), localIp, sizeof(localIp));
            }
            else {
                sockaddr_in6 addr;
                socklen_t len = sizeof(sockaddr_in6);
                getsockname(ctrlSockfd, reinterpret_cast<sockaddr*>(&addr), &len);
                inet_ntop(ipver, &(addr.sin6_addr), localIp, sizeof(localIp));
            }

            ctrlSockfd  = sockfd;
            ctrlPort    = port;
            ipVersion   = ipAddr->ai_family;
            localIpAddr = std::string(localIp);

            break;
        }

        freeaddrinfo(ipAddrHdr);

        return ipAddr != nullptr;
    }


    bool writeCtrlCmd(const std::string &cmd) {
        bool res = true;
        size_t writeSofar = 0;
        const char *wbuf = reinterpret_cast<const char *>(cmd.c_str());
        while (writeSofar < cmd.size()) {
            auto wn = write(ctrlSockfd, wbuf + writeSofar, cmd.size() - writeSofar);
            if (wn < 0) {
                res = false;
                break;
            }

            writeSofar += static_cast<size_t>(wn);
        }

        return res;
    }


    bool readCtrlReply(std::string &line) {
        char ch;
        ssize_t rn;
        bool res = true;

        line = "";
        do {
            rn = read(ctrlSockfd, &ch, 1);
            if (rn == -1) {
                res = false;
                break;
            }
            if (rn == 0)
                break;

            line += ch;
        } while (ch != '\n');

        return res;
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


    bool readDataReply(int sockfd, std::vector<char> &buf) {
        char rb[BUFFER_SIZE_MIN];
        buf.reserve(BUFFER_SIZE_MIN);

        int rn;
        while ((rn = readDataEnsure(sockfd, rb, BUFFER_SIZE_MIN)) > 0) {
            std::copy(rb, rb + rn, std::back_inserter(buf));
        }

        return rn >= 0;
    }


    int readDataEnsure(int sockfd, char *buf, int size) {
        unsigned char *rbuf = reinterpret_cast<unsigned char *>(buf);
        int readSoFar = 0;
        while (readSoFar < size) {
            auto rn = read(sockfd, rbuf + readSoFar, static_cast<unsigned long>(size - readSoFar));
            if (rn == -1) {
                readSoFar = -1;
                break;
            }
            if (rn == 0)
                break;

            readSoFar += rn;
        }

        return readSoFar;
    }


    int ctrlSockfd;
    int dataSockfd;
    int ipVersion;
    std::string localIpAddr;
    std::string ctrlPort;
};


FtpService::FtpService() {
    _impl = std::make_unique<Impl>();
}


FtpService::~FtpService() {
    if (_impl->ctrlSockfd != -1)
        close(_impl->ctrlSockfd);
    if (_impl->dataSockfd != -1)
        close(_impl->dataSockfd);
}


bool FtpService::openCtrlConnect(const std::string &hostname, uint16_t port) {
    return _impl->connectHost(hostname, std::to_string(port));
}


bool FtpService::readCtrlReply(FtpCtrlReply &reply) {
    if (!_impl->readCtrlReply(reply.msg))
        return false;

    _impl->parseCtrlReplyCode(reply.msg, reply.code);
    return true;
}


bool FtpService::closeCtrlConnect() {
    if (close(_impl->ctrlSockfd) != 0)
        return false;

    _impl->ctrlSockfd  = -1;
    _impl->ipVersion   = -1;
    _impl->localIpAddr = "";
    _impl->ctrlPort    = "";

    return true;
}


bool FtpService::openDataConnect(uint16_t port) {
    int dataSockfd;
    if (!_impl->listenHost(std::to_string(port), dataSockfd))
        return false;

    _impl->dataSockfd = dataSockfd;
    return true;
}


bool FtpService::readDataReply(std::vector<char> &buf) {
    sockaddr_storage peerAddr;
    socklen_t len = sizeof(peerAddr);
    int sockfd = accept(_impl->dataSockfd, reinterpret_cast<sockaddr *>(&peerAddr), &len);
    if (sockfd == -1)
        return false;

    bool resRead   = _impl->readDataReply(sockfd, buf);
    bool resClose  = close(sockfd) == 0;
    return resRead && resClose;
}


bool FtpService::closeDataConnect() {
    if (close(_impl->dataSockfd) != 0)
        return false;

    _impl->dataSockfd = -1;
    return true;
}


bool FtpService::sendUSER(const std::string &username) {
    std::string cmd = "USER " + username + "\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendPASS(const std::string &password) {
    std::string cmd = "PASS " + password + "\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendCWD(const std::string &path) {
    std::string cmd = "CWD " + path + "\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendPWD() {
    std::string cmd = "PWD\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendQUIT() {
    std::string cmd = "QUIT\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendLIST(const std::string &path) {
    std::string space = path.empty() ? "" : " ";
    std::string cmd = "LIST" + space + path + "\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendPASV() {
    std::string cmd = "PASV\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendEPSV(const std::string &networkProtocol) {
    std::string space = networkProtocol.empty() ? "" : " ";
    std::string cmd = "EPSV" + space + networkProtocol + "\r\n";
    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendPORT(uint16_t port) {
    std::string cmd = "PORT ";

    // add local ip address to cmd
    auto ipn = splitString(_impl->localIpAddr, ".");
    for (const auto &n : ipn)
        cmd += n + ",";

    // add first 8 bits and second 8 bits of port number to cmd
    std::string p1 = std::to_string(port >> 8);
    std::string p2 = std::to_string(port & 0x00FF);
    cmd += p1 + "," + p2 + "\r\n";

    return _impl->writeCtrlCmd(cmd);
}


bool FtpService::sendEPRT(uint16_t port) {
    std::string cmd = "EPRT ";
    if (_impl->ipVersion == AF_INET)
        cmd += "|1|" + _impl->localIpAddr + "|" + std::to_string(port) + "|";
    else
        cmd += "|2|" + _impl->localIpAddr + "|" + std::to_string(port) + "|";

    return _impl->writeCtrlCmd(cmd);
}


void FtpService::parsePASVReply(const std::string &pasvReply, uint16_t &port) {

}


void FtpService::parseEPSVReply(const std::string &epsvReply, uint16_t &port) {

}
