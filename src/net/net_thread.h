// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2015-01-05
//

#ifndef NET_PTHREAD_H
#define NET_PTHREAD_H

#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <iostream>

class Thread
{
public:
    Thread();
    virtual ~Thread() {}
    void Start();
    void Join();
    void Stop() { m_stop = true;}
    void Bind(int cpu_core);
    virtual void Run() = 0;

private:
    static void *Work(void *param);
protected:
    pthread_t m_pid;
    bool m_stop;
    int32_t m_core;
};


#endif

