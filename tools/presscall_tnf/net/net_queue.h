#ifndef NET_QUEUE_H
#define NET_QUEUE_H
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
class CNetQueue
{
public:
    CNetQueue(bool rlock = false, bool wlock = false, uint32_t buffer_size = 32 * 1024 * 1024);
    ~CNetQueue();
    int Push(const char *data, uint32_t data_len);
    int Pop(char *buf, uint32_t buf_size, uint32_t *data_len);

    std::string DebugString();
public:
    bool IsEmpty() {
        return m_head == m_tail ? true : false;
    }
    int Fd() {
        return m_fd[0];
    }

private:
    int EnQueue(const char *data, uint32_t data_len);
    int DeQueue(char *buf, uint32_t buf_size, uint32_t *data_len);

    void SetNoBlock(int fd);
private:
    int m_fd[2];
	pthread_mutex_t m_rmutex;
	pthread_mutex_t m_wmutex;
    char *m_buffer;
    bool m_wlock;
    bool m_rlock;
    uint32_t m_buffer_len;
    uint32_t m_head;
    uint32_t m_tail;
    uint32_t m_enqueue_num;
    uint32_t m_dequeue_num;

};

#endif // NET_QUEUE_H
