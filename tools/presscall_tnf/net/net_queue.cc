#include "net_queue.h"

CNetQueue::CNetQueue(bool rlock, bool wlock, uint32_t buffer_size)
{
    m_buffer = NULL;
    m_buffer_len = buffer_size;
    m_buffer = new char[m_buffer_len];
    m_wlock = wlock;
    m_rlock = rlock;
    m_head = 0;
    m_tail = 0;
    m_dequeue_num = 0;
    m_enqueue_num = 0;
    if (m_wlock) {
	    pthread_mutex_init(&m_wmutex, NULL);
    }
    if (m_rlock) {
	    pthread_mutex_init(&m_rmutex, NULL);
    }

    if (pipe(m_fd)) {
        perror("pipe failed!");
        assert(0);
    }
    SetNoBlock(m_fd[0]);
    SetNoBlock(m_fd[1]);
}


CNetQueue::~CNetQueue()
{
    delete [] m_buffer;
    m_buffer = NULL;
    if (m_wlock) {
	    pthread_mutex_destroy(&m_wmutex);
    }
    if (m_rlock) {
	    pthread_mutex_destroy(&m_rmutex);
    }
}

void CNetQueue::SetNoBlock(int fd)
{
    int opts;
    opts = fcntl(fd, F_GETFL);
    if(opts < 0) {
        perror("fcntl(F_GETFL)\n");
        assert(0);
    }
    opts = (opts | O_NONBLOCK | O_NDELAY);
    if(fcntl(fd, F_SETFL, opts) < 0) {
        perror("fcntl(F_SETFL)\n");
        assert(0);
    }
}

int CNetQueue::EnQueue(const char *data, uint32_t data_len)
{
    uint32_t head = m_head;
    uint32_t tail = m_tail;

    uint32_t total_len = data_len + sizeof(data_len);
    uint32_t free_len = head > tail ? head - tail: head + m_buffer_len - tail;
    uint32_t tail_len = m_buffer_len - tail;
    if (free_len <= total_len) {
        return -1;
    }

    if (tail_len >= total_len) {
        memcpy(m_buffer + tail, &data_len, sizeof(data_len));
        memcpy(m_buffer + tail + sizeof(data_len), data, data_len);
        m_tail += data_len + sizeof(data_len);
    } else if (tail_len >= sizeof(data_len)) {
        uint32_t first_len = tail_len - sizeof(data_len);
        memcpy(m_buffer + tail, &data_len, sizeof(data_len));
        memcpy(m_buffer + tail + sizeof(data_len), data, first_len);
        memcpy(m_buffer, data + first_len, data_len - first_len);
        m_tail = tail + data_len + sizeof(data_len) - m_buffer_len;
    } else {
        uint32_t second_len = sizeof(data_len) - tail_len;
        memcpy(m_buffer + tail, &data_len, tail_len);
        memcpy(m_buffer, (char *)&data_len + tail_len, second_len);
        memcpy(m_buffer + second_len, data, data_len);
        m_tail = second_len + data_len;
    }
    if (m_tail >= m_buffer_len){
        m_tail = 0;
    }

    write(m_fd[1], "\0", 1);

    return 0;
}

int CNetQueue::DeQueue(char *buf, uint32_t buf_size, uint32_t *data_len)
{
    uint32_t head = m_head;
    uint32_t tail = m_tail;
    if (head == tail) {
        *data_len = 0;
        char c;
        read(m_fd[0], &c, 1);
        return -1;
    }

    uint32_t real_len = 0;
    uint32_t first_len = 0;
    uint32_t second_len = 0;
    uint32_t used_len = tail > head ? tail - head : tail + m_buffer_len - head;
    uint32_t next = 0;
    if (buf_size < sizeof(uint32_t)) {
        return -2;
    }

    if (head + sizeof(uint32_t) >= m_buffer_len) {
        first_len = m_buffer_len - head;
        second_len = sizeof(uint32_t) - first_len;
        memcpy(&real_len, m_buffer + head, first_len);
        memcpy((char *)&real_len + first_len, m_buffer, second_len);
        next = second_len;
    } else {
        memcpy(&real_len, m_buffer + head, sizeof(uint32_t));
        next = head + sizeof(uint32_t);
    }

    *data_len = real_len;
    assert(used_len >= real_len + sizeof(uint32_t));
    if (real_len + sizeof(uint32_t) > buf_size) {
        *data_len = real_len + sizeof(uint32_t);
        return -3;
    }

    if (next + real_len <= m_buffer_len) {
        memcpy(buf, m_buffer + next, real_len);
        m_head = next + real_len;
    } else {
        first_len = m_buffer_len - next;
        memcpy(buf, m_buffer + next, first_len);
        memcpy(buf + first_len, m_buffer, real_len - first_len);
        m_head = real_len - first_len;
    }
    if (m_head >= m_buffer_len){
        m_head = 0;
    }

    char c;
    read(m_fd[0], &c, 1);
    return 0;
}

int CNetQueue::Pop(char *buf, uint32_t buf_size, uint32_t *data_len)
{
    //printf("wlock[%d],rlock[%d]\n", m_wlock,m_rlock);
    int ret;
    if (m_rlock) {
	    if (pthread_mutex_lock(&m_rmutex) == -1) {
	        pthread_mutex_unlock(&m_rmutex);
            return -100;
        }
        ret = DeQueue(buf, buf_size, data_len);
        if (ret == 0) m_dequeue_num++;
	    pthread_mutex_unlock(&m_rmutex);
    } else {
        ret = DeQueue(buf, buf_size,  data_len);
        if (ret == 0) m_dequeue_num++;
    }
    return ret;
}

int CNetQueue::Push(const char *data, uint32_t data_len)
{
    int ret = 0;
    if (m_wlock) {
	    if (pthread_mutex_lock(&m_wmutex) == -1) {
	        pthread_mutex_unlock(&m_wmutex);
            return -100;
        }
        ret = EnQueue(data, data_len);
        if (ret == 0) m_enqueue_num++;
	    pthread_mutex_unlock(&m_wmutex);
    } else {
        ret = EnQueue(data, data_len);
        if (ret == 0) m_enqueue_num++;
    }
    return ret;
}

std::string CNetQueue::DebugString()
{
    char buf[10240];
    snprintf(buf, 10240, "push_num=%d,pop_num=%d,size=%d,head[%d],tail[%d]",
             m_enqueue_num, m_dequeue_num, m_enqueue_num - m_dequeue_num,
             m_head, m_tail);
    return buf;
}
