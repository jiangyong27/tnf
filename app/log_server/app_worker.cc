#include "app_worker.h"

CAppWorker::CAppWorker()
{
    m_longconn_helper = new LongconnHelper();
    m_buffer2 = NULL;
    m_writer_queue = NULL;
    m_dump_log = 0;
}

CAppWorker::~CAppWorker()
{
    if (m_buffer2) delete [] m_buffer2;
    delete m_longconn_helper;
}

uint32_t CAppWorker::GetSeq(char *data_buf, uint32_t data_len)
{
    LongconnHelper pack_tool;
	uint16_t service_type;
	uint32_t seq = -1;

	pack_tool.Unpack(data_buf, data_len, &service_type, &seq);
    return seq;
}


int CAppWorker::Init(const std::string& conf_file, CLogger* logger)
{
    int ret = CTNFWorker::Init(conf_file, logger);
    if (ret != 0) {
        return ret;
    }
    CNetConf net_conf;
    net_conf.OpenIniFile(m_conf_file);
    net_conf.GetFiledValue("worker", "dump_log", 0, &m_dump_log);

    m_buffer2 = new char[m_buffer_size];
    return 0;
}



int CAppWorker::RspReader(const char *data_buf, uint32_t data_len, uint32_t conn_id, uint32_t seq)
{
    //reader_rsp_t *rsp_header = (reader_rsp_t *) m_buffer;
    //rsp_header->conn_id = conn_id;

    reader_rsp_t rsp_header;
    gettimeofday(&rsp_header.create_time, NULL);
    rsp_header.conn_id = conn_id;
    rsp_header.seq = seq;
    memcpy(m_buffer2, &rsp_header, sizeof(rsp_header));
    memcpy(m_buffer2 + sizeof(rsp_header), data_buf, data_len);
    int ret = m_reader_send->Push(m_buffer2, sizeof(rsp_header) + data_len);
    return ret;
}

void CAppWorker::DispatchWriter()
{
    //todo
}

void CAppWorker::DispatchReader()
{
    uint32_t data_len;
    do {
        int ret = m_reader_recv->Pop(m_buffer2, m_buffer_size, &data_len);
        if (ret != 0) break;

        if (data_len <= sizeof(reader_req_t)) {
            LOG_ERROR(m_logger, "data_len too small!data_len[%d]", data_len);
            break;
        }

        reader_req_t *req_header = (reader_req_t*) m_buffer2;
        struct timeval now;
        gettimeofday(&now, NULL);
        if (TIME_DIFF_MS(now, req_header->create_time) >= m_queue_timeout) {
            LOG_ERROR(m_logger, "pop queue [%d]ms,timeout[%d],seq[%d]",
                      TIME_DIFF_MS(now, req_header->create_time), m_queue_timeout,
                      GetSeq(m_buffer2 + sizeof(reader_req_t),data_len - sizeof(reader_req_t)));
        }

        CAppSession* session = new CAppSession(req_header->conn_id, this, req_header->seq);
        ret = session->ParseRequest(m_buffer2 + sizeof(reader_req_t), data_len - sizeof(reader_req_t));
        if (ret < 0) {
            delete session;
            return;
        }

        if (ret == 0) {
            ret = session->CreateResponse(m_buffer, &data_len);
            if (ret == 0) {
                ret = RspReader(m_buffer, data_len, session->GetConnid(), session->GetTnfSeq());
                if (ret != 0) LOG_ERROR(m_logger, "RspReader failed!ret=%d", ret);
            } else {
                LOG_ERROR(m_logger, "CreateResponse failed!ret=%d", ret);
            }
            delete session;
        } else {
            ///todo
            m_session_mgr->Set(session->GetPackSeq(), session, 10000);
        }
    } while (!m_reader_recv->IsEmpty());
    return;
}



