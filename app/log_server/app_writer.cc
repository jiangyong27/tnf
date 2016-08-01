#include "app_writer.h"
CAppWriter::CAppWriter()
{
    m_buffer_size = 1024 * 1024 * 2;
    m_buffer = new char[m_buffer_size];
    m_epoll = new CEpoll(10);
    m_path_prefix = "../data/";
    m_pre_time = 0;
    m_fd = -1;
}

CAppWriter::~CAppWriter()
{
    delete m_buffer;
}

int CAppWriter::SwitchToNextFile()
{
    struct tm local_time = {0};
    time_t now;
    time(&now);
    localtime_r(&now, &local_time);

    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%4d%02d%02d_%02d", local_time.tm_year + 1900,
             local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour);
    std::string cur_path = m_path_prefix + buf;

    if (m_fd >= 0) {
        close(m_fd);
    }

    m_fd = open(cur_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (m_fd < 0) {
        return -1;
    }

    return 0;
}

bool CAppWriter::IsSwitchFile() {
    time_t time_now;
    time(&time_now);
    if (time_now - m_pre_time >=3600 || time_now % 3600 == 0){
        m_pre_time = time_now;
        return true;
    }
    return false;
}


bool CAppWriter::Write(const char* data, size_t len) {

    if (IsSwitchFile()) {
        if (0 != SwitchToNextFile()) {
            return false;
        }
    }

    if (write(m_fd, data, len) != static_cast<ssize_t>(len)) {
        return false;
    }

    return true;
}

void CAppWriter::OnQueue()
{
    uint32_t data_len = 0;
    int ret = 0;
    do {
        ret = m_queue->Pop(m_buffer, m_buffer_size, &data_len);
        if (ret != 0) break;
        Write(m_buffer, data_len);
    } while(!m_queue->IsEmpty());
}

void CAppWriter::Run()
{
    m_epoll->Add(m_queue->Fd(), true, false);
    std::vector<int> rfds;
    while (1) {
        m_epoll->Wait(&rfds, NULL, 1);
        OnQueue();
    }
}
