// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2015-01-05
//

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <sched.h>
#include <stdio.h>
#include <stdint.h>

/** Mutex for pthead */
class Mutex
{
public:
	Mutex();
	virtual ~Mutex();

	virtual void Lock() const;
	virtual void Unlock() const;

private:
	mutable pthread_mutex_t m_mutex;
};

/** Semaphore for pthread */
typedef unsigned int value_t;
class Semaphore
{
public:
	Semaphore(value_t start_val = 0);
	~Semaphore();

	/** return 0 if successful */
	int Post();
	/** Wait for Post return 0 if successful */
	int Wait();

	/** Not implemented for win32 */
	int TryWait();

	/** Not implemented for win32 */
	int GetValue();

private:
	Semaphore(const Semaphore& ) {} // copy constructor
	Semaphore& operator=(const Semaphore& ) { return *this; } // assignment operator
	sem_t m_sem;
};



#endif // _SOCKETS_Semaphore_H

