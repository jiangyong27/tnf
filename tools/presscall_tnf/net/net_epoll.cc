#include "net_epoll.h"

CEpoll::CEpoll(int num)
{
    m_max_fd = num;
    m_epoll_fd = epoll_create(num);
    if (m_epoll_fd == -1) {
        perror("epoll_create failed!");
        assert(0);
    }
    m_events = new struct epoll_event[num];
    m_count = 0;
}

CEpoll::~CEpoll()
{
    delete [] m_events;
}

int CEpoll::Size()
{
    return m_count;
}

void CEpoll::Add(int fd, void *ptr, bool bRead, bool bWrite)
{
    struct epoll_event ev;
    ev.events = 0;
    if (bRead) {
        ev.events |= EPOLLIN;
    }
    if (bWrite){
        ev.events |= EPOLLOUT;
    }
    ev.data.ptr = ptr;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        printf("epoll_ctl add failed!fd[%d],error[%s]\n", fd, strerror(errno));
        return;
    }
    m_count++;
}

void CEpoll::Add(int fd, bool bRead, bool bWrite)
{
    struct epoll_event ev;
    ev.events = 0;
    if (bRead) {
        ev.events |= EPOLLIN;
    }
    if (bWrite){
        ev.events |= EPOLLOUT;
    }
    //ev.events = (bRead ? EPOLLIN : 0) | (bWrite ? EPOLLOUT : 0) | EPOLLHUP | EPOLLERR;
    ev.data.fd = fd;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        printf("epoll_ctl add failed!fd[%d],error[%s]\n", fd, strerror(errno));
        return ;
    }
    m_count++;
}

void CEpoll::Mod(int fd, void *ptr, bool bRead, bool bWrite)
{
    struct epoll_event ev;
    ev.events = 0;
    if (bRead) {
        ev.events |= EPOLLIN;
    }
    if (bWrite){
        ev.events |= EPOLLOUT;
    }
    //ev.events = (bRead ? EPOLLIN : 0) | (bWrite ? EPOLLOUT : 0);
    //ev.events = (bRead ? EPOLLIN : 0) | (bWrite ? EPOLLOUT : 0) | EPOLLHUP | EPOLLERR;
    ev.data.ptr = ptr;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        printf("epoll_ctl mod failed!fd[%d],error[%s]\n", fd, strerror(errno));
    }
}

void CEpoll::Mod(int fd, bool bRead, bool bWrite)
{
    struct epoll_event ev;
    //ev.events = (bRead ? EPOLLIN : 0) | (bWrite ? EPOLLOUT : 0);
    ev.events = 0;
    if (bRead) {
        ev.events |= EPOLLIN;
    }
    if (bWrite){
        ev.events |= EPOLLOUT;
    }
    //ev.events = (bRead ? EPOLLIN : 0) | (bWrite ? EPOLLOUT : 0) | EPOLLHUP | EPOLLERR;
    ev.data.fd = fd;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        printf("epoll_ctl mod failed!fd[%d],error[%s]\n", fd, strerror(errno));
    }
}

void CEpoll::Hup(int fd, void *ptr)
{
    struct epoll_event ev;
    ev.events = EPOLLHUP | EPOLLERR | EPOLLIN;
    ev.data.ptr = ptr;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        printf("epoll_ctl mod failed!fd[%d],error[%s]\n", fd, strerror(errno));
    }
    return ;
}

void CEpoll::Del(int fd, bool is_close)
{
    struct epoll_event ev;
    ev.events = 0;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, &ev) == -1) {
        printf("epoll_ctl del failed!fd[%d],error[%s]\n", fd, strerror(errno));
        return ;
    }
    if (is_close) {
        close(fd);
    }
    m_count--;
}


void CEpoll::Wait(std::vector<void *> *reads, std::vector<void*> *writes, int timeout)
{
    int nfds;
    void *ptr;
    if (reads) reads->clear();
    if (writes) writes->clear();

    nfds = epoll_wait(m_epoll_fd, m_events, m_max_fd, timeout);
    if  (nfds == -1 && errno != EINTR) {
        perror("epoll_wait failed!");
        assert(0);
    }
    for (int i = 0; i < nfds; i++) {
        ptr = m_events[i].data.ptr;

        if (m_events[i].events & EPOLLIN || m_events[i].events & EPOLLHUP || m_events[i].events & EPOLLERR) {
            if (reads) reads->push_back(ptr);
            continue;
        }

        if (m_events[i].events & EPOLLOUT) {
            if (writes) writes->push_back(ptr);
            continue;
        }
    }
}


void CEpoll::Wait(std::vector<int> *reads, std::vector<int> *writes, int timeout)
{
    int nfds;
    int ptr;
    if (reads) reads->clear();
    if (writes) writes->clear();

    nfds = epoll_wait(m_epoll_fd, m_events, m_max_fd, timeout);
    if  (nfds == -1 && errno != EINTR) {
        perror("epoll_wait failed!");
        assert(0);
    }
    for (int i = 0; i < nfds; i++) {
        ptr = m_events[i].data.fd;

        if (m_events[i].events & EPOLLIN || m_events[i].events & EPOLLHUP || m_events[i].events & EPOLLERR) {
            if (reads) reads->push_back(ptr);
            continue;
        }

        if (m_events[i].events & EPOLLOUT) {
            if (writes) writes->push_back(ptr);
            continue;
        }
    }
}
