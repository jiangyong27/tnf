#ifndef _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_TNF_WORKER_H
#define _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_TNF_WORKER_H
#pragma once

#include "net/net_queue.h"
#include "net/net_logger.h"
#include "net/net_check.h"
#include "net/net_api.h"
#include "net/net_conf.h"
#include "net/net_thread.h"
#include "net/net_epoll.h"
#include "net/net_session.h"

#include "tnf_writer.h"
#include "tnf_reader.h"
#include "tnf_def.h"

const int   MAX_MQ_NUM = 10;
enum {
    QUEUE_TYPE_READER = 1,
    QUEUE_TYPE_WRITER = 2
};

typedef void (*disp_func) (void *);
struct SMQInfo{
    CNetQueue *mq;
    disp_func func;
    void *priv;
    bool is_active;
};

class CTNFWorker : public Thread
{
public:
    CTNFWorker();
    virtual ~CTNFWorker();
    int Init(const std::string &conf_file, CLogger* logger = NULL);
    int LoadHandler(const std::string& so_file);
    int AddMQDispatch(CNetQueue* queue, disp_func func);

protected:
    void Run();

protected:
    std::string m_conf_file;
    CSessionMgr *m_session_mgr;
    CNetQueue *m_writer_queue;
    CNetQueue *m_reader_queue;
    CEpoll *m_epoll;
    CLogger *m_logger;
    uint32_t m_buffer_size;
    uint32_t m_idx;
    uint32_t m_queue_timeout;

    SMQInfo m_mqs[MAX_MQ_NUM];
    uint32_t m_mqs_num;
    char *m_buffer;
};
#endif // _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_TNF_WORKER_H
