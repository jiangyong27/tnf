
#include "stater.h"
SStaterItem::SStaterItem()
{
    _total_cost_usec = 0;
    _max_cost_usec = 0;
    _min_cost_usec = (uint64_t) 1000 * 1000 * 3600; // 1hour
    _avg_cost_usec = 0;
    _total_num = 0;

    gettimeofday(&_begin_time, NULL);

    _threshold1_num = 0;
    _threshold2_num = 0;
    _threshold3_num = 0;

}
SStaterConf::SStaterConf() {
    _threshold1 = 10000;
    _threshold2 = 50000;
    _threshold3 = 80000;
    _is_mutex = false;
}

CStater::CStater()
{
    _threshold1 = 10000;
    _threshold2 = 50000;
    _threshold3 = 80000;
    _is_mutex = false;
}

CStater::~CStater()
{
    std::map<std::string, SStaterItem *>::iterator iter = _stat.begin();
    for (;iter != _stat.end(); ++iter) {
        delete iter->second;
    }

    if (_is_mutex) {
        pthread_mutex_destroy(&_mutex);
    }
}

bool CStater::Init(const SStaterConf& conf)
{
    _is_mutex = conf._is_mutex;
    _threshold1 = conf._threshold1;
    _threshold2 = conf._threshold2;
    _threshold3 = conf._threshold3;
    if (_is_mutex) {
        pthread_mutex_init(&_mutex, NULL);
    }
    return true;
}

void CStater::BeginStat(const std::string& stat_key)
{
    //Lock();
    std::map<std::string, SStaterItem*>::iterator iter;
    iter = _stat.find(stat_key);
    SStaterItem* stat = NULL;

    if (iter == _stat.end()) {
        try {
            stat = new SStaterItem();
            _stat[stat_key] = stat;
        } catch( ... ) {
            printf("alloc memory error!\n");
            Unlock();
            return ;
        }
    } else {
        stat = iter->second;
    }
    gettimeofday(&stat->_begin_time, NULL);
    //Unlock();
}

void CStater::EndStat(const std::string& stat_key, uint32_t count)
{
    //Lock(); 不支持重入
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    std::map<std::string, SStaterItem*>::iterator iter;
    iter = _stat.find(stat_key);
    SStaterItem* stat = NULL;

    if (iter == _stat.end()) {
        return;
    } else {
        stat = iter->second;
    }

    AddStat(stat, stat->_begin_time, end_time, count);
    //Unlock();
}

void CStater::AddStat(SStaterItem *stat, const struct timeval& st, const struct timeval& ed, uint32_t count)
{
    stat->_total_num += count;
    uint64_t cost_usec = (ed.tv_sec - st.tv_sec) * 1000000 + (ed.tv_usec - st.tv_usec);

    /// 最大值
    if (cost_usec > stat-> _max_cost_usec) {
        stat->_max_cost_usec = cost_usec;
    }

    /// 最小值
    if (cost_usec < stat->_min_cost_usec) {
        stat->_min_cost_usec = cost_usec;
    }

    /// 阈值3个
    if (cost_usec >= _threshold1) {
        stat->_threshold1_num += count;
    }
    if (cost_usec >= _threshold2) {
        stat->_threshold2_num += count;
    }
    if (cost_usec >= _threshold3) {
        stat->_threshold3_num += count;
    }

    stat->_total_cost_usec += cost_usec;
}

void CStater::AddStat(const std::string& stat_key,
             const struct timeval& st,
             const struct timeval& ed,
             uint32_t count)
{
    Lock();
    std::map<std::string, SStaterItem*>::iterator iter;
    iter = _stat.find(stat_key);
    SStaterItem* stat = NULL;

    if (iter == _stat.end()) {
        try {
            stat = new SStaterItem();
            _stat[stat_key] = stat;
        } catch (...) {
            printf("alloc mermory error!\n");
            Unlock();
            return ;
        }
    } else {
        stat = iter->second;
    }
    AddStat(stat, st, ed, count);
    Unlock();
}

void CStater::AddCount(const std::string& stat_key, uint32_t count)
{
    std::map<std::string, SStaterItem*>::iterator iter;
    iter = _stat.find(stat_key);
    SStaterItem* stat = NULL;

    if (iter == _stat.end()) {
        stat = new SStaterItem();
        _stat[stat_key] = stat;
    } else {
        stat = iter->second;
    }

    stat->_total_num += count;
}

void CStater::DelStat(const std::string& stat_key)
{
    std::map<std::string, SStaterItem*>::iterator iter;
    iter = _stat.find(stat_key);
    if (iter != _stat.end()) {
        delete iter->second;
        _stat.erase(iter);
    }
}


std::string CStater::FormatTime(uint64_t cost_usec)
{
    char buf[512];
    if (cost_usec < 1000) {
        snprintf(buf, 512, "%.2f(us)", (float)cost_usec);
    } else if (cost_usec < 1000 * 1000) {
        snprintf(buf, 512, "%.2f(ms)", cost_usec/ (float)1000);
    } else if (cost_usec < 1000 * 1000 * 60) {
        snprintf(buf, 512, "%.2f(s)", cost_usec/ ((float)1000 * 1000));
    } else if (cost_usec < (uint64_t)1000 * 1000 * 3600) {
        snprintf(buf, 512, "%.2f(m)", cost_usec/ ((float)1000 * 1000 * 60));
    } else {
        snprintf(buf, 512, "%.2f(h)", cost_usec/ ((float)1000 * 1000 * 3600));
    }
    return buf;
}

std::string CStater::ToString()
{
    char buf[1024 * 100];
    char szTmpStr1[512];
    char szTmpStr2[512];
    char szTmpStr3[512];
    std::string result;

    snprintf(szTmpStr1, 512, ">%.3fms", _threshold1 / (float)1000);
    snprintf(szTmpStr2, 512, ">%.3fms", _threshold2 / (float)1000);
    snprintf(szTmpStr3, 512, ">%.3fms", _threshold3 / (float)1000);

    snprintf(buf, 1024 * 100, "%-20s|%15s|%15s|%15s|%15s|%15s|%11s|%11s|%11s|\n",
                          "STATKEY","TOTAL(N)","TOTAL","AVG","MAX","MIN",szTmpStr1,szTmpStr2,szTmpStr3);
    result.append(buf);

    std::map<std::string, SStaterItem* >::iterator iter;
    for (iter = _stat.begin(); iter != _stat.end(); ++iter) {
        SStaterItem* item = iter->second;
        if (item->_total_num != 0) {
            item->_avg_cost_usec = item->_total_cost_usec / item->_total_num;
        }
        snprintf(buf, 1024 * 100,"%-20s|%15u|%15s|%15s|%15s|%15s|%11u|%11u|%11u|\n",
                 iter->first.c_str(), item->_total_num,
                 FormatTime(item->_total_cost_usec).c_str(),
                 FormatTime(item->_avg_cost_usec).c_str(),
                 FormatTime(item->_max_cost_usec).c_str(),
                 FormatTime(item->_min_cost_usec).c_str(),
                 item->_threshold1_num, item->_threshold2_num, item->_threshold3_num);
        result.append(buf);
    }
    return result;
}
