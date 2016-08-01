// Copyright (c) 2013, Tencent Inc. All rights reserved.
#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern "C" {

const static unsigned PKT_MAX_LEN = (32<<20);

int net_complete_longconn(const void* data, unsigned data_len)
{
    unsigned total_len = 0;

    if ( data_len < 4 ) {
        return 0;
    }

    total_len = *((unsigned *)data);

    total_len = ntohl(total_len);

    if ( total_len > PKT_MAX_LEN ) {
        return -1;
    }

    if ( data_len < total_len ) {
        return 0;
    }

    return total_len;
}

int net_complete_http(const void* data, unsigned data_len)
{
    char *p, *q;
    if(data_len < 14)
        return 0;

    if((p = strstr((char*)data, "\r\n\r\n")) != NULL) {
        if((q = strstr((char*)data, "Content-Length:")) == NULL) {  //no http body
            return p - (char*)data + 4;
        }
        else {                                                      //has http body
            unsigned pkglen = p - (char*)data + 4 + atoi(q + 15);
            if(data_len >= pkglen) {
                return pkglen;
            }
            else {
                return 0;
            }
        }
    }
    else if(data_len > 10240000) {
        return -1;
    }
    else {
        return 0;
    }
}


}
