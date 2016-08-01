#include "app_session.h"
#include "app_worker.h"

CAppSession::CAppSession(uint32_t connid, CAppWorker* worker, uint32_t tnf_seq)
{
    m_connid = connid;
    m_worker = worker;
    m_tnf_seq = tnf_seq;
    m_logger = m_worker->m_logger;
}

CAppSession::~CAppSession()
{
}

void CAppSession::OnExpire()
{
}

void CAppSession::Reset()
{
    m_tnf_seq = 0;
    m_pack_seq = 0;
}

int CAppSession::ParseRequest(const char *data_buf, uint32_t data_len)
{
    char *package_buf = NULL;
    uint32_t package_len = 0;
    int ret = m_worker->m_longconn_helper->Unpack(data_buf, data_len, 0, &package_buf, &package_len, &m_service_type, &m_pack_seq);
    if (ret != 0) {
        LOG_ERROR(m_logger, "longconn unpack failed!");
        return -1;
    }

    log_server::LogRequest log_request;
    if (!log_request.ParseFromArray(package_buf, package_len)) {
        LOG_ERROR(m_logger, "parse pb failed!");
        return -1;
    }

    m_echo = log_request.echo();
    if (m_worker->m_dump_log != 0) {
        m_worker->m_writer_send->Push(log_request.log_content().c_str(), log_request.log_content().size());
    }
    return 0;
}

int CAppSession::CreateResponse(char *data_buf, uint32_t *data_len)
{
    log_server::LogResponse log_response;
    log_response.set_retcode(0);
    log_response.set_retmsg("ok");
    log_response.set_echo(m_echo);

    std::string tmp;
    log_response.SerializeToString(&tmp);
    char *package_buf = NULL;
    uint32_t package_len = 0;

    m_worker->m_longconn_helper->Pack(m_service_type, m_pack_seq, 0, tmp.c_str(),tmp.size(), &package_buf, &package_len);
    memcpy(data_buf, package_buf, package_len);
    *data_len = package_len;
    return 0;
}
