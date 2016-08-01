#include "tnf_worker.h"

CTNFWorker::CTNFWorker()
{
    m_epoll = new CEpoll(10);
    m_session_mgr = new CSessionMgr();
    m_reader_queue = NULL;
    m_writer_queue = NULL;
    m_buffer_size = 0;
    m_buffer = NULL;
    m_mqs_num = 0;
    static uint32_t idx = 0;
    m_idx = idx++;
}

CTNFWorker::~CTNFWorker()
{
    delete m_epoll;
    if (m_buffer) delete [] m_buffer;
}

int CTNFWorker::Init(const std::string &conf_file, CLogger* logger)
{
    m_conf_file = conf_file;
    m_logger = logger;
    //Bind(m_idx+1);
    CNetConf net_conf;
    net_conf.OpenIniFile(m_conf_file);
    net_conf.GetFiledValue("worker", "buffer_size", 2, &m_buffer_size);
    net_conf.GetFiledValue("worker", "queue_timeout", 100, &m_queue_timeout);

    m_buffer_size = m_buffer_size * 1000 * 1000;
    if (m_buffer != NULL) {
        delete [] m_buffer;
    }
    m_buffer = new char[m_buffer_size];
    return 0;
}


int CTNFWorker::AddMQDispatch(CNetQueue* queue, disp_func func)
{
    SMQInfo *mq = &m_mqs[m_mqs_num++];
    mq->mq = queue;
    mq->priv = this;
    mq->func = func;
    mq->is_active = false;
    m_epoll->Add(queue->Fd(), mq, true, false);
}

void CTNFWorker::Run()
{
    std::vector<void*> rfds;
    while (!m_stop) {
        rfds.clear();
        m_epoll->Wait(&rfds, NULL, 1);
        for (uint32_t i = 0; i < rfds.size(); ++i) {
            SMQInfo* data = (SMQInfo*) rfds[i];
            data->func(data->priv);
            data->is_active = true;
        }
        for (uint32_t i = 0; i < m_mqs_num;i++) {
            SMQInfo* data = &m_mqs[i];
            if (data->is_active == false) {
                data->func(data->priv);
            } else {
                data->is_active = false;
            }
        }
        m_session_mgr->CheckExpire();
    }
}
