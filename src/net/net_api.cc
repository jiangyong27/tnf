#include "net_api.h"
void set_sock_reuseseaddr(int sock)
{
    int flag = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
        perror("setsockopt failed!");
    }
}

void set_sock_noblock(int sockfd)
{
    int opts;
    opts = fcntl(sockfd, F_GETFL);
    if(opts < 0) {
        perror("fcntl(F_GETFL)\n");
        assert(0);
    }
    opts = (opts | O_NONBLOCK);
    if(fcntl(sockfd, F_SETFL, opts) < 0) {
        perror("fcntl(F_SETFL)\n");
        assert(0);
    }
}

void set_sock_nodelay(int sock)
{
    int flag = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag)) == -1) {
        perror("setsockopt TCP_NODELAY failed!");
    }
    return;
}

void set_sock_keepalive(int sock)
{
    int keepAlive = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)) == -1) {
        perror("setsocket SO_KEEP_ALIVE failed");
        return ;
    }

    int keepIdle = 300;
    int keepInterval = 10;
    int keepCount = 3;
    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)) < 0 ) {
        perror("setsockopt");
        return ;
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)) < 0) {
        perror("setsockopt");
        return ;
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)) < 0 ) {
        perror("setsockopt");
        return ;
    }
}

int get_sock_sendbuf_size(int sock)
{
    int buf_size;
    socklen_t optlen = sizeof(buf_size);
    if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF,&buf_size, &optlen) < 0) {
        perror("get socketopt send_buf size faile!");
        return -1;
    }
    return buf_size;
}

int get_sock_recvbuf_size(int sock)
{
    int buf_size;
    socklen_t optlen = sizeof(buf_size);
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF,&buf_size, &optlen) < 0) {
        perror("get socketopt send_buf size faile!");
        return -1;
    }
    return buf_size;
}


std::string ip2str(uint32_t ip)
{
    char ip_str[20];
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip;
    if(inet_ntop(AF_INET,&addr.sin_addr.s_addr,ip_str,sizeof(ip_str)) == NULL){
        return "";
    }
    return ip_str;
}

uint32_t str2ip(const std::string& str)
{
    struct in_addr s;
    inet_pton(AF_INET, str.c_str(),(void*)&s);
    return s.s_addr;
}

uint32_t get_host_ip(const std::string& host)
{
    struct hostent hostbuf, *hp;
    char tmphstbuf[1024];
    int res;
    int herr;
    uint32_t ip;

    res = gethostbyname_r(host.c_str(), &hostbuf, tmphstbuf, 1024, &hp, &herr);
    if (res || hp == NULL || hp->h_addr == NULL) {
        perror("gethostbyname_r");
        return 0;
    }

    memcpy(&ip, hp->h_addr, 4 );
    return ip;
}

int get_peer_addr(int sock, uint32_t* ip, uint16_t *port)
{
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    int ret = getpeername(sock, &addr, &addr_len);
    if (ret != 0) {
        perror("getpeername");
        return ret;
    }
    struct sockaddr_in *in = (struct sockaddr_in*) &addr;

    *ip = in->sin_addr.s_addr;
    *port = ntohs(in->sin_port);
    return ret;
}

int get_sock_addr(int sock, uint32_t* ip, uint16_t *port)
{
    struct sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    int ret = getsockname(sock, &addr, &addr_len);
    if (ret != 0) {
        perror("getsockname");
        return ret;
    }
    struct sockaddr_in *in = (struct sockaddr_in*) &addr;

    *ip = in->sin_addr.s_addr;
    *port = ntohs(in->sin_port);
    return ret;
}

int init_client(uint32_t addr, uint16_t port)
{
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("socket failed!");
        assert(0);
    }

    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = addr;
    local.sin_port = htons(port);

    if (connect(fd, (struct sockaddr*) &local, sizeof(local)) < 0) {
        close(fd);
        perror("Connect!");
        return -1;
    }
    return fd;
}

int init_server(uint32_t addr, uint16_t port)
{
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("socket failed!");
        assert(0);
    }

    //set_sock_noblock(fd);
    //set_sock_reuseseaddr(fd);

    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = addr;
    local.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &local, sizeof(local)) < 0) {
        perror("bind failed!");
        assert(0);
    }

    if (listen(fd, 20) < 0 ) {
        perror("listen failed!");
        assert(0);
    }
    return fd;
}

