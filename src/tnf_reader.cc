#include "tnf_reader.h"
#include "longconn/longconn_helper.h"

//
void reader_conn_t::Reset()
{
    send_pos = 0;
    send_len = 0;
    recv_pos = 0;
    recv_len = 0;
    conn_id  = 0;
    type = kConnTypeSocket;
    time(&access_time);
}

reader_conn_t::reader_conn_t()
{
    recv_buf = NULL;
    send_buf = NULL;
    ptr = NULL;
    Reset();
}

reader_conn_t::~reader_conn_t()
{
    if (send_buf) delete [] send_buf;
    if (recv_buf) delete [] recv_buf;
}

CTNFReader::CTNFReader()
{
    m_epoll = new CEpoll(1024);
    m_seq = 0;
    m_use_http = 1;
    m_use_keepalive = 1;
    m_buffer = NULL;
    m_send_num = 0;
    m_recv_num = 0;
    m_total_cost = 0;
    m_queue_cost = 0;
    m_total_net_cost = 0;
    m_reload = 0;
}

CTNFReader::~CTNFReader()
{
    delete m_epoll;
    if (m_buffer) delete [] m_buffer;
}


int CTNFReader::AddQueue(CNetQueue* recv, CNetQueue* send)
{
    m_send_queue.push_back(send);
    m_recv_queue.push_back(recv);

    reader_conn_t* conn = new reader_conn_t();
    conn->Reset();
    conn->type = kConnTypeQueue;
    conn->fd = recv->Fd();
    conn->ptr = recv;
    m_epoll->Add(conn->fd, conn, true, false);
    return 0;
}

int CTNFReader::LoadConf()
{
    CNetConf conf;
    if (!conf.OpenIniFile(m_conf)) {
        printf("load conf file[%s] faield!\n", m_conf.c_str());
        return -1;
    }

    std::string addr;
    conf.GetFiledValue("reader", "server_addr", "0.0.0.0", &addr);
    conf.GetFiledValue("reader", "server_port", 12000, &m_server_port);
    conf.GetFiledValue("reader", "max_conn_num", 1000, &m_max_conn_num);
    conf.GetFiledValue("reader", "max_send_size", 1024 * 1024 * 2, &m_max_send_size);
    conf.GetFiledValue("reader", "max_recv_size", 1024 * 1024 * 10, &m_max_recv_size);
    conf.GetFiledValue("reader", "conn_timeout", 60, &m_conn_timeout);
    conf.GetFiledValue("reader", "use_http", 1, &m_use_http);
    conf.GetFiledValue("reader", "use_keepalive", 1, &m_use_keepalive);
    m_server_ip = str2ip(addr);

    LOG_INFO(m_logger, "server[%s:%d],max_conn_num[%d],max_send_size[%d],max_recv_size[%d],"
             "conn_timeout[%d],use_http[%d],use_keepalive[%d]",
             addr.c_str(), m_server_port, m_max_conn_num, m_max_send_size, m_max_recv_size, m_conn_timeout,
             m_use_http, m_use_keepalive
             );
    return 0;
}

int CTNFReader::Init(const std::string& conf_path, CLogger* logger)
{
    m_logger = logger;
    m_conf = conf_path;
    if (LoadConf() != 0) {
        return -1;
    }
    m_server_fd = init_server(m_server_ip, m_server_port);
    if (m_server_fd < 0) {
        printf("init server [%s:%d] faield\n", ip2str(m_server_ip).c_str(), m_server_port);
        return -2;
    }
    m_buffer_size = m_max_send_size > m_max_recv_size ? m_max_send_size : m_max_recv_size;
    m_buffer = new char[m_buffer_size];
    set_sock_reuseseaddr(m_server_fd);
    set_sock_noblock(m_server_fd);
    set_sock_nodelay(m_server_fd);

    reader_conn_t* conn = new reader_conn_t();
    conn->Reset();
    conn->type = kConnTypeListen;
    conn->fd = m_server_fd;
    m_epoll->Add(m_server_fd, conn, true, false);
    return 0;
}

void CTNFReader::OnAccept()
{
    int conn_sock;
    struct sockaddr_in remote;
    uint32_t addr_len = sizeof(remote);
    memset(&remote, 0, sizeof(remote));
    conn_sock = accept(m_server_fd, (struct sockaddr *) &remote, (socklen_t *) &addr_len);
    if (conn_sock == -1 ) {
        if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR) {
            LOG_ERROR(m_logger, "accept failed![error=%s]", strerror(errno));
            perror("accept failed !");
            assert(0);
        }
        return;
    }
    set_sock_noblock(conn_sock);
    set_sock_keepalive(conn_sock);
    set_sock_nodelay(conn_sock);
    reader_conn_t *conn = NULL;
    if (m_conn_free.size() == 0) {
        conn = new reader_conn_t();
        conn->send_buf = new char[m_max_send_size];
        conn->recv_buf = new char[m_max_recv_size];
    } else {
        conn = m_conn_free.front();
        m_conn_free.pop_front();
    }

    conn->Reset();
    conn->type = kConnTypeSocket;
    conn->fd = conn_sock;
    m_epoll->Add(conn_sock, conn, true, false);
    static uint32_t conn_id = 0;
    conn->conn_id = conn_id;
    m_conn_map[conn->conn_id] = conn;
    conn_id++;
}

void CTNFReader::InWorker(const char* conn_buf, uint32_t conn_len, uint32_t conn_id)
{
    struct timeval *now = new timeval;
    gettimeofday(now, NULL);

    reader_req_t header;
    header.conn_id = conn_id;
    header.seq = m_seq;
    m_seq_cost[m_seq] = now;
    m_seq++;
    gettimeofday(&header.create_time, NULL);
    memcpy(m_buffer, &header, sizeof(header));
    memcpy(m_buffer + sizeof(header), conn_buf, conn_len);
    static uint32_t work_id = 0;
    CNetQueue* queue = m_send_queue[work_id % m_send_queue.size()];
    work_id++;
    int ret = queue->Push(m_buffer, sizeof(header) + conn_len);
    if (ret != 0) {
        LOG_ERROR(m_logger, "recv push failed![%d]", ret);
        return;
    }

    return ;
}

void CTNFReader::OnWorker(CNetQueue* queue)
{
    uint32_t data_len;
    int ret = 0;
    //for (uint32_t i = 0; i < 10; i++) {
    do {
        ret = queue->Pop(m_buffer, m_buffer_size, &data_len);
        if (ret != 0) {
            break;
        }
        reader_rsp_t *header = (reader_rsp_t*) m_buffer;
        HHMap<uint32_t, reader_conn_t*>::iterator iter = m_conn_map.find(header->conn_id);
        if (iter == m_conn_map.end()) {
            LOG_DEBUG(m_logger, "conn_id[%d] closed!", header->conn_id);
            continue;
        }
        HHMap<uint32_t, timeval *>::iterator iter2 = m_seq_cost.find(header->seq);
        timeval now;
        gettimeofday(&now, NULL);
        m_queue_cost += TIME_DIFF_US(now , header->create_time);
        if (iter2 != m_seq_cost.end()) {
            m_total_cost += TIME_DIFF_US(now, *(iter2->second));
            delete iter2->second;
            m_seq_cost.erase(iter2);
        } else {
            LOG_ERROR(m_logger, "tnf_seq[%d] not found", header->seq);
        }

        uint32_t real_data_len = data_len - sizeof(reader_rsp_t);
        reader_conn_t *conn = iter->second;
        if (conn->send_len + real_data_len > m_max_send_size) {
            LOG_ERROR(m_logger, "send fast buffer full! buffer[%d],send_len[%d]",
                      m_max_send_size-conn->send_len,real_data_len);
            continue;
        }

        memcpy(conn->send_buf + conn->send_len, m_buffer + sizeof(reader_rsp_t), real_data_len);
        conn->send_len += real_data_len;
        m_epoll->Mod(conn->fd, conn, true, true);
        m_send_num++;
    } while(!queue->IsEmpty());

}

void CTNFReader::OnRecv(reader_conn_t* conn)
{
    int ret = recv(conn->fd, conn->recv_buf + conn->recv_len, m_max_recv_size - conn->recv_len, 0);
    if (ret < 0 ) {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
            LOG_DEBUG(m_logger, "recv eintr!ret[%d],error[%s]", ret,strerror(errno));
            return;
        }
        LOG_ERROR(m_logger, "recv failed!ret[%d],error[%s]", ret,strerror(errno));
        OnClose(conn);
        return;
    } else if (ret == 0) {
        OnClose(conn);
        return;
    }

    conn->recv_len += ret;

    if (m_use_http == 1) {
        ret = net_complete_http(conn->recv_buf, conn->recv_len);
    } else {
        ret = net_complete_longconn(conn->recv_buf, conn->recv_len);
    }

    if (ret > 0) {
        if (ret < (int)conn->recv_len) {
            InWorker(conn->recv_buf, ret, conn->conn_id);
            m_recv_num++;
            uint32_t pos = ret;
            while (1) {
                if (m_use_http == 1) {
                    ret = net_complete_http(conn->recv_buf + pos, conn->recv_len - pos);
                } else {
                    ret = net_complete_longconn(conn->recv_buf + pos, conn->recv_len - pos);
                }
                if (ret == 0 || ret < 0) {
                    break;
                }

                InWorker(conn->recv_buf + pos, ret, conn->conn_id);
                pos += ret;
                m_recv_num++;
            }
            memcpy(conn->recv_buf, conn->recv_buf + pos , conn->recv_len - pos);
            conn->recv_len = conn->recv_len - pos;
        } else if (ret == (int) conn->recv_len) {
            InWorker(conn->recv_buf, ret, conn->conn_id);
            conn->recv_len = 0;
            m_recv_num++;
        } else {
            LOG_ERROR(m_logger, "net check error1!recv_len[%d],check_len[=%d]", conn->recv_len, ret);
            OnClose(conn);
        }
    } else if (ret < 0) {
        LOG_ERROR(m_logger, "net check error2!recv_len[%d],check_len[=%d]", conn->recv_len, ret);
        OnClose(conn);
    }
    time(&conn->access_time);
}

void CTNFReader::OnSend(reader_conn_t* conn)
{
    int ret = send(conn->fd, conn->send_buf + conn->send_pos, conn->send_len - conn->send_pos, 0);
    if (ret < 0 ) {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
            LOG_DEBUG(m_logger, "send eintr!ret[%d],error[%s]", ret,strerror(errno));
            return;
        }
        LOG_ERROR(m_logger, "send failed!ret[%d],error[%s]", ret,strerror(errno));
        OnClose(conn);
        return ;
    } else if (ret == 0) {
        //m_epoll->Mod(conn->fd, conn, true, false);
        LOG_DEBUG(m_logger, "send zero!ret[%d],error[%s]", ret,strerror(errno));
        return;
    }
    struct timeval now;
    gettimeofday(&now, NULL);
    conn->send_pos += ret;
    if (conn->send_pos >= conn->send_len) {
        conn->send_len = 0;
        conn->send_pos = 0;
        if (m_use_http == 1 && m_use_keepalive == 0) {
            OnClose(conn);
        } else {
            m_epoll->Mod(conn->fd, conn, true, false);;
        }
    }
}

void CTNFReader::OnClose(reader_conn_t* conn)
{
    if (m_logger != NULL) {
        uint32_t ip1,ip2;
        uint16_t port1,port2;
        get_peer_addr(conn->fd, &ip1, &port1);
        get_sock_addr(conn->fd, &ip2, &port2);
        LOG_DEBUG(m_logger, "close fd[%d],[%s:%d]->[%s:%d]", conn->fd,ip2str(ip1).c_str(), port1, ip2str(ip2).c_str(), port2);
    }
    m_epoll->Del(conn->fd);
    close(conn->fd);
    m_conn_free.push_back(conn);
    m_conn_map.erase(conn->conn_id);
}

void CTNFReader::OnCheck()
{
    static time_t pre = 0;
    time_t now;
    time(&now);
    if (now - pre < 1) {
        return;
    }

    // free conn
    for (std::list<reader_conn_t*>::iterator iter = m_conn_free.begin(); iter != m_conn_free.end();) {
        reader_conn_t *conn = *iter;
        if (now - conn->access_time >= m_conn_timeout) {
            LOG_INFO(m_logger, "conn_id[%d],fd[%d],access[%d],delete!", conn->conn_id, conn->fd, conn->access_time);
            delete conn;
            iter = m_conn_free.erase(iter);
        } else {
            ++iter;
        }
    }

    // free seq_map
    struct timeval now_tv;
    gettimeofday(&now_tv, NULL);
    for (HHMap<uint32_t, timeval *>::iterator iter = m_seq_cost.begin(); iter != m_seq_cost.end();) {
        if (TIME_DIFF_MS(now_tv, *(iter->second)) >= m_conn_timeout * 1000) {
            LOG_ERROR(m_logger, "seq[%d] app worker process timeout1!", iter->first);
            delete iter->second;
            iter = m_seq_cost.erase(iter);
        } else {
            ++iter;
        }
    }
    LOG_INFO(m_logger, "recv_num[%d],send_num[%d],total_cost[%.2f]ms,queue_cost[%.2f]ms",
             m_recv_num, m_send_num, m_total_cost/float(1000)/(m_send_num+1),
             m_queue_cost / float(1000)/(m_send_num+1));

    m_send_num = 0;
    m_recv_num = 0;
    m_queue_cost = 0;
    m_total_cost = 0;
    pre = now;

    if (m_reload == 1 ) {
        m_reload = 0;
        LoadConf();
    }
}


void CTNFReader::Run()
{
    std::vector<void *> rfds;
    std::vector<void *> wfds;
    while (!m_stop) {
        m_epoll->Wait(&rfds, &wfds, 1);
        for (uint32_t i = 0; i < rfds.size(); i++) {
            reader_conn_t *conn = (reader_conn_t *) rfds[i];
            if (conn->type == kConnTypeListen) {
                OnAccept();
            } else if (conn->type == kConnTypeSocket) {
                OnRecv(conn);
            } else if (conn->type == kConnTypeQueue) {
                CNetQueue *queue = (CNetQueue*) conn->ptr;
                OnWorker(queue);
            }
        }

        for (uint32_t i = 0; i < m_recv_queue.size(); i++) {
            CNetQueue *queue = m_recv_queue[i];
            OnWorker(queue);
        }

        for (uint32_t i = 0; i < wfds.size(); i++) {
            reader_conn_t *conn = (reader_conn_t *) wfds[i];
            OnSend(conn);
        }
        OnCheck();
    }
}

