#ifndef _STORAGE_BASE_TIMER_H_
#define _STORAGE_BASE_TIMER_H_

#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>

#ifndef TIME_DIFF_MS
#define TIME_DIFF_MS(a, b) (((a).tv_sec - (b).tv_sec) * 1000 + ((a).tv_usec - (b).tv_usec) / 1000)
#endif

class CBaseTimerInfo
{
public:
    CBaseTimerInfo(){
        struct timeval now;
        gettimeofday(&now, NULL);
        _is_expire_delete = true;
        _create_time = now;
        _last_time = now;
    }

    virtual ~CBaseTimerInfo(){};
    friend class CTimerHandler;

public:
    virtual void OnExpire(){ return; };
    virtual void SetDelete(bool del){_is_expire_delete = del;};
    virtual bool GetDelete(){ return _is_expire_delete; };

protected:
    struct timeval _create_time;
    struct timeval _last_time;
    uint32_t _gap;
    uint32_t _msg_seq;
    bool _is_expire_delete;
};

class CTimerHandler
{
public:
    CTimerHandler(){};
    virtual ~CTimerHandler(){};
    int Set(uint32_t msg_seq, CBaseTimerInfo* timer_info, uint32_t gap_ms = 1000 /* 10 seconds */);
    int Get(uint32_t msg_seq, CBaseTimerInfo** timer_info);
    int Exist(uint32_t msg_seq);
    virtual void CheckExpire();
protected:
    std::map<uint32_t, CBaseTimerInfo*> _mp_timer_info;
};



#endif
