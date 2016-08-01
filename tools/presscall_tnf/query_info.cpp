#include "query_info.h"

CQueryInfo::CQueryInfo(CNetQueue *queue, uint32_t seq)
{
    m_queue = queue;
    m_seq = seq;
    gettimeofday(&m_create, NULL);
}


CQueryInfo::~CQueryInfo()
{
}

void CQueryInfo::OnExpire()
{

    server_result_t result;

    result.retcode = 2;
    result.result_num = 0;
    result.cost = Cost();

    m_queue->Push((char*)&result, sizeof(result));
}
uint32_t CQueryInfo::Cost()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return TIME_DIFF_MS(now, m_create);
}
