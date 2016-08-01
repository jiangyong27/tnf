#include "tnf_writer.h"
writer_data_t:: writer_data_t()
{
    recv_buf = NULL;
    send_buf = NULL;
    sock_status = 0;
    Reset();
}

writer_data_t::~writer_data_t()
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

void writer_data_t::Reset()
{
    send_len = 0;
    recv_len = 0;
    send_pos = 0;
    fd = -1;
}

CTNFWriter::CTNFWriter(CLogger* logger)
{
    m_epoll = NULL;
    m_logger = logger;
    m_req_queue = NULL;
    m_rsp_queue = NULL;
    m_use_keepalive = 1;
    m_epoll = new CEpoll(10);
    m_max_pack_len = 1024 * 1024 * 32;
    m_buffer = new char [m_max_pack_len];

}

CTNFWriter::~CTNFWriter()
{
    for (std::map<uint64_t, writer_data_t*>::iterator iter = m_connects.begin(); iter != m_connects.end(); ++iter) {
        close(iter->second->fd);
        delete iter->second;
    }
    delete [] m_buffer;
    delete m_epoll;
}
void CTNFWriter::SetQueue(CNetQueue* req_queue, CNetQueue* rsp_queue)
{
    m_req_queue = req_queue;
    m_rsp_queue = rsp_queue;
    if (m_req_queue) {
        writer_data_t *data = new writer_data_t;
        m_queue_fd = m_req_queue->Fd();
        data->fd = m_queue_fd;
        m_epoll->Add(m_queue_fd, data, true, false);
    }
}


void CTNFWriter::Run()
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
            writer_data_t* data = (writer_data_t*)(recv_fds[i]);
            if (data->fd == m_queue_fd) {
                OnQueue(data);
            } else {
                OnRecv(data);
            }
        }

        for (uint32_t i = 0; i < send_fds.size(); ++i) {
            writer_data_t* data = (writer_data_t*) send_fds[i];
            OnSend(data);
        }
    }
}

void CTNFWriter::OnQueue(writer_data_t* queue_data)
{
    uint32_t data_len;
    int ret = m_req_queue->Pop(m_buffer, m_max_pack_len, &data_len);
    if (ret != 0) {
        LOG_ERROR(m_logger, "RecvQueue failed!max[%d],ret[%d]",m_max_pack_len, ret);
        return;
    }

    writer_req_header_t *header = (writer_req_header_t*) m_buffer;
    writer_data_t *data = NULL;
    Connect(header->conn_id, header->ip, header->port, &data);
    if (data == NULL) {
        LOG_ERROR(m_logger, "connect [%s:%d] failed!", ip2str(header->ip).c_str(), header->port);
        return ;
    }

    uint32_t real_data_len = data_len - sizeof(writer_req_header_t);
    if (data->send_len + real_data_len > m_max_pack_len) {
        LOG_ERROR(m_logger, "wait_send[%d],real_send[%d],max[%d]", data->send_len, real_data_len, m_max_pack_len);
        return;
    }

    memcpy(data->send_buf + data->send_len, m_buffer + sizeof(writer_req_header_t), real_data_len);
    data->send_len += real_data_len;

    if (data->sock_status == 0) { //new connected
        m_epoll->Add(data->fd, data, true, true);
        data->sock_status = 1;
    } else {
        m_epoll->Mod(data->fd, data, true, true);
    }

}


void CTNFWriter::Connect(uint64_t conn_id, uint32_t ip, uint16_t port, writer_data_t **data)
{
    std::map<uint64_t, writer_data_t*>::iterator iter = m_connects.find(conn_id);
    if (iter == m_connects.end()) {
        int fd = init_client(ip, port);
        if (fd < 0 ) {
            *data = NULL;
            return ;
        }
        LOG_DEBUG(m_logger, "connect [%s:%d] success!", ip2str(ip).c_str(), port);

        set_sock_noblock(fd);
        *data = new writer_data_t();
        (*data)->Reset();
        (*data)->fd = fd;
        (*data)->send_buf = new char[m_max_pack_len];
        (*data)->recv_buf = new char[m_max_pack_len];
        m_connects[conn_id] = *data;
    } else {
        *data = iter->second;
    }

    return;
}

void CTNFWriter::OnClose(writer_data_t* data)
{
    if (m_logger != NULL) {
        uint32_t ip1,ip2;
        uint16_t port1,port2;
        get_sock_addr(data->fd, &ip1, &port1);
        get_peer_addr(data->fd, &ip2, &port2);
        LOG_DEBUG(m_logger, "close fd[%d],[%s:%d]->[%s:%d]", data->fd,ip2str(ip1).c_str(), port1, ip2str(ip2).c_str(), port2);
    }

    for (std::map<uint64_t, writer_data_t*>::iterator iter = m_connects.begin(); iter != m_connects.end(); ) {
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

void CTNFWriter::OnSend(writer_data_t *data)
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
        return;
    }

    data->send_pos += ret;
    if (data->send_pos >= data->send_len) {
        data->send_len = 0;
        data->send_pos = 0;
        m_epoll->Mod(data->fd, data, true, true);;
    }
}


void CTNFWriter::OnRecv(writer_data_t* data)
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
        OnClose(data);
        return;
    }

    data->recv_len += ret;

    if (!m_rsp_queue) {
        data->recv_len = 0;
        return;
    }

    ret = net_complete_longconn(data->recv_buf, data->recv_len);
    if (ret > 0) {
        if (ret < (int)data->recv_len) {
            //writer_rsp_header_t *header = (writer_rsp_header_t *)m_buffer;
            memcpy(m_buffer + sizeof(writer_rsp_header_t), data->recv_buf, ret);
            if (0 != m_rsp_queue->Push(m_buffer, ret + sizeof(writer_rsp_header_t))) {
                LOG_ERROR(m_logger, "recv queue push failed!");
            }
            memcpy(data->recv_buf, data->recv_buf + ret , data->recv_len - ret);
            data->recv_len = data->recv_len - ret;
        } else if (ret == (int) data->recv_len) {
            //writer_rsp_header_t *header = (writer_rsp_header_t *)m_buffer;
            memcpy(m_buffer + sizeof(writer_rsp_header_t), data->recv_buf, data->recv_len);
            if (0 != m_rsp_queue->Push(data->recv_buf, data->recv_len)) {
                LOG_ERROR(m_logger, "recv queue push failed!");
            }
            data->recv_len = 0;
        } else {
            LOG_ERROR(m_logger, "net check error!recv_len[%d],check_len[=%d]", data->recv_len, ret);
        }

        if (m_use_keepalive == 0 ) {
            OnClose(data);
        }
    }
}
