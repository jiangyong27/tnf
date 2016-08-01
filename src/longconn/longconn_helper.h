// Copyright (c) 2013, Tencent Inc. All rights reserved.
// Author: Bo Ma <boma@tencent.com>

#ifndef ONLINE_ADAPTOR_SRC_LONGCONN_HELPER_H
#define ONLINE_ADAPTOR_SRC_LONGCONN_HELPER_H
#pragma once

#include <stdint.h>

#include "base_protocol.h"


class LongconnHelper {
public:

    int Pack(uint16_t service_type, uint32_t seq, uint16_t key, const char* value,
             uint32_t value_len, char** package, uint32_t* package_len);
    int Unpack(const char* data, uint32_t data_len, uint16_t key, char** value, uint32_t* value_len, uint16_t* service_type, uint32_t* seq);
    int Unpack(const char* data, uint32_t data_len, uint16_t key, char** value, uint32_t* value_len);
    int Unpack(const char* data, uint32_t data_len, uint16_t key, int* value);
    int Unpack(const char* data, uint32_t data_len, uint16_t* service_type, uint32_t* seq);

    LongconnHelper();
    ~LongconnHelper();

private:
    xfs::base::CBaseProtocolPack* m_packer;
    xfs::base::CBaseProtocolUnpack* m_unpacker;
};


#endif // ONLINE_ADAPTOR_SRC_LONGCONN_HELPER_H
