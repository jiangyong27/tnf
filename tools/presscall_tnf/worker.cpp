#include "worker.h"
void disp_client_recv(void *priv)
{
    CWorker *worker = (CWorker*)priv;
    worker->ClientQueue();
}

void disp_reader_recv(void *priv)
{
    CWorker *worker = (CWorker*)priv;
    worker->ReaderQueue();
}

CWorker::CWorker()
{
    static int idx = 0;
    m_worker_idx = idx++;
    m_client = NULL;
    m_stop = false;
    m_send_num = 0;
    m_recv_num = 0;
    m_timeout = 10000;
    m_timeout_num  = 0;
    m_buffer_size = 1024 * 1024 * 10;
    m_buffer = NULL;

    m_client_send_queue = NULL;
    m_client_recv_queue = NULL;
    m_pack = new LongconnHelper();
    m_session_mgr = new CSessionMgr();
    m_epoll = new CEpoll(10);
}

CWorker::~CWorker()
{
    if (m_client) {
        m_client->Stop();
        m_client->Join();
        delete m_client;
    }
    if (m_client_send_queue) delete m_client_send_queue;
    if (m_client_recv_queue) delete m_client_recv_queue;
    if (m_buffer) delete [] m_buffer;
    delete m_pack;
    delete m_session_mgr;
    delete m_epoll;

}

void CWorker::Init(const worker_conf_t& conf)
{
    m_logger = conf.logger;
    m_reader_send_queue = conf.reader_send_queue;
    m_reader_recv_queue = conf.reader_recv_queue;

    m_client_send_queue = new CNetQueue(false, true);
    m_client_recv_queue = new CNetQueue(true, false);
    m_buffer = new char[m_buffer_size];

    //m_client = new CTNFWriter(m_client_send_queue, m_client_recv_queue, m_logger);
    //m_client->SetProtocol(CNetClient::PROTOCOL_LONGCONN);
    m_client = new CTNFWriter(m_logger);
    m_client->SetQueue(m_client_send_queue, m_client_recv_queue);

    std::string::size_type pos = conf.addr.find(":");
    if (pos != std::string::npos) {
        m_ip = str2ip(conf.addr.substr(0, pos));
        m_port = atoi(conf.addr.substr(pos + 1).c_str());
    }

    worker_epoll_t* client_recv = new worker_epoll_t;
    worker_epoll_t* reader_recv = new worker_epoll_t;
    client_recv->fd = m_client_recv_queue->Fd();
    client_recv->priv = this;
    client_recv->func = disp_client_recv;

    reader_recv->fd = m_reader_send_queue->Fd();
    reader_recv->priv = this;
    reader_recv->func = disp_reader_recv;
    m_epoll->Add(m_client_recv_queue->Fd(), client_recv, true, false);
    m_epoll->Add(m_reader_send_queue->Fd(), reader_recv, true, false);

    m_client->Start();
}

void CWorker::ParseLOGResponse(uint32_t seq, const char* data_buf, uint32_t data_len)
{
    log_server::LOGResponse log_response;
    if (!log_response.ParseFromArray(data_buf, data_len)) {
        LOG_P("parse pb failed");
        return;
    }

    CBaseSession *info = NULL;
    CQueryInfo *query_info = NULL;
    if (m_session_mgr->Get(seq, &info) != 0) {
        LOG_ERROR(m_logger, "session get failed![%d]",seq);
        return;
    }

    query_info = dynamic_cast<CQueryInfo*>(info);

    server_result_t result;
    result.retcode = 0;
    result.cost = query_info->Cost();
    result.result_num = 1;
    //printf("seq=%d,seq=%d,cost=%d,seq=%s\n", seq, query_info->m_seq, query_info->Cost(), log_response.echo().c_str());

    if (0 != m_reader_recv_queue->Push((char*)&result, sizeof(result))) {
        LOG_ERROR(m_logger, "push reader_recv_queue failed!");
    }
    delete query_info;
}

void CWorker::ParseRBUResponse(uint32_t seq, const char* data_buf, uint32_t data_len)
{
    RBUResponse rbu_response;
    if (!rbu_response.ParseFromArray(data_buf, data_len)) {
        LOG_P("parse pb failed");
        return;
    }

    CBaseSession *info = NULL;
    CQueryInfo *query_info = NULL;
    if (m_session_mgr->Get(seq, &info) != 0) {
        LOG_ERROR(m_logger, "session get failed!");
        return;
    }

    query_info = dynamic_cast<CQueryInfo*>(info);

    server_result_t result;
    result.retcode = 0;
    result.cost = query_info->Cost();
    result.result_num = rbu_response.result_list_size();

    if (0 != m_reader_recv_queue->Push((char*)&result, sizeof(result))) {
        LOG_ERROR(m_logger, "push reader_recv_queue failed!");
    }
    delete query_info;
}

void CWorker::CreateRBURequest(uint32_t seq, const std::string& query, std::string *req)
{
    std::string search_id = "jasonyjiang";
    RBURequest rbu_request;
    rbu_request.set_search_query(query);
    rbu_request.set_num_per_page(10);
    rbu_request.set_search_id(search_id);
    rbu_request.set_echo(to_str<uint32_t>(seq));

    rbu_request.SerializeToString(req);
    return;
}

void CWorker::CreateLOGRequest(uint32_t seq, const std::string& query, std::string *req)
{
    log_server::LOGRequest log_request;
    log_request.set_log_content(query);
    log_request.set_echo(to_str<uint32_t>(seq));
    log_request.SerializeToString(req);
    return;
}


void CWorker::ReaderQueue()
{
    uint32_t buf_len;
    int ret = 0;
    static uint32_t static_seq = 10000;
    while(!m_reader_send_queue->IsEmpty()) {
        ret = m_reader_send_queue->Pop(m_buffer, m_buffer_size, &buf_len);
        if (ret != 0) {
            break;
        }
        static_seq++;
        uint32_t seq = static_seq;

        std::string tmp;
        //CreateRBURequest(std::string(m_buffer, buf_len), &tmp);
        CreateLOGRequest(seq, std::string(m_buffer, buf_len), &tmp);

        char *package = NULL;
        uint32_t package_len = 0;
        if (m_pack->Pack(10001, seq, 0, tmp.c_str(), tmp.size(), &package, &package_len)) {
            LOG_ERROR(m_logger, "pack failed!");
            return;
        }

        writer_req_header_t header;
        header.ip = m_ip;
        header.port = m_port;
        header.conn_id = 1;

        memcpy(m_buffer, &header, sizeof(header));
        memcpy(m_buffer + sizeof(header), package, package_len);

        if (m_client_send_queue->Push(m_buffer, package_len + sizeof(header)) != 0) {
            LOG_ERROR(m_logger, "push client send queue failed!");
            continue;
        }
        CBaseSession *info = new CQueryInfo(m_reader_recv_queue, seq);
        m_session_mgr->Set(seq, info, m_timeout);
        //LOG_INFO(m_logger, "send pack seq[%d] succ!", seq);
        m_send_num++;
    }
    return;
}


void CWorker::ClientQueue()
{
    uint32_t buf_len;
    while(!m_client_recv_queue->IsEmpty()) {
        int ret = 0;
        ret = m_client_recv_queue->Pop(m_buffer, m_buffer_size, &buf_len);
        if (ret != 0) {
            break;
        }

        m_recv_num++;

        char *package = NULL;
        uint32_t package_len = 0;
        uint16_t service_type = 0;
        uint32_t seq = 0;
        if (m_pack->Unpack(m_buffer + sizeof(writer_rsp_header_t), buf_len - sizeof(writer_rsp_header_t), 0, &package, &package_len, &service_type, &seq)) {
            LOG_P("unpack failed!");
            continue;
        }

        //LOG_INFO(m_logger, "recv pack seq[%d] succ!", seq);
        //ParseRBUResponse(seq, package, package_len);
        ParseLOGResponse(seq, package, package_len);
    }
}

void CWorker::Run()
{
    std::vector<void *> rfds;
    while (!m_stop) {
        m_session_mgr->CheckExpire();
        m_epoll->Wait(&rfds, NULL, 1);
        for (uint32_t i = 0;i < rfds.size(); ++i) {
            worker_epoll_t *data = (worker_epoll_t*) rfds[i];
            data->func(data->priv);
        }
    }
}
