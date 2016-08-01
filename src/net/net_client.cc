#include "net_client.h"

stat_net_t::stat_net_t()
{
    connetct_num = 0;
    close_num = 0;
    send_num = 0;
    recv_num = 0;
    send_zero_num = 0;
    recv_zero_num = 0;
    send_bytes = 0;
    recv_bytes = 0;
}

stat_net_t stat_net_t::operator-(stat_net_t &other)
{
    stat_net_t tmp;
    tmp.connetct_num = connetct_num - other.connetct_num;
    tmp.close_num = close_num - other.close_num;
    tmp.send_num = send_num - other.send_num;
    tmp.recv_num = recv_num - other.recv_num;
    tmp.send_bytes = send_bytes - other.send_bytes;
    tmp.recv_bytes = recv_bytes - other.recv_bytes;
    tmp.recv_zero_num = recv_zero_num - other.recv_zero_num;
    return tmp;
}

client_data_t:: client_data_t()
{
    recv_buf = NULL;
    send_buf = NULL;
    sock_status = 0;
    Reset();
}

client_data_t::~client_data_t()
{
    if (recv_buf) {
        delete [] recv_buf;
        recv_buf = NULL;
    }
    if (send_buf) {
        delete [] send_buf;
        send_buf = NULL;
    }
    Reset();
}

void client_data_t::Reset()
{
    send_len = 0;
    recv_len = 0;
    send_pos = 0;
    fd = -1;
}

CNetClient::CNetClient(CNetQueue *send_queue, CNetQueue *recv_queue, CLogger* logger)
{
    m_epoll = NULL;
    m_recv_func = NULL;
    m_logger = logger;
    m_recv_queue = recv_queue;
    m_send_queue = send_queue;
    m_protocol = PROTOCOL_HTTP;
    m_use_keepalive = 1;
    m_epoll = new CEpoll(1024);
    m_stat = new stat_net_t;
    m_pre_stat = new stat_net_t;
    m_max_pack_len = 1024 * 1024 * 32;
    m_run_num = 1;

    if (m_send_queue) {
        client_data_t *data = new client_data_t;
        m_queue_fd = m_send_queue->Fd();
        data->fd = m_queue_fd;
        data->send_buf = new char[m_max_pack_len];
        m_epoll->Add(m_queue_fd, data, true, false);
    }
}

CNetClient::~CNetClient()
{
    for (std::map<uint64_t, client_data_t*>::iterator iter = m_connects.begin(); iter != m_connects.end(); ++iter) {
        close(iter->second->fd);
        delete iter->second;
    }
    delete m_epoll;
    delete m_stat;
    delete m_pre_stat;
}


void CNetClient::Run()
{
    std::vector<void *> recv_fds;
    std::vector<void *> send_fds;

    while (!m_stop) {
        recv_fds.clear();
        send_fds.clear();
        m_epoll->Wait(&recv_fds, &send_fds, 1);
        if (recv_fds.size() == 0 && send_fds.size() == 0) {
            continue;
        }

        for (uint32_t i = 0; i < recv_fds.size(); ++i) {
            client_data_t* data = (client_data_t*)(recv_fds[i]);
            if (data->fd == m_queue_fd) {
                OnQueue(data);
            } else {
                OnRecv(data);
            }
        }

        for (uint32_t i = 0; i < send_fds.size(); ++i) {
            client_data_t* data = (client_data_t*) send_fds[i];
            OnSend(data);
        }
    }
}


// 4 +2 bytes = ip + port
//
void CNetClient::OnQueue(client_data_t* queue_data)
{
    int iRet = m_send_queue->Pop(queue_data->send_buf, m_max_pack_len, &(queue_data->send_len));
    if (iRet != 0) {
        LOG_ERROR(m_logger, "RecvQueue failed!max[%d],len[%d]",m_max_pack_len, queue_data->send_len);
        return;
    }

    send_header_t header;
    memcpy(&header, queue_data->send_buf, sizeof(header));

    client_data_t *data = NULL;
    Connect(header.ip, header.port, &data);
    if (data == NULL) {
        return ;
    }

    if (data->send_len + queue_data->send_len - sizeof(header) > m_max_pack_len) {
        return;
    }

    memcpy(data->send_buf + data->send_len, queue_data->send_buf + sizeof(header), queue_data->send_len - sizeof(header));
    data->send_len += queue_data->send_len - sizeof(header);

    if (data->sock_status == 0) { //new connected
        m_epoll->Add(data->fd, data, true, true);
        data->sock_status = 1;
    } else {
        m_epoll->Mod(data->fd, data, true, true);
    }

}


void CNetClient::Connect(uint32_t ip, uint16_t port, client_data_t **data)
{
    uint64_t connect_key = ((uint64_t)ip << 32) + port;

    std::map<uint64_t, client_data_t*>::iterator iter = m_connects.find(connect_key);
    if (iter == m_connects.end()) {
        int fd = init_client(ip, port);
        if (fd < 0 ) {
            *data = NULL;
            return ;
        }
        LOG_DEBUG(m_logger, "connect [%s:%d] success!", ip2str(ip).c_str(), port);

        set_sock_noblock(fd);
        *data = new client_data_t();
        (*data)->Reset();
        (*data)->fd = fd;
        (*data)->send_buf = new char[m_max_pack_len];
        (*data)->recv_buf = new char[m_max_pack_len];
        m_connects[connect_key] = *data;
        m_stat->connetct_num++;
    } else {
        *data = iter->second;
    }

    return;
}

void CNetClient::OnClose(client_data_t* data)
{
    if (m_logger != NULL) {
        uint32_t ip1,ip2;
        uint16_t port1,port2;
        get_sock_addr(data->fd, &ip1, &port1);
        get_peer_addr(data->fd, &ip2, &port2);
        LOG_DEBUG(m_logger, "close fd[%d],[%s:%d]->[%s:%d]", data->fd,ip2str(ip1).c_str(), port1, ip2str(ip2).c_str(), port2);
    }

    m_stat->close_num++;

    for (std::map<uint64_t, client_data_t*>::iterator iter = m_connects.begin(); iter != m_connects.end(); ) {
        if (data->fd == iter->second->fd) {
            m_connects.erase(iter++);
        } else {
            ++iter;
        }
    }
    m_epoll->Del(data->fd);
    close(data->fd);

    delete data;
}

void CNetClient::OnSend(client_data_t *data)
{
    int ret = send(data->fd, data->send_buf + data->send_pos, data->send_len - data->send_pos, 0);
    if (ret < 0 ) {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
            return;
        }
        LOG_ERROR(m_logger, "send failed!ret[%d],error[%s]", ret,strerror(errno));
        OnClose(data);
        return ;
    } else if (ret == 0) {
        //OnClose(data);
        m_epoll->Mod(data->fd, data, true, false);
        m_stat->send_zero_num = 0;
        return;
    }

    m_stat->send_bytes += ret;
    data->send_pos += ret;
    if (data->send_pos >= data->send_len) {
        data->send_len = 0;
        data->send_pos = 0;
        m_epoll->Mod(data->fd, data, true, true);;
        m_stat->send_num++;
    }
}


void CNetClient::OnRecv(client_data_t* data)
{
    int ret = recv(data->fd, data->recv_buf + data->recv_len, m_max_pack_len - data->recv_len, 0);
    if (ret < 0 ) {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
            return;
        }
        LOG_ERROR(m_logger, "recv failed!ret[%d],error[%s]", ret,strerror(errno));
        OnClose(data);
        return;
    } else if (ret == 0) {
        m_stat->recv_zero_num++;
        OnClose(data);
        return;
    }

    data->recv_len += ret;
    m_stat->recv_bytes += ret;

    if (!m_recv_queue) {
        data->recv_len = 0;
        return;
    }

    /// protocol
    if (m_protocol == PROTOCOL_HTTP) {
        ret = net_complete_http(data->recv_buf, data->recv_len);
    } else if (m_protocol == PROTOCOL_LONGCONN) {
        ret = net_complete_longconn(data->recv_buf, data->recv_len);
    }

    if (ret > 0) {
        //recv_header_t header;
        if (ret < (int)data->recv_len) {
            if (0 != m_recv_queue->Push(data->recv_buf, ret)) {
                LOG_ERROR(m_logger, "recv queue push failed!");
            }
            memcpy(data->recv_buf, data->recv_buf + ret , data->recv_len - ret);
            data->recv_len = data->recv_len - ret;
        } else if (ret == (int) data->recv_len) {
            if (0 != m_recv_queue->Push(data->recv_buf, data->recv_len)) {
                LOG_ERROR(m_logger, "recv queue push failed!");
            }
            data->recv_len = 0;
        } else {
            LOG_P("net check error!recv_len[%d],check_len[=%d]", data->recv_len, ret);
        }
        m_stat->recv_num++;

        if (m_protocol == PROTOCOL_HTTP && m_use_keepalive == 0 ) {
            OnClose(data);
        }
    }
}
