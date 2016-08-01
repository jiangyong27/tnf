// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-08-22
//
#ifndef _ROOT_MTX_NETFRAME_NET_LOG_H
#define _ROOT_MTX_NETFRAME_NET_LOG_H
#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>

enum {
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_FATAL = 4,

    LOG_CHANGE_MIN = 100,
    LOG_CHANGE_HOUR= 101,
    LOG_CHANGE_DAY = 102,
    LOG_CHANGE_MONTH = 103,
    LOG_CHANGE_NUM = 104,
};

const uint32_t kChangeLine = 1000000; /// 每个文件100w记录
const uint32_t kChangeLineFileNum = 10; /// 保存10个文件

const uint32_t kChangeHourFileNum = 3; /// 按小时保存文件48个
const uint32_t kChangeMonthFileNum = 10; /// 按月保存文件10个
const uint32_t kChangeDayFileNum = 10; /// 按天保存文件10个

void log_p(const char *fmt, ...);

#define LOG_INFO(logger, format, ...) if (logger) logger->Log(LOG_LEVEL_INFO, "[INFO][%s:%d] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(logger, format, ...) if (logger) logger->Log(LOG_LEVEL_FATAL, "[FATAL][%s:%d] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(logger, format, ...) if (logger) logger->Log(LOG_LEVEL_ERROR, "[ERROR][%s:%d] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(logger, format, ...) if (logger) logger->Log(LOG_LEVEL_DEBUG, "[DEBUG][%s:%d] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_P(fmt, ...) log_p("[%s:%d]: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); ///日志打印到标准输入输出

class CLogger
{
public:
    CLogger(const std::string& log_file, bool is_mutex = false);
    ~CLogger();

    bool Init(const std::string& path);
    void Log(int level, const char *format, ...);

    void SetLevel(int level) {
        m_log_level = level;
    }
    void SetChange(int type) {
        m_change_type = type;
    }

private:
    void ChangeFile();
    void ChangeNum();
    void ChangeDay();
    void ChangeMonth();
    void ChangeHour();
    void DeleteHistory();

private:
    std::string m_path;
    int m_fd;
    int m_log_level;
    int m_change_type;
    bool m_is_mutex;
    uint32_t m_count;

    pthread_mutex_t m_mutex;
    char *m_buf;
    uint32_t m_buf_len;
    struct tm m_time;
    struct tm m_last_time;

};
#endif // _ROOT_MTX_NETFRAME_NET_LOG_H
