#ifndef ONLINE_ABU_SDK_NET_CHECK_H
#define ONLINE_ABU_SDK_NET_CHECK_H
#pragma once
extern "C" {
    int net_complete_longconn(const void* data, unsigned data_len);
    int net_complete_http(const void* data, unsigned data_len);
}

#endif // ONLINE_ABU_SDK_NET_CHECK_H
