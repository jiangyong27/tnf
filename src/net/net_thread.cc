#include "net_thread.h"

Thread::Thread()
{
    m_core = -1;
    m_stop = false;
}

void Thread::Start()
{
    if (0 != pthread_create(&m_pid, NULL, Work, this)) {
        throw std::string("pthread create failed!");
    }
}

void Thread::Join()
{
    pthread_join(m_pid, NULL);
}

void Thread::Bind(int cpu_core)
{
    m_core = cpu_core % sysconf(_SC_NPROCESSORS_CONF);

}

void *Thread::Work(void *param)
{
    Thread *self = (Thread*)(param);

    // bind core
    if (self->m_core >= 0) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(self->m_core, &mask);
        if (pthread_setaffinity_np(self->m_pid, sizeof(mask), &mask) < 0) {
            throw std::string("pthread_setaffinity_np failed!");
        }
        printf("pthread[%u] bind cpu[%d]\n", (uint32_t)self->m_pid, self->m_core);
    }

    self->Run();
    return NULL;
}

