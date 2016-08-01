// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2015-01-05
//

#include "net_conf.h"
CNetConf::CNetConf()
{
}

CNetConf::~CNetConf()
{
}

bool CNetConf::OpenIniFile(const std::string& path)
{
    std::ifstream inifile;
    inifile.open(path.c_str(), std::ios::in);
    if (!inifile.is_open()) {
        return false;
    }

    std::string line;
    std::string field;
    std::string::size_type pos;
    std::string::size_type pos2;

    while(getline(inifile, line)) {
        if (line[0] == '#' || line.size() == 0)
            continue;

        pos = line.find("[");
        pos2 = line.find("]");
        if (pos != std::string::npos && pos2 != std::string::npos && pos2 > pos) {
            field = line.substr(pos+1, pos2-pos-1);
            continue;
        }
        std::string::size_type pos = line.find("=");
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        m_kv[field + "-" + key] = value;
    }
    inifile.close();
    return true;
}

bool CNetConf::HasFiledValue(const std::string& field, const std::string& key)
{
    std::map<std::string, std::string>::iterator iter;
    iter = m_kv.find(field + "-" + key);
    if (iter == m_kv.end()) {
        return false;
    }
    return true;
}

bool CNetConf::GetFiledValue(const std::string& field, const std::string& key, std::string* value)
{
    std::map<std::string, std::string>::iterator iter;
    iter = m_kv.find(field + "-" + key);
    if (iter == m_kv.end()) {
        return false;
    }
    value->assign(iter->second);
    return true;
}

bool CNetConf::GetFiledValue(const std::string& field, const std::string& key, const std::string& def, std::string* value)
{
    if (!GetFiledValue(field, key, value)) {
        value->assign(def);
        return false;
    }
    return true;
}

bool CNetConf::GetFiledValue(const std::string& field, const std::string& key, uint32_t def, uint16_t *value)
{
    std::string s;
    if (!GetFiledValue(field, key, &s)) {
        *value = def;
        return false;
    } else {
        *value = atoi(s.c_str());
        return true;
    }
}

bool CNetConf::GetFiledValue(const std::string& field, const std::string& key, uint32_t def, uint32_t *value)
{
    std::string s;
    if (!GetFiledValue(field, key, &s)) {
        *value = def;
        return false;
    } else {
        *value = atoi(s.c_str());
        return true;
    }
}

bool CNetConf::GetFiledValue(const std::string& field, const std::string& key, int def, int *value)
{
    std::string s;
    if (!GetFiledValue(field, key, &s)) {
        *value = def;
        return false;
    }else{
        *value = atoi(s.c_str());
        return true;
    }
}

bool CNetConf::GetFiledValue(const std::string& field, const std::string& key, bool def, bool *value)
{
    std::string s;
    if (!GetFiledValue(field, key, &s)) {
        *value = def;
        return false;
    } else {
        *value = atoi(s.c_str()) == 0 ? false : true;
        return true;
    }
}

bool CNetConf::GetFiledValue(const std::string& field, const std::string& key, double def, double *value)
{
    std::string s;
    if (!GetFiledValue(field, key, &s)) {
        *value = def;
        return false;
    } else {
        *value = atof(s.c_str());
        return true;
    }
}
