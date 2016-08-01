// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2015-01-05
//

#include "pthread.h"

/** Mutex */
Mutex::Mutex()
{
	pthread_mutex_init(&m_mutex, NULL);
}


Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}


void Mutex::Lock() const
{
	pthread_mutex_lock(&m_mutex);
}


void Mutex::Unlock() const
{
	pthread_mutex_unlock(&m_mutex);
}


/** Semaphore */
Semaphore::Semaphore(value_t start_val)
{
	sem_init(&m_sem, 0, start_val);
}


Semaphore::~Semaphore()
{
	sem_destroy(&m_sem);
}


int Semaphore::Post()
{
	return sem_post(&m_sem);
}


int Semaphore::Wait()
{
	return sem_wait(&m_sem);
}


int Semaphore::TryWait()
{
	return sem_trywait(&m_sem);
}


int Semaphore::GetValue()
{
    int value;
	if(sem_getvalue(&m_sem, &value)==0)
        return value;
    return -1;
}



