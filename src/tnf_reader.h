#ifndef _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_TNF_RECV_H
#define _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_TNF_RECV_H
#pragma once
#include <string>
#include <map>
#include <list>
#include <vector>
#include <queue>
#include <iostream>

#include "net/net_queue.h"
#include "net/net_logger.h"
#include "net/net_check.h"
#include "net/net_api.h"
#include "net/net_conf.h"
#include "net/net_thread.h"
#include "net/net_epoll.h"
#include "net/net_session.h"

#include "tnf_def.h"

const uint8_t kConnTypeListen = 0;
const uint8_t kConnTypeSocket = 1;
const uint8_t kConnTypeQueue  = 2;

struct reader_req_t
{
    uint32_t ip;
    uint16_t port;
    uint32_t conn_id;
    uint32_t seq;
    struct timeval create_time;
};

struct reader_rsp_t
{
    uint32_t ip;
    uint16_t port;
    uint32_t conn_id;
    uint32_t seq;
    struct timeval create_time;
};

struct reader_conn_t
{
    char *send_buf;
    char *recv_buf;
    uint32_t send_pos;
    uint32_t recv_pos;
    uint32_t send_len;
    uint32_t recv_len;
    uint32_t conn_id;

    int fd;
    time_t access_time;
    uint8_t type;
    void *ptr;

    void Reset();
    reader_conn_t();
    ~reader_conn_t();
};

class CTNFReader : public Thread
{
public:
    CTNFReader();
    ~CTNFReader();
    void Run();
    int Init(const std::string& conf_path, CLogger* logger = NULL);
    int AddQueue(CNetQueue* recv, CNetQueue* send);
    void Reload() {m_reload = 1;}
    int LoadConf();
private:

private:
    void OnCheck();
    void OnAccept();
    void OnRecv(reader_conn_t* conn);
    void OnSend(reader_conn_t* conn);
    void OnClose(reader_conn_t* conn);
    void OnWorker(CNetQueue* conn);
    void InWorker(const char* data_buf, uint32_t data_len, uint32_t conn_id);

protected:
    char *m_buffer;
    uint32_t m_buffer_size;
private:
    std::string m_conf;
    uint32_t m_server_ip;
    uint32_t m_conn_timeout;
    uint16_t m_server_port;
    uint32_t m_max_recv_size;
    uint32_t m_max_send_size;
    uint32_t m_max_conn_num;
    uint32_t m_use_http;
    uint32_t m_use_keepalive;

    uint32_t m_send_num;
    uint32_t m_recv_num;
    uint64_t m_total_cost;
    uint64_t m_queue_cost;
    uint64_t m_total_net_cost;

    uint32_t m_seq;
    int m_server_fd;

    CEpoll *m_epoll;
    CLogger *m_logger;
    std::list<reader_conn_t *> m_conn_free;
    HHMap<uint32_t, reader_conn_t*> m_conn_map;
    HHMap<uint32_t, timeval *> m_seq_cost;
    std::vector<CNetQueue *> m_recv_queue;
    std::vector<CNetQueue *> m_send_queue;
    uint32_t m_reload;
};
#endif // _DATA_JASONYJIANG_COMMON_PROJ_TOOLS_TNF_TNF_RECV_H
