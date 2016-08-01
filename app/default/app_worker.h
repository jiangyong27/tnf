#ifndef _DATA_TNF_SRC_APP_WORKER_H
#define _DATA_TNF_SRC_APP_WORKER_H
#pragma once
#include "tnf_worker.h"

class CAppWorker : public CTNFWorker
{
public:
    virtual ~CAppWorker();
    CAppWorker();
    void OnWriterQueue(CNetQueue *queue);
    void OnReaderQueue(CNetQueue *queue);
    bool Init(const std::string& conf_file, CLogger* logger);
private:
};

#endif // _DATA_TNF_SRC_APP_WORKER_H
