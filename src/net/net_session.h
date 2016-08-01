#ifndef _STORAGE_BASE_TIMER_H_
#define _STORAGE_BASE_TIMER_H_

#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>

#include <tr1/unordered_map>
#include <tr1/unordered_set>
#ifndef HHMap
#define HHMap std::tr1::unordered_map
#endif

#ifndef HHSet
#define HHSet std::tr1::unordered_set
#endif

#ifndef TIME_DIFF_MS
#define TIME_DIFF_MS(a, b) (((a).tv_sec - (b).tv_sec) * 1000 + ((a).tv_usec - (b).tv_usec) / 1000)
#endif

class CBaseSession
{
public:
    CBaseSession(){
        struct timeval now;
        gettimeofday(&now, NULL);
        _is_expire_delete = true;
        _create_time = now;
        _last_time = now;
    }

    virtual ~CBaseSession(){};
    friend class CSessionMgr;

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

class CSessionMgr
{
public:
    CSessionMgr(){};
    virtual ~CSessionMgr(){};
    int Set(uint32_t msg_seq, CBaseSession* timer_info, uint32_t gap_ms = 1000 /* 10 seconds */);
    int Get(uint32_t msg_seq, CBaseSession** timer_info);
    int Exist(uint32_t msg_seq);
    virtual void CheckExpire();
protected:
    HHMap<uint32_t, CBaseSession*> _mp_timer_info;
};



#endif
