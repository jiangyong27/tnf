#ifndef TNF_SESSION_H
#define TNF_SESSION_H
#pragma once
#include "net/net_session.h"
#include "net/net_logger.h"
#include "proto/log_interface.pb.h"

class CAppWorker;
class CAppSession : public CBaseSession
{
public:
    CAppSession(uint32_t connid, CAppWorker *worker, uint32_t tnf_seq);
    ~CAppSession();
    int ParseRequest(const char *data_buf, uint32_t data_len);
    int CreateResponse(char *data_buf, uint32_t *data_len);

    void OnExpire();
    void Reset();
    uint32_t GetPackSeq() {
        return m_pack_seq;
    }
    uint32_t GetConnid() {
        return m_connid;
    }
    uint32_t GetTnfSeq() {
        return m_tnf_seq;
    }

private:
    CAppWorker *m_worker;
    CLogger *m_logger;
    uint32_t m_pack_seq;
    uint32_t m_tnf_seq;
    uint32_t m_connid;

private:
    std::string m_echo;
    uint16_t m_service_type;


};


#endif // TNF_SESSION_H
