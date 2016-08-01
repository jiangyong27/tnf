// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2015-01-05
//

#ifndef _HOME_UBUNTU_TEST_INI_H
#define _HOME_UBUNTU_TEST_INI_H
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <map>

class CNetConf
{
public:
    CNetConf();
    ~CNetConf();
    bool OpenIniFile(const std::string& path);

    bool GetFiledValue(const std::string& field, const std::string& key, const std::string& def, std::string* value);
    bool GetFiledValue(const std::string& field, const std::string& key, uint32_t def, uint32_t *value);
    bool GetFiledValue(const std::string& field, const std::string& key, uint32_t def, uint16_t *value);
    bool GetFiledValue(const std::string& field, const std::string& key, int def, int *value);
    bool GetFiledValue(const std::string& field, const std::string& key, bool def, bool *value);
    bool GetFiledValue(const std::string& field, const std::string& key, double def, double *value);
    bool HasFiledValue(const std::string& field, const std::string& key);
private:
    bool GetFiledValue(const std::string& field, const std::string& key, std::string* value);
    std::string m_file;
    std::map<std::string, std::string> m_kv;
};

#endif // _HOME_UBUNTU_TEST_INI_H
