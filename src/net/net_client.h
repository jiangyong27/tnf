#ifndef NET_CLIENT_H
#define NET_CLIENT_H
#pragma once
#include <map>

#include "net_thread.h"
#include "net_api.h"
#include "net_epoll.h"
#include "net_logger.h"
#include "net_check.h"
#include "net_queue.h"

typedef void (*recv_hander_f)(void *master, const char *recv_buf, uint32_t recv_len);

// todo
// 1. cycle buffer
//

struct send_header_t
{
    uint32_t ip;
    uint16_t port;
};

struct recv_header_t
{
    uint32_t ip;
    uint16_t port;
};

struct client_data_t
{
    int fd;
    char *send_buf;
    char *recv_buf;
    uint32_t send_len;
    uint32_t recv_len;
    uint32_t send_pos;
    uint32_t recv_pos;
    int sock_status;
    client_data_t();
    ~client_data_t();
    void Reset();
};

struct stat_net_t
{
    uint32_t connetct_num;
    uint32_t close_num;
    uint32_t send_num;
    uint32_t recv_num;
    uint64_t send_bytes;
    uint64_t recv_bytes;
    uint32_t send_zero_num;
    uint32_t recv_zero_num;
    stat_net_t();
    stat_net_t operator- (stat_net_t &other);
};

class CNetClient : public Thread
{
public:
    enum {
        ERR_OK                  = 0,
        ERR_SEND_TOO_FAST       = -1,
        ERR_SEND_TOO_LARGE      = -2,
        ERR_CONEECT_FAILED      = -3,
        PROTOCOL_HTTP           = 1,
        PROTOCOL_LONGCONN       = 2,
        COMM_TYPE_ASYNC         = 100, //异步
        COMM_TYPE_SYNC          = 101, //同步
    };

public:
    CNetClient(CNetQueue *send_queue, CNetQueue *recv_queue = NULL, CLogger* logger = NULL);
    ~CNetClient();
    void Run();

    void SetProtocol(int protocol) {
        m_protocol = protocol;
    }

    void SetKeepalive(bool keepalive = true) {
        m_use_keepalive = keepalive ? 1 : 0;
    }

    const stat_net_t GetTotalStat() {
        return *m_stat;
    }

    const stat_net_t GetAddStat() {
        stat_net_t stat = *m_stat - *m_pre_stat;
        *m_pre_stat = *m_stat;
        return stat;
    }

private:
    void Connect(uint32_t ip, uint16_t port, client_data_t **data);
    void OnSend(client_data_t *data);
    void OnRecv(client_data_t *data);
    void OnClose(client_data_t *data);
    void OnQueue(client_data_t *data);

private:
    int m_protocol;
    int m_use_keepalive;
    int m_queue_fd;

    std::map<uint64_t, client_data_t*> m_connects;
    CLogger *m_logger;
    CNetQueue *m_send_queue;
    CNetQueue *m_recv_queue;

    recv_hander_f m_recv_func;
    CEpoll *m_epoll;
    void *m_master;
    stat_net_t *m_stat;
    stat_net_t *m_pre_stat;

    uint32_t m_max_pack_len;
    uint32_t m_run_num;
};

#endif // NET_CLIENT_H
