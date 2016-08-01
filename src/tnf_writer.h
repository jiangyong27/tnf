#ifndef NET_CLIENT_H
#define NET_CLIENT_H
#pragma once
#include <map>

#include "net/net_thread.h"
#include "net/net_api.h"
#include "net/net_epoll.h"
#include "net/net_logger.h"
#include "net/net_check.h"
#include "net/net_queue.h"

struct writer_req_header_t
{
    uint64_t conn_id;
    uint32_t ip;
    uint16_t port;
};

struct writer_rsp_header_t
{
    uint32_t ip;
    uint16_t port;
};

struct writer_data_t
{
    int fd;
    char *send_buf;
    char *recv_buf;
    uint32_t send_len;
    uint32_t recv_len;
    uint32_t send_pos;
    uint32_t recv_pos;
    int sock_status;
    uint64_t conn_id;
    writer_data_t();
    ~writer_data_t();
    void Reset();
};


class CTNFWriter : public Thread
{
public:
    CTNFWriter(CLogger* logger = NULL);
    ~CTNFWriter();
    void Run();
    void SetQueue(CNetQueue* req_queue, CNetQueue* rsp_queue);

    void SetKeepalive(bool keepalive = true) {
        m_use_keepalive = keepalive ? 1 : 0;
    }

private:
    void Connect(uint64_t conn_id, uint32_t ip, uint16_t port, writer_data_t **data);
    void OnSend(writer_data_t *data);
    void OnRecv(writer_data_t *data);
    void OnClose(writer_data_t *data);
    void OnQueue(writer_data_t *data);

private:
    int m_use_keepalive;
    int m_queue_fd;

    std::map<uint64_t, writer_data_t*> m_connects;
    CLogger *m_logger;
    CNetQueue *m_req_queue;
    CNetQueue *m_rsp_queue;
    CEpoll *m_epoll;

    uint32_t m_max_pack_len;
    char *m_buffer;
};

#endif // NET_CLIENT_H
