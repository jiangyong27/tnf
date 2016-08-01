#ifndef _DATA_TNF_SRC_APP_WORKER_H
#define _DATA_TNF_SRC_APP_WORKER_H
#pragma once
#include "tnf_worker.h"
#include "app_session.h"
#include "longconn/longconn_helper.h"

#ifndef TIME_DIFF_MS
#define TIME_DIFF_MS(a, b) (((a).tv_sec - (b).tv_sec) * 1000 + ((a).tv_usec - (b).tv_usec) / 1000)
#endif

class CAppWorker : public CTNFWorker
{
public:
    friend class CAppSession;
    virtual ~CAppWorker();
    CAppWorker();
public:
    int Init(const std::string& conf_file, CLogger* logger);
    void Reload();
    void DispatchReader();
    void DispatchWriter();
    uint32_t GetSeq(char *data_buf, uint32_t data_len);
    int RspReader(const char *data_buf, uint32_t data_len, uint32_t conn_id, uint32_t seq);

    void SetWriterQueue(CNetQueue* queue)
    {
        m_writer_send = queue;
    }

    void SetReaderQueue(CNetQueue* recv, CNetQueue* send)
    {
        m_reader_send = send;
        m_reader_recv = recv;
    }

private:
    LongconnHelper* m_longconn_helper;
    char *m_buffer2;
    CNetQueue  *m_reader_recv;
    CNetQueue  *m_reader_send;
    CNetQueue  *m_writer_send;

    uint32_t m_dump_log;

};

#endif // _DATA_TNF_SRC_APP_WORKER_H
