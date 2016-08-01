#ifndef _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_NET_CLIENT_WORKER_H
#define _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_NET_CLIENT_WORKER_H
#pragma once

#include "net/net_logger.h"
#include "net/net_client.h"
#include "net/net_thread.h"
#include "net/net_session.h"
#include "net/net_queue.h"
#include "net/net_epoll.h"
#include "common/common_def.h"
#include "longconn/longconn_helper.h"
#include "proto/business_interface.pb.h"
#include "proto/log_interface.pb.h"
#include "reader.h"
#include "query_info.h"
#include "tnf_writer.h"

struct worker_epoll_t
{
    void (*func)(void *priv) ;
    void *priv;
    int fd;
};

struct worker_conf_t
{
    CLogger *logger;
    CNetQueue *reader_recv_queue;
    CNetQueue *reader_send_queue;
    std::string addr;
};

class CWorker : public Thread
{
public:
    CWorker();
    ~CWorker();
    void Run();
    void Init(const worker_conf_t& conf);
    void ClientQueue();
    void ReaderQueue();

    friend class CQueryInfo;
private:
    void CreateRBURequest(uint32_t seq, const std::string& query, std::string *req);
    void CreateLOGRequest(uint32_t seq, const std::string& query, std::string *req);

    void ParseRBUResponse(uint32_t seq, const char* data_buf, uint32_t data_len);
    void ParseLOGResponse(uint32_t seq, const char* data_buf, uint32_t data_len);

private:
    int m_worker_idx;
    //CNetClient *m_client;
    CTNFWriter *m_client;
    CSessionMgr *m_session_mgr;
    CLogger *m_logger;
    LongconnHelper *m_pack;
    CEpoll *m_epoll;
    CNetQueue *m_reader_send_queue;
    CNetQueue *m_reader_recv_queue;
    CNetQueue *m_client_send_queue;
    CNetQueue *m_client_recv_queue;

    char *m_buffer;
    uint32_t m_buffer_size;
    uint32_t m_ip;
    uint16_t m_port;
    uint32_t m_send_num;
    uint32_t m_recv_num;
    uint32_t m_timeout;
    uint32_t m_timeout_num;

};

#endif // _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_NET_CLIENT_WORKER_H
