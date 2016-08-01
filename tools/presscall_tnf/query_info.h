#ifndef _QUERY_INFO_H_
#define _QUERY_INFO_H_

#include <time.h>
#include <stdint.h>
#include <map>
#include "net/net_logger.h"
#include "net/net_session.h"
#include "net/net_queue.h"


#include "reader.h"
class CQueryInfo : public CBaseSession
{
public:
    CQueryInfo(CNetQueue *queue, uint32_t seq);
    virtual ~CQueryInfo();
    void OnExpire();
    uint32_t Cost();
    uint32_t m_seq;
protected:
    CNetQueue *m_queue;
    struct timeval m_create;

};



#endif
