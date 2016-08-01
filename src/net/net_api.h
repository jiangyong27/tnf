#ifndef COMMON_API_H
#define COMMON_API_H
#pragma once

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include <iostream>

void set_sock_noblock(int sock);
void set_sock_reuseseaddr(int sock);
void set_sock_keepalive(int sock);
void set_sock_nodelay(int sock);
void set_sock_sendbuf_size(int sock, int size);
void set_sock_recvbuf_size(int sock, int size);
int get_sock_sendbuf_size(int sock);
int get_sock_recvbuf_size(int sock);

int get_sock_addr(int sock, uint32_t *ip, uint16_t *port);
int get_peer_addr(int sock, uint32_t *ip, uint16_t *port);
uint32_t get_host_ip(const std::string& host);

uint32_t str2ip(const std::string& str);
std::string ip2str(uint32_t ip);

int init_client(uint32_t addr, uint16_t port);
int init_server(uint32_t addr, uint16_t port);


#endif // COMMON_API_H
