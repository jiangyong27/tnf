#ifndef _DATA_PREPROCESS_SRC_PREPROCESS_STATER_H
#define _DATA_PREPROCESS_SRC_PREPROCESS_STATER_H
#pragma once
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <map>

struct SStaterItem {
    uint64_t _total_cost_usec;
    uint64_t _max_cost_usec;
    uint64_t _min_cost_usec;
    uint64_t _avg_cost_usec;

    uint32_t _total_num;
    uint32_t _threshold1_num;
    uint32_t _threshold2_num;
    uint32_t _threshold3_num;

    struct timeval _begin_time;
    SStaterItem();
};

struct SStaterConf
{
    bool _is_mutex;
    uint32_t _threshold1;
    uint32_t _threshold2;
    uint32_t _threshold3;
    SStaterConf();
};

class CStater
{
public:
    CStater();
    ~CStater();
    bool Init(const SStaterConf& conf);
    void AddCount(const std::string& stat_key, uint32_t count = 1);
    void BeginStat(const std::string& stat_key);
    void EndStat(const std::string& stat_key, uint32_t count = 1);
    void AddStat(const std::string& stat_key,
             const struct timeval& st,
             const struct timeval& ed,
             uint32_t count = 1);
    void DelStat(const std::string& stat_key);
    std::string ToString();
private:
    void Lock() {
        if (_is_mutex) {
            pthread_mutex_lock(&_mutex);
        }
    }
    void Unlock() {
        if (_is_mutex) {
            pthread_mutex_unlock(&_mutex);
        }
    }
    void AddStat(SStaterItem *stat, const struct timeval& st, const struct timeval& ed, uint32_t count);
    std::string FormatTime(uint64_t cost);
private:
    pthread_mutex_t _mutex;
    bool _is_mutex;
    std::map<std::string, SStaterItem*> _stat;
    uint64_t _threshold1;
    uint64_t _threshold2;
    uint64_t _threshold3;
};

#endif // _DATA_PREPROCESS_SRC_PREPROCESS_STATER_H
