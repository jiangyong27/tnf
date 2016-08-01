#ifndef _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_SRC_NET_NET_SOCKET_H
#define _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_SRC_NET_NET_SOCKET_H
#pragma once

#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

class CSocket
{
public:
    CSocket(int socket);
    CSocket();
    virtual ~CSocket();
    void SetReuseAddr(int flag = 1);
    void SetNoblock(int flag = 1);
    void SetNodeley(int flat = 1);
    void SetKeepAlive();
    int SendBuf(char *data_buf, uint32_t data_len);
    int RecvBuf(char *data_buf, uint32_t data_len);
    int GetSock() { return m_socket;}
protected:
    int m_socket;
};

class CTCPSocket : public CSocket
{
public:
    CTCPSocket();
    ~CTCPSocket();
    int Listen(uint32_t ip, uint16_t port, int max = 20);
    int Connect(uint32_t ip, uint16_t port);
    CSocket* Accept();
protected:
    int Bind(uint32_t ip, uint32_t port);
protected:
};

#endif // _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_SRC_NET_NET_SOCKET_H
