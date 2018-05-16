#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "ConnectionPool.h"

namespace emc{

class TCPConnection : public Connection {
public:
    TCPConnection(std::string ip, unsigned short port) : ip_ (ip), 
    port_(port),
    sockfd_(-1) {
        // create socket
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    };

    ~TCPConnection(){
        if (sockfd_ != -1) {
            int rc = ::close(sockfd_);
            if (rc) {
                std::cout << "close(" << sockfd_ << "):(" << errno << ")" << std::endl;
            } 
        }
    }

    void connect() {

        // create sockaddr_in
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port_);
        servaddr.sin_addr.s_addr = inet_addr(ip_.c_str());

        // Connect
        if (::connect(sockfd_, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
            std::cout << "Connect error, ip: " << ip_ << ", port: " << port_ << std::endl;
            return;
        }
        std::cout << "Connect success";
    }

private:
    std::string ip_;
    unsigned short port_;
    int sockfd_;
};

class TCPConnectionFactory : public ConnectionFactory {
public:
    TCPConnectionFactory(std::string ip, unsigned short port) 
    : ip_(ip),
    port_(port) {};

    std::shared_ptr<Connection> create() {

        // Create connection
        std::shared_ptr<TCPConnection> conn(new TCPConnection(ip_, port_));

        // Connect
        conn->connect();

        return std::dynamic_pointer_cast<Connection>(conn);
    }

private:
    std::string ip_;
    unsigned short port_;
};

};

#endif /* TCPCONNECTION_H_ */
