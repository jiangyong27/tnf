#include "app_worker.h"

CAppWorker::CAppWorker()
{
}

CAppWorker::~CAppWorker()
{
}

void CAppWorker::OnWriterQueue(CNetQueue* queue)
{
}

void CAppWorker::OnReaderQueue(CNetQueue *queue)
{
}


bool CAppWorker::Init(const std::string& conf_file, CLogger* logger)
{
    return true;
}

