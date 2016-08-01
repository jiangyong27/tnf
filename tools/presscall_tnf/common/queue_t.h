#ifndef TSHP_HU_QUEUE_T_H
#define TSHP_HU_QUEUE_T_H

#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <queue>

template <typename T>
class CQueueT
{
public :
    CQueueT();
    ~CQueueT();
    int Init(unsigned int udwMaxNodeNum);
    int Uninit();
public :
    /**
     *  ���õȴ�ֱ��Push�ɹ�
     *  \args
     *    1> [pTask] <in> ����ڵ��ָ��
     *  \return
     *    0�ɹ� -1ʧ��
     */
    int WaitTillPush(const T& Node);
    /**
     *  ���õȴ�ֱ��Pop�ɹ�
     *  \args
     *    1> [ppTask] <out> ��ȡ�ڵ�ָ���ָ��
     *  \return
     *    0�ɹ� -1ʧ��
     */
    int WaitTillPop(T& Node);

    /**
     *  �ȴ�һ��ʱ��ֱ��Push�ɹ�
     *  \args
     *    1> [pTask] <in> ����ڵ��ָ��
     *    2> [usec]  <in> �ȴ�ʱ�� ��΢��
     *  \return
     *    0�ɹ� -1ʧ��
     */
    int WaitTimePush(const T& Node, unsigned int usec);

    /**
     *  �ȴ�һ��ʱ��ֱ��Pop�ɹ�
     *  \args
     *    1> [ppTask] <out> ��ȡ�ڵ�ָ���ָ��
     *    2> [usec]  <in> �ȴ�ʱ�� ��΢��
     *  \return
     *    0�ɹ� -1ʧ��
     */
    int WaitTimePop(T& Node, unsigned int usec);

    /**
     *  �����Ƿ�Ϊ��
     *  \return
     *    1Ϊ�� 0��Ϊ��
     */
    int BeEmpty();

    /**
     *  ���нڵ����
     *  \return
     *    ���нڵ����
     */
    unsigned int Size();

private :
    std::queue<T> m_queue;
    //���ڽڵ�ĸ���
    sem_t m_semExist;
    //�սڵ�ĸ���
    sem_t m_semEmpty;
    //���л�����
    pthread_mutex_t m_mtxQue;
    //���нڵ�������
    unsigned int m_udwMaxNodeNum;
};

template <typename T>
CQueueT<T>::CQueueT()
{
    m_udwMaxNodeNum = 0;
}

template <typename T>
CQueueT<T>::~CQueueT()
{
    // do nothing
}

template <typename T>
int CQueueT<T>::Init(unsigned int udwMaxNodeNum)
{
    int result = 0;
    m_udwMaxNodeNum = udwMaxNodeNum;
    result = sem_init(&m_semExist, 0, 0);
    if (result == -1)
    {
        return -1;
    }
    result = sem_init(&m_semEmpty, 0, m_udwMaxNodeNum);
    if (result == -1)
    {
        return -1;
    }

    result = pthread_mutex_init(&m_mtxQue, NULL);
    if (result == -1)
    {
        return -1;
    }
    return 0;
}

template <typename T>
int CQueueT<T>::Uninit()
{
    // do nothing.
    return 0;
}

template <typename T>
int CQueueT<T>::WaitTillPush(const T& Node)
{
    int flag = 0;
    while (1)
    {
        flag = sem_wait(&m_semEmpty);
        if (-1 == flag)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            break;
        }
    }
    flag = pthread_mutex_lock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    m_queue.push(Node);
    flag = sem_post(&m_semExist);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    flag = pthread_mutex_unlock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    return 0;
}

template <typename T>
int CQueueT<T>::WaitTillPop(T& Node)
{
    int flag = 0;
    while (1)
    {
        flag = sem_wait(&m_semExist);
        if (-1 == flag)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            break;
        }
    }
    flag = pthread_mutex_lock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    Node = m_queue.front();
    m_queue.pop();
    flag = sem_post(&m_semEmpty);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    flag = pthread_mutex_unlock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    return 0;
}

template <typename T>
int CQueueT<T>::WaitTimePush(const T& Node, unsigned int usec)
{
    struct timespec ts;
    ts.tv_sec = time(NULL);
    ts.tv_nsec = 0;
    if (usec >= 1000000)
    {
        ts.tv_sec += usec / 1000000;
        ts.tv_nsec = (usec % 1000000) * 1000;
    }
    else
    {
        ts.tv_nsec = usec * 1000;
    }
    int flag = 0;
    while (1)
    {
        flag =  sem_timedwait(&m_semEmpty, &ts);
        if (-1 == flag)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else if (errno == ETIMEDOUT)
            {
                return -1;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            break;
        }
    }
    flag = pthread_mutex_lock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    m_queue.push(Node);
    flag = sem_post(&m_semExist);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    flag = pthread_mutex_unlock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    return 0;
}

template <typename T>
int CQueueT<T>::WaitTimePop(T& Node, unsigned int usec)
{
    struct timespec ts;
    ts.tv_sec = time(NULL);
    ts.tv_nsec = 0;
    if (usec >= 1000000)
    {
        ts.tv_sec += usec / 1000000;
        ts.tv_nsec = (usec % 1000000) * 1000;
    }
    else
    {
        ts.tv_nsec = usec * 1000;
    }
    int flag = 0;
    while (1)
    {
        flag =  sem_timedwait(&m_semExist, &ts);
        if (-1 == flag)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else if (errno == ETIMEDOUT)
            {
                return -1;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            break;
        }
    }
    flag = pthread_mutex_lock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    Node = m_queue.front();
    m_queue.pop();
    flag = sem_post(&m_semEmpty);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    flag = pthread_mutex_unlock(&m_mtxQue);
    if (flag == -1)
    {
        pthread_mutex_unlock(&m_mtxQue);
        return -1;
    }
    return 0;
}

template <typename T>
int CQueueT<T>::BeEmpty()
{
    if (m_queue.empty())
        return 1;
    else
        return 0;
}

template <typename T>
unsigned int CQueueT<T>::Size()
{
    return (unsigned int)m_queue.size();
}

#endif ///< __QUEUE_T_H

