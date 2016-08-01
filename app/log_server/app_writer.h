#ifndef _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_APP_LOG_SERVER_APP_WRITER_H
#define _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_APP_LOG_SERVER_APP_WRITER_H
#pragma once

#include "net/net_queue.h"
#include "net/net_thread.h"
#include "net/net_logger.h"
#include "net/net_epoll.h"

class CAppWriter : public Thread
{
public:
    CAppWriter();
    ~CAppWriter();
    void SetQueue(CNetQueue *queue) {m_queue = queue;}
    void Run();
    void OnQueue();
private:
    int SwitchToNextFile();
    bool IsSwitchFile();
    bool Write(const char* data, size_t len);
private:
    int m_fd;
    std::string m_path_prefix;
    time_t m_pre_time;
    CNetQueue *m_queue;
    CEpoll *m_epoll;
    char *m_buffer;
    uint32_t m_buffer_size;
};
#endif // _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_APP_LOG_SERVER_APP_WRITER_H
