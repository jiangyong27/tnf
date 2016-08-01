// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-08-24
//
#ifndef JASONYJIANG_FILE_H
#define JASONYJIANG_FILE_H
#pragma once
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

/**打印错误信息 */
#ifndef PRINT_ERROR
#define PRINT_ERROR(str)\
    cerr <<"[ERROR]["<<__FILE__<<"]["<<__LINE__<<"]["<<__FUNCTION__<<"()] ";\
    cerr << str <<endl;
#endif

/** 打印信息 */
#ifndef PRINT_INFO
#define PRINT_INFO(str)\
    cout <<"[INFO]["<<__FILE__<<"]["<<__LINE__<<"]["<<__FUNCTION__<<"()] ";\
    cout << str <<endl;
#endif

/**打印调试信息*/
#ifndef PRINT_DEBUG
#define PRINT_DEBUG(str)\
    cout <<"[DEBUG]"<<__FILE__<<"]["<<__LINE__<<"]["<<__FUNCTION__<<"()] ";\
    cout <<str<<endl;
#endif

/** 文件常用静态操作*/
class FileUtility
{
    public:

        /**格式化写文件 */
        static bool WriteFile(const std::string& sFileName, const char* fmt, ...);
        static bool Write(const std::string& sFileName, const std::string& value);
        static bool Write(const std::string& sFileName, const int& value);
        static bool Write(const std::string& sFileName, const uint32_t& value);
        static bool Write(const std::string& sFileName, const uint64_t& value);

        /** 全量加载文件 */
        static bool LoadLines(const std::string& path,vector<std::string>* lines);
        static bool LoadString(const std::string& path,std::string* str);

        /** 文件常用操作*/
        static bool DeleteFile(const std::string& path);
        static bool DeleteDir(const std::string& path);
        static bool CreateDir(const std::string& path);
        static bool CreateLink(const std::string& src,const std::string& dst="");
        static bool Rename(const std::string& src,const std::string& dst);
        static bool Copy(const std::string& src,const std::string& dst);
        static bool Touch(const std::string& sFileName);
        static bool GetFileList(const std::string& path, std::vector<std::string>* files);

        /** 获得文件信息 */
        static int GetFileSize(const std::string& path);
        static bool GetCurrentDir(std::string* dir);
        static time_t GetModifyTime(const std::string& path);
        static time_t GetAccessTime(const std::string& path);

        /**常用判断操作 */
        static bool IsDir(const std::string& path);
        static bool IsFile(const std::string& path);
        static bool IsUserFile(const std::string& path);
        static bool IsFileExist(const std::string& path);

        static bool SplitPath(const std::string& Path, std::string *FilePath, std::string *FileName);
        static bool SplitFileName(const std::string& FileName, std::string *BaseName, std::string *SuffixName);
    private:
        static inline bool GetStat(const std::string& path,struct stat* st);
};


/** 文件类 */
class CFile
{
public:
    enum {
        MODE_READ = 0,
        MODE_WRITE,
        MODE_TRUNC
    };
public:
    CFile(const std::string& path, int flag = MODE_READ);
    ~CFile();
    void Seek(uint64_t pos);
    uint64_t Tell();
    bool Good();
    bool ReadLine(std::string* line);
    bool ReadFile(char* buf, uint32_t len);
    bool WriteFile(const std::string& line);
    bool WriteFile(const char* fmt, ...);
    bool WriteFile(const char* fmt, uint32_t len);
    bool WriteLine(const std::string& line);
    void Flush();

private:
    std::string m_path;
    std::fstream m_file;
    bool m_read;
    int m_mode;
};

#endif // FILE_H
