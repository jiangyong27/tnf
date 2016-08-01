#include "net_socket.h"
CSocket::CSocket(int socket) : m_socket(socket)
{
}
CSocket::CSocket()
{
    m_socket = -1;
}
CSocket::~CSocket()
{
    if (m_socket != -1) {
        close(m_socket);
    }
}

void CSocket::SetReuseAddr(int flag)
{
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
        perror("setsockopt failed!");
    }
}

void CSocket::SetNoblock(int flag)
{
    int opts;
    opts = fcntl(m_socket, F_GETFL);
    if(opts < 0) {
        perror("fcntl(F_GETFL)\n");
        assert(0);
    }
    if (flag == 0) {
        opts = (opts & ~O_NONBLOCK);
    } else {
        opts = (opts | O_NONBLOCK);
    }

    if(fcntl(m_socket, F_SETFL, opts) < 0) {
        perror("fcntl(F_SETFL)\n");
        assert(0);
    }
}

void CSocket::SetNodeley(int flag)
{
    if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag)) == -1) {
        perror("setsockopt TCP_NODELAY failed!");
    }
    return;
}

void CSocket::SetKeepAlive()
{
    int keepAlive = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)) == -1) {
        perror("setsocket SO_KEEP_ALIVE failed");
        return ;
    }

    int keepIdle = 300;
    int keepInterval = 10;
    int keepCount = 3;
    if (setsockopt(m_socket, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)) < 0 ) {
        perror("setsockopt");
        return ;
    }

    if (setsockopt(m_socket, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)) < 0) {
        perror("setsockopt");
        return ;
    }

    if (setsockopt(m_socket, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)) < 0 ) {
        perror("setsockopt");
        return ;
    }
}

int CSocket::SendBuf(char *data_buf, uint32_t data_len)
{
    int ret;
    while (1) {
        ret = send(m_socket, data_buf, data_len, 0);
        if (ret < 0 ) {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            }
            return -1;
        }
    }
    return ret;
}

int CSocket::RecvBuf(char *data_buf, uint32_t data_len)
{
    int ret;
    while (1) {
        ret = recv(m_socket, data_buf, data_len, 0);
        if (ret < 0 ) {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            }
            return -1;
        }
    }
    return ret;
}

/*********************************TCPSocket ****************************/
CTCPSocket::CTCPSocket()
{
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("socket failed!");
        assert(0);
    }
}

CTCPSocket::~CTCPSocket()
{
}

int CTCPSocket::Bind(uint32_t ip, uint32_t port)
{
    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = ip;
    local.sin_port = htons(port);

    if (bind(m_socket, (struct sockaddr *) &local, sizeof(local)) < 0) {
        perror("bind failed!");
        return -1;
    }
    return 0;
}

int CTCPSocket::Listen(uint32_t ip, uint16_t port, int max)
{
    if (Bind(ip, port) < 0) {
        return -1;
    }

    if (listen(m_socket, max) < 0 ) {
        perror("listen failed!");
        return -2;
    }

    return 0;
}

int CTCPSocket::Connect(uint32_t ip, uint16_t port)
{
    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = ip;
    local.sin_port = htons(port);

    if (connect(m_socket, (struct sockaddr*) &local, sizeof(local)) < 0) {
        perror("Connect!");
        return -1;
    }
    return 0;
}

CSocket* CTCPSocket::Accept()
{
    int sock = accept(m_socket, NULL, NULL);
    if (sock == -1 ) {
        if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR) {
            perror("accept failed !");
            return NULL;
        }
    }
    CSocket* socket = new CSocket(sock);
    return socket;
}
