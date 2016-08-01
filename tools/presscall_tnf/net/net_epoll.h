// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-09-24
//

#ifndef _DATA_MTX_WXP_SRC_EPOLL_H
#define _DATA_MTX_WXP_SRC_EPOLL_H
#pragma once

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
#include <errno.h>

#include <iostream>
#include <vector>
#include <set>

class CEpoll
{
public:
    CEpoll(int num);
    ~CEpoll();
    void Add(int fd, bool bRead, bool bWrite);
    void Add(int fd, void *ptr, bool bRead, bool bWrite);
    void Mod(int fd, bool bRead, bool bWrite);
    void Mod(int fd, void *ptr, bool bRead, bool bWrite);
    void Del(int fd, bool is_close = true);
    void Hup(int fd, void *ptr);
    int Size();
    void Wait(std::vector<int> *reads, std::vector<int> *writes, int timeout = -1);
    void Wait(std::vector<void*> *reads, std::vector<void*> *writes, int timeout = -1);

private:


private:
    struct epoll_event *m_events;
    int m_count;
    int m_epoll_fd;
    int m_max_fd;
};
#endif // _DATA_MTX_WXP_SRC_EPOLL_H
