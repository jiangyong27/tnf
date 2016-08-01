#ifndef _DATA_COMMON_READER_H
#define _DATA_COMMON_READER_H
#pragma once
#include <algorithm>
#include <memory>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "net/net_logger.h"
#include "net/net_thread.h"
#include "net/net_queue.h"
#include "net/net_session.h"
#include "net/net_epoll.h"
#include "net/net_client.h"
#include "net/net_grid.h"
#include "common/stater.h"
#include "common/datetime.h"
#include "common/file.h"
#include "common/string.h"
#include "common/queue_t.h"
#include "common/common_def.h"
#include "longconn/longconn_helper.h"
#include "proto/business_interface.pb.h"

typedef CQueueT<std::string> reader_queue_t;

struct reader_conf_t {
    std::string input_dir;
    CNetQueue* send_queue;
    CNetQueue* recv_queue;
    CLogger* logger;
    uint32_t max_qps;
    uint32_t ip;
    uint16_t port;
};
struct server_result_t
{
    uint32_t retcode;
    uint32_t cost;
    uint32_t result_num;
};

class CReader : public Thread
{
public:
    CReader();
    ~CReader();
    void Init(const reader_conf_t& conf);
    virtual void Run();
    void Send();
    void Recv();

private:
    uint32_t m_send_num;
    uint32_t m_recv_num;
    uint32_t m_timeout_num;
    uint32_t m_zero_num;
    uint32_t m_buffer_size;
    uint32_t m_max_qps;
    uint32_t m_ip;
    uint16_t m_port;
    uint32_t m_threshold1;
    uint32_t m_threshold2;
    uint32_t m_threshold3;
    uint32_t m_threshold1_num;
    uint32_t m_threshold2_num;
    uint32_t m_threshold3_num;

    uint64_t m_total_cost;
    std::string m_input_dir;
    char *m_buffer;

    CSessionMgr *m_session_mgr;
    CNetQueue *m_send_queue;
    CNetQueue *m_recv_queue;
    CEpoll *m_epoll;
    CLogger *m_logger;
    LongconnHelper *m_pack;
    std::vector<std::string> m_query;
};

#endif // _DATA_COMMON_SEARCH_IN_PREPROCESS_SRC_PREPROCESS_PREPROCESS_H
