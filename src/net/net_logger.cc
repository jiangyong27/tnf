#include "net_logger.h"

/// 打印到标准输出
void log_p(const char *fmt, ...)
{
    time_t timep;
    time(&timep);
    struct tm tmtime;
    localtime_r(&timep,&tmtime);
    printf("[%04d-%02d-%02d %02d:%02d:%02d]",
           1900+tmtime.tm_year,
           tmtime.tm_mon+1,
           tmtime.tm_mday,
           tmtime.tm_hour,
           tmtime.tm_min,
           tmtime.tm_sec
    );

    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    vprintf(fmt, arg_ptr);
    va_end(arg_ptr);
}


CLogger::CLogger(const std::string& log_file, bool is_mutex)
{
    if (is_mutex) {
        pthread_mutex_init(&m_mutex, NULL);
    }
    m_is_mutex = is_mutex;
    m_count = 0;
    m_log_level = LOG_LEVEL_INFO;
    m_change_type = LOG_CHANGE_NUM;

    m_buf_len = 1024 * 100;
    m_buf = new char[m_buf_len];

    time_t timep;
    time(&timep);
    localtime_r(&timep, &m_time);
    localtime_r(&timep, &m_last_time);
    m_fd = -1;

    if (!Init(log_file)) {
        throw std::string("init log failed!");
    }
}

CLogger::~CLogger()
{
    if (m_is_mutex) {
        pthread_mutex_destroy(&m_mutex);
    }
    delete [] m_buf;
    if (m_fd > 0) {
        close(m_fd);
    }
}


/** 初始化*/
bool CLogger::Init(const std::string& path)
{
    m_path = path;
    m_fd = open(m_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (m_fd < 0) {
        printf("open file[%s] failed", path.c_str());
        return false;
    }
    return true;
}

void CLogger::ChangeNum()
{
    if (m_count % kChangeLine != 0 ) {
        return;
    }

    if (m_fd < 0) {
        return ;
    }
    close(m_fd);
    char file1[512];
    char file2[512];
    for(uint32_t i = kChangeLineFileNum - 1; i > 0; i--) {
        snprintf(file1, 512, "%s.%d", m_path.c_str(), i);
        if (i == 1) {
            snprintf(file2, 512, "%s", m_path.c_str());
        } else {
            snprintf(file2, 512, "%s.%d", m_path.c_str(), i-1);
        }
        rename(file2, file1);
    }

    m_fd = open(m_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
}

void CLogger::ChangeHour()
{
    if (m_last_time.tm_hour == m_time.tm_hour) {
        return ;
    }

    if (m_fd < 0) {
        return ;
    }

    close(m_fd);

    char file1[512];
    snprintf(file1, 512, "%s.%04d%02d%02d%02d", m_path.c_str(),
             1900 + m_last_time.tm_year, m_last_time.tm_mon+1, m_last_time.tm_mday, m_last_time.tm_hour);
    rename(m_path.c_str(), file1);

    m_last_time = m_time;
    m_fd = open(m_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
    DeleteHistory();
}

void CLogger::ChangeDay()
{
    if (m_last_time.tm_mday == m_time.tm_mday) {
        return ;
    }

    if (m_fd < 0) {
        return ;
    }
    close(m_fd);

    char file1[512];
    snprintf(file1, 512, "%s.%04d%02d%02d", m_path.c_str(),
             1900 + m_last_time.tm_year, m_last_time.tm_mon+1, m_last_time.tm_mday);
    rename(m_path.c_str(), file1);
    m_last_time = m_time;

    m_fd = open(m_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
    DeleteHistory();
}

void CLogger::ChangeMonth()
{
    if (m_last_time.tm_mon == m_time.tm_mon) {
        return ;
    }

    if (m_fd < 0) {
        return ;
    }
    close(m_fd);

    char file1[512];
    snprintf(file1, 512, "%s.%04d%02d", m_path.c_str(),
             1900 + m_last_time.tm_year, m_last_time.tm_mon+1);
    rename(m_path.c_str(), file1);
    m_last_time = m_time;

    m_fd = open(m_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
    DeleteHistory();
}

void CLogger::DeleteHistory()
{
    char file1[512];
    time_t timep;
    time(&timep);

    if (m_change_type == LOG_CHANGE_HOUR) {
        timep -= kChangeHourFileNum * 3600;
    } else if (m_change_type == LOG_CHANGE_DAY) {
        timep -= kChangeHourFileNum * 3600 * 24;
    } else if(m_change_type == LOG_CHANGE_MONTH) {
        timep -= kChangeHourFileNum * 3600 * 24 * 30;
    } else {
        return ;
    }

    struct tm mtime;
    localtime_r(&timep, &mtime);

    if (m_change_type == LOG_CHANGE_HOUR) {
        snprintf(file1, 512, "%s.%04d%02d%02d%02d", m_path.c_str(),
                 1900 + mtime.tm_year, mtime.tm_mon+1, mtime.tm_mday, mtime.tm_hour);
    } else if (m_change_type == LOG_CHANGE_DAY) {
        snprintf(file1, 512, "%s.%04d%02d%02d", m_path.c_str(),
                 1900 + mtime.tm_year, mtime.tm_mon+1, mtime.tm_mday);
    } else if (m_change_type == LOG_CHANGE_MONTH) {
        snprintf(file1, 512, "%s.%04d%02d", m_path.c_str(),
                 1900 + mtime.tm_year, mtime.tm_mon+1);
    }

    remove(file1);

}

/** 切换文件写日志*/
void CLogger::ChangeFile()
{
    if (m_change_type == LOG_CHANGE_NUM) {
        ChangeNum();
    } else if (m_change_type == LOG_CHANGE_HOUR) {
        ChangeHour();
    } else if (m_change_type == LOG_CHANGE_MONTH) {
        ChangeMonth();
    } else if (m_change_type == LOG_CHANGE_DAY) {
        ChangeDay();
    }
}

/** 写日志到文件*/
void CLogger::Log(int level, const char *format, ...)
{
    if (m_fd < 0) {
        return ;
    }

    if (level < m_log_level)
        return ;

    if (m_is_mutex) {
        pthread_mutex_lock(&m_mutex);
    }

	va_list ap;
	va_start(ap, format);
	vsnprintf(m_buf, m_buf_len, format, ap);
	va_end(ap);

    time_t timep;
    time(&timep);
    localtime_r(&timep, &m_time);

    char time_tmp[30];
    int len = snprintf(time_tmp, 30, "[%04d-%02d-%02d %02d:%02d:%02d]", 1900+m_time.tm_year, m_time.tm_mon+1, \
           m_time.tm_mday, m_time.tm_hour, m_time.tm_min, m_time.tm_sec);

    m_count++;
    ChangeFile();

    write(m_fd, time_tmp, len);
    write(m_fd, m_buf, strlen(m_buf));

    /// unlock file
    if (m_is_mutex) {
        pthread_mutex_unlock(&m_mutex);
    }
}
