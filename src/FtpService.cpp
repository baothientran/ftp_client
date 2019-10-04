#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <exception>
#include "FtpService.h"


struct FtpService::Impl {
    bool writeSocketCmd(const std::string &cmd, FtpStatus &status) {
        bool wres = writeSocketEnsure(cmd.c_str(), cmd.size() * sizeof(char));
        if (!wres) {
            status.code = FAIL_WRITE_REQUEST;
            status.msg = "cannot send request to server";
            return false;
        }

        return true;
    }


    void readSocketReply(FtpStatus &status) {
        std::string reply;
        bool rres = readlineSocketEnsure(reply);
        if (!rres) {
            status.code = FAIL_READ_REPLY;
            status.msg  = "cannot read reply from server";
        }
        else {
            FtpStatusCode replyCode = parseReplyCode(reply);
            status.code = replyCode;
            status.msg = reply;
        }
    }


    bool connectHost(const std::string &hostAddr, const std::string &port, int &outSockfd, FtpStatus &status) {
        // get ip address
        int stat;
        addrinfo hint, *ipAddrHdr = nullptr;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_UNSPEC;
        hint.ai_socktype = SOCK_STREAM;
        if ((stat = getaddrinfo(hostAddr.c_str(), port.c_str(), &hint, &ipAddrHdr)) == -1) {
            status.code = CONNECTION_ERROR;
            status.msg  = std::string(gai_strerror(stat));
            return false;
        }

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

            outSockfd = sockfd;
            break;
        }

        // check if any socket open
        if (ipAddr == nullptr) {
            status.code = CONNECTION_ERROR;
            status.msg  = "cannot open socket to connect to host " + hostAddr + " port " + port;
            return false;
        }

        freeaddrinfo(ipAddrHdr);

        return true;
    }


    bool writeSocketEnsure(const void *buf, size_t size) {
        bool res = true;
        size_t writeSofar = 0;
        const char *wbuf = reinterpret_cast<const char *>(buf);
        while (writeSofar < size) {
            auto wn = write(ctrlSockfd, wbuf + writeSofar, size - writeSofar);
            if (wn < 0) {
                res = false;
                break;
            }

            writeSofar += static_cast<size_t>(wn);
        }

        return res;
    }


    bool readlineSocketEnsure(std::string &line) {
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

        return true;
    }


    FtpStatusCode parseReplyCode(const std::string &reply) {
        unsigned replyCode = 0;
        for (size_t i = 0; i < reply.size(); ++i) {
            if (reply[i] >= '0' && reply[i] <= '9')
                replyCode = replyCode * 10 + static_cast<unsigned>(reply[i]-'0');
            else
                break;
        }

        return static_cast<FtpStatusCode>(replyCode);
    }


    int ctrlSockfd;
    std::string hostAddr;
    std::string ctrlPort;
};


FtpService::FtpService() {
    _impl = std::make_unique<Impl>();
}


FtpService::~FtpService() {
    if (_impl->ctrlSockfd != -1)
        close(_impl->ctrlSockfd);
}


FtpStatus FtpService::connectHost(const std::string &hostAddr, const std::string &port) {
    // open ctrl port
    FtpStatus status;
    int ctrlSockfd;
    if (!_impl->connectHost(hostAddr, port, ctrlSockfd, status))
        return status;

    // open data port
    _impl->ctrlSockfd   = ctrlSockfd;
    _impl->hostAddr     = hostAddr;
    _impl->ctrlPort     = port;
    _impl->readSocketReply(status);

    return status;
}


FtpStatus FtpService::user(const std::string &username) {
    std::string cmd = "USER " + username + "\r\n";
    FtpStatus status;
    if (_impl->writeSocketCmd(cmd, status))
        _impl->readSocketReply(status);

    return status;
}


FtpStatus FtpService::pass(const std::string &password) {
    std::string cmd = "PASS " + password + "\r\n";
    FtpStatus status;
    if (_impl->writeSocketCmd(cmd, status))
        _impl->readSocketReply(status);

    return status;
}


FtpStatus FtpService::cwd(const std::string &path) {
    std::string cmd = "CWD " + path + "\r\n";
    FtpStatus status;
    if (_impl->writeSocketCmd(cmd, status))
        _impl->readSocketReply(status);

    return status;
}


FtpStatus FtpService::pwd() {
    std::string cmd = "PWD\r\n";
    FtpStatus status;
    if (_impl->writeSocketCmd(cmd, status))
        _impl->readSocketReply(status);

    return status;
}


FtpStatus FtpService::quit() {
    std::string cmd = "QUIT\r\n";
    FtpStatus status;
    if (_impl->writeSocketCmd(cmd, status))
        _impl->readSocketReply(status);

    return status;
}


FtpStatus FtpService::list(const std::string &path) {
    std::string cmd = "LIST\r\n";
    FtpStatus status;
    if (_impl->writeSocketCmd(cmd, status))
        _impl->readSocketReply(status);

    return status;
}
