// Copyright (c) 2013, Tencent Inc. All rights reserved.
// Author: Bo Ma <boma@tencent.com>

#include "longconn_helper.h"

using namespace std;
using xfs::base::CBaseProtocolPack;
using xfs::base::CBaseProtocolUnpack;



LongconnHelper::LongconnHelper() {
    m_packer = new CBaseProtocolPack;
    m_packer->Init();
    m_unpacker = new CBaseProtocolUnpack;
    m_unpacker->Init();
}

LongconnHelper::~LongconnHelper() {
    delete m_packer;
    m_packer = NULL;
    delete m_unpacker;
    m_unpacker = NULL;
}

int LongconnHelper::Pack(uint16_t service_type, uint32_t seq, uint16_t key, const char* value,
                         uint32_t value_len, char** package, uint32_t* package_len) {
    if (package == NULL || package_len == NULL) {
        return -1;
    }

    m_packer->ResetContent();
    m_packer->SetSeq(seq);
    m_packer->SetServiceType(service_type);

    if (!m_packer->SetKey(key, (uint8_t*)value, value_len)) {
        return -2;
    }

    m_packer->GetPackage((uint8_t**)package, package_len);
    return 0;
}

int LongconnHelper::Unpack(const char* data, uint32_t data_len,
                           uint16_t* service_type, uint32_t* seq) {
    if (data == NULL || service_type == NULL || seq == NULL) {
        return -1;
    }

    m_unpacker->UntachPackage();
    m_unpacker->AttachPackage((uint8_t*)data, data_len);
    if (!m_unpacker->Unpack()) {
        return -2;
    }

    *seq = m_unpacker->GetSeq();
    *service_type = m_unpacker->GetServiceType();

    return 0;
}

int LongconnHelper::Unpack(const char* data, uint32_t data_len, uint16_t key,
                           char** value, uint32_t* value_len) {
    if (value == NULL || value_len == NULL || data == NULL || data_len == 0) {
        return -1;
    }

    m_unpacker->UntachPackage();
    m_unpacker->AttachPackage((uint8_t*)data, data_len);
    if (!m_unpacker->Unpack()) {
        return -2;
    }

    if (!m_unpacker->GetVal(key, (uint8_t**)value, value_len)) {
        return -3;
    }

    return 0;
}

int LongconnHelper::Unpack(const char* data, uint32_t data_len, uint16_t key, int* value) {
    if (value == NULL || data == NULL || data_len == 0) {
        return -1;
    }

    m_unpacker->UntachPackage();
    m_unpacker->AttachPackage((uint8_t*)data, data_len);
    if (!m_unpacker->Unpack()) {
        return -2;
    }

    if (!m_unpacker->GetVal(key, value)) {
        return -3;
    }

    return 0;
}

int LongconnHelper::Unpack(const char* data, uint32_t data_len, uint16_t key, char** value,
                           uint32_t* value_len, uint16_t* service_type, uint32_t* seq) {
    if (value == NULL || value_len == NULL || service_type == NULL
        || seq == NULL || data == NULL || data_len == 0) {
        return -1;
    }

    m_unpacker->UntachPackage();
    m_unpacker->AttachPackage((uint8_t*)data, data_len);
    if (!m_unpacker->Unpack()) {
        return -2;
    }

    *seq = m_unpacker->GetSeq();
    *service_type = m_unpacker->GetServiceType();

    if (!m_unpacker->GetVal(key, (uint8_t**)value, value_len)) {
        return -3;
    }

    return 0;
}

